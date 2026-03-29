#include "iot_mqtt.h"
#include "cJSON.h"
//是否收到 MQTT 消息
extern __IO uint8_t IsReceiveMQTTMsg;
struct __mqtt_receive_info mqtt_recv_info;

uint8_t ut_rx[MAX_RX_BUFF] = {0}; //串口接收缓冲区（原始数据）
char CHAR_AT_BUFF[MAX_AT_BUFF] = {0}; //存储 AT 指令返回字符串
__IO uint16_t num_rx = 0;         //当前接收到的数据长度
//通过 AT 指令控制 WiFi 模块实现 MQTT 通信
const char CHAR_RST[] = "AT+RST\r\n";   //重启模块
const char CHAR_CWMODE[] = "AT+CWMODE=1\r\n"; //设置为 STA模式（客户端）
const char CHAR_ATE0[] = "ATE0\r\n";    //关闭 AT 指令回显
const char CHAR_CIPSNTPCFG[] = "AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n"; //设置网络时间服务器 1：启用 8：时区（中国）ntp服务器：阿里云
const char CHAR_MQTTUSERCFG1[] = "AT+MQTTUSERCFG=0,1,\"NULL\",\"用户名\",\"用户密码\",0,0,\"\"\r\n";  //设置 MQTT 登录信息
const char CHAR_MQTTUSERCFG2[] = "AT+MQTTCLIENTID=0,\"你的clientID\"\r\n";  //设置 ClientID MQTT 客户端唯一标识
const char CHAR_MQTTCNNT[] = "AT+MQTTCONN=0,\"你的mqttHostURL\",1883,1\r\n";  //连接 MQTT 服务器
const char CHAR_MQTTSUB[] = "AT+MQTTSUB=0,\"订阅主题\",1\r\n";  //订阅主题
const char CHAR_MQTTPUB_TEST[] = "AT+MQTTPUB=0,\"推送主题\",\"test\",1,0\r\n";  //发布测试消息

/// 整数幂函数 b 的 e 次方（b^e）
uint32_t indexINT(uint8_t b, uint8_t e) {
  uint32_t res = 1;
  uint8_t i = 0;
  for (; i < e; i++) {
    res *= b;
  }
  return res;
}

/// 字符串转整数
uint32_t strToINT(const char *ptr, uint16_t size) {
  uint32_t INT = 0;
  uint16_t i;
  if (size) {
    for (i = 0; i < size; i++) {
      INT += ((ptr[i] - '0') * indexINT(10, size - i - 1));
    }
  }
  return INT;
}

/// 查找子字符串 在字符串 str 中查找 subStr，如果找到就返回位置
int findSubstring(const char *str, const char *subStr, int *position) {
  char *found = strstr(str, subStr); // 在str中查找子字符串
  if (found != NULL) {
    *position = found - str; // 计算子字符串在str中的位置
    return 1; // 找到子字符串，返回真
  } else {
    return 0; // 找不到子字符串，返回假
  }
}

/// 查找Json字符串 从一段字符串中提取出完整的 JSON 数据
//在字符串中找到一个完整的 { ... } JSON，并返回它的位置和长度
//+MQTTSUBRECV:0,"topic",23,{"temp":25,"hum":60}  ==>  {"temp":25,"hum":60}
bool find_json(const char *str, int *position, int *length) {
  int str_len = strlen(str);
  int i;
  int start = -1, end = -1;
  int brace_count = 0;

  for (i = 0; i < str_len; i++) {
    if (str[i] == '{') {
      if (start == -1) {
        start = i;
      }
      brace_count++;
    } else if (str[i] == '}') {
      brace_count--;
      if (brace_count == 0) {
        end = i;
        break;
      }
    }
  }

  if (start != -1 && end != -1) {
    *position = start;
    *length = end - start + 1;
    return true;
  } else {
    *position = -1;
    *length = 0;
    return false;
  }
}

/// 通过串口发送 AT 指令，让 WiFi 模块自己去连接网络
//0 => 成功; 1 => 关闭回显失败; 2 => 设置模式失败; 3 => 连接 wifi失败;
uint8_t WiFi_connect(const char *ssid, const char *psd) {
  HAL_UART_Transmit(&huart3, (uint8_t *) CHAR_RST, strlen(CHAR_RST), 1000);
  delay_ms(2000);

  /// 关闭回显
  //if (ut_AT_send((uint8_t *) CHAR_ATE0, strlen(CHAR_ATE0), 200) == false) {
  //  return 1;
  //}

  /// 设置模式
  if (ut_AT_send((uint8_t *) CHAR_CWMODE, strlen(CHAR_CWMODE), 200) == false) {
    return 2;
  }

  /// 连接 wifi名称和密码
  memset(CHAR_AT_BUFF, 0, MAX_AT_BUFF);
  sprintf(CHAR_AT_BUFF, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, psd);
  if (ut_AT_send((uint8_t *) CHAR_AT_BUFF, strlen(CHAR_AT_BUFF), 3000) == false) {
    return 3;
  }
  return 0;
}

/// 连接MQTT   配置 MQTT → 连接服务器 → 订阅主题 → 测试发布
uint8_t mqtt_connect(void) {
  /// 时区配置
  if (ut_AT_send((uint8_t *) CHAR_CIPSNTPCFG, strlen(CHAR_CIPSNTPCFG), 200) == false) {
    return 1;
  }

  /// 复制MQTT配置
  if (ut_AT_send((uint8_t *) CHAR_MQTTUSERCFG1, strlen(CHAR_MQTTUSERCFG1), 300) == false) {
    return 2;
  }
  if (ut_AT_send((uint8_t *) CHAR_MQTTUSERCFG2, strlen(CHAR_MQTTUSERCFG2), 300) == false) {
    return 3;
  }

  ///开始连接
  if (ut_AT_send((uint8_t *) CHAR_MQTTCNNT, strlen(CHAR_MQTTCNNT), 2000) == false) {
    return 4;
  }
  printf("MQTT connect OK\r\n");

  /// 开启服务器订阅
  if (ut_AT_send((uint8_t *) CHAR_MQTTSUB, strlen(CHAR_MQTTSUB), 300) == false) {
    return 5;
  }
  printf("MQTT subscribe OK\r\n");

  /// 发布测试
  if (ut_AT_send((uint8_t *) CHAR_MQTTPUB_TEST, strlen(CHAR_MQTTPUB_TEST), 300) == false) {
    return 6;
  }
  printf("MQTT publish test OK\r\n");

  return 0;
}

/// 把本地数据 → 拼成 JSON → 用 AT+MQTTPUB 发到云端
//上传传感器数据
void mqtt_data_update(uint8_t airtemp, uint8_t soilhumid, uint8_t airlight) {
  memset(CHAR_AT_BUFF, 0, MAX_AT_BUFF);
  sprintf(CHAR_AT_BUFF,
          "AT+MQTTPUB=0,\"/sys/idirU2f0dPU/esp8266dev/thing/event/property/post\",\"{\\\"sys\\\":{\\\"ack\\\":0}\\,\\\"params\\\":{\\\"AirTemp\\\":%d\\,\\\"SoilHumid\\\":%d\\,\\\"AirLight\\\":%d\\}\\,\\\"method\\\":\\\"thing.event.property.post\\\"}\",1,0\r\n",
          airtemp, soilhumid, airlight);
  HAL_UART_Transmit(&huart3, CHAR_AT_BUFF, strlen(CHAR_AT_BUFF), 1000);
}
//上传设备状态
void mqtt_sta_update(uint8_t sta_pump, uint8_t sta_bulb, uint8_t sta_beep) {
  memset(CHAR_AT_BUFF, 0, MAX_AT_BUFF);
  sprintf(CHAR_AT_BUFF,
          "AT+MQTTPUB=0,\"/sys/idirU2f0dPU/esp8266dev/thing/event/property/post\",\"{\\\"sys\\\":{\\\"ack\\\":0}\\,\\\"params\\\":{\\\"StaBulb\\\":%d\\,\\\"StaPump\\\":%d\\,\\\"StaBeep\\\":%d\\}\\,\\\"method\\\":\\\"thing.event.property.post\\\"}\",1,0\r\n",
          sta_bulb, sta_pump, sta_beep);
  HAL_UART_Transmit(&huart3, CHAR_AT_BUFF, strlen(CHAR_AT_BUFF), 1000);
}
//上传阈值参数
void mqtt_threshold_update(uint8_t airtempup, uint8_t soilhumiddown, uint8_t lightdown) {
  memset(CHAR_AT_BUFF, 0, MAX_AT_BUFF);
  sprintf(CHAR_AT_BUFF,
          "AT+MQTTPUB=0,\"/sys/idirU2f0dPU/esp8266dev/thing/event/property/post\",\"{\\\"sys\\\":{\\\"ack\\\":0}\\,\\\"params\\\":{\\\"AirTempUp\\\":%d\\,\\\"SoilHumidDown\\\":%d\\,\\\"AirLightDown\\\":%d}\\,\\\"method\\\":\\\"thing.event.property.post\\\"}\",1,0\r\n",
          airtempup, soilhumiddown, lightdown);
  HAL_UART_Transmit(&huart3, CHAR_AT_BUFF, strlen(CHAR_AT_BUFF), 1000);
}

/// 监听串口缓冲区 监听串口数据，提取 MQTT 下发的 JSON，并交给解析函数处理
void mqtt_monitor(void) {
  int pos1 = 0;
  int pos2 = 0;
  int DataJsonLen = 0;

  /** 检查串口缓冲区非空 **/
  if (ut_rx[0] != '\0') {
    if (findSubstring((char *) ut_rx, "+MQTTSUBRECV", &pos1)) {
      if (find_json((char *) &ut_rx[pos1], &pos2, &DataJsonLen)) {

        /// 拷贝json数据
        memset(CHAR_AT_BUFF, 0, MAX_AT_BUFF);
        memcpy(CHAR_AT_BUFF, &ut_rx[pos1 + pos2], DataJsonLen);
        decode_mqtt_json(CHAR_AT_BUFF);
        printf("%s\r\n", CHAR_AT_BUFF);

        memset(ut_rx, 0, MAX_RX_BUFF);    /// 清空缓冲
        num_rx = 0;
      } else {

        /// json 数据不完整 数据没有 接收 报文 刷新缓冲区
        memset(ut_rx, 0, MAX_RX_BUFF);
        num_rx = 0;
      }
    } else {

      /// 数据没有 接收 报文 刷新缓冲区
      memset(ut_rx, 0, MAX_RX_BUFF);
      num_rx = 0;
    }
  }
}

/// 解析Json数据  把 JSON 字符串解析成结构体 mqtt_recv_info 中的各个变量
void decode_mqtt_json(char *str) {
  cJSON *params_obj;

  /** 创建Json对象 **/
  cJSON *root = cJSON_Parse(str);

  if (root == NULL)
    printf("json root error\r\n");

  /** 逐个检查相应参数 **/
  cJSON *mqtt_params = cJSON_GetObjectItem(root, "params");
  if (cJSON_IsObject(mqtt_params)) {
    //获取对象节点
    params_obj = cJSON_GetObjectItem(mqtt_params, "AirTemp");
    //判断类型
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      //读取值
      mqtt_recv_info.IsAirTempData = true;
      mqtt_recv_info.AirTemp = params_obj->valueint;
    } else mqtt_recv_info.IsAirTempData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "SoilHumid");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsSoilHumidData = true;
      mqtt_recv_info.SoilHumid = params_obj->valueint;
    } else mqtt_recv_info.IsSoilHumidData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "AirLight");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsAirLightData = true;
      mqtt_recv_info.AirLight = params_obj->valueint;
    } else mqtt_recv_info.AirLight = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "StaBulb");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsStateBulbData = true;
      mqtt_recv_info.StateBulb = params_obj->valueint;
    } else mqtt_recv_info.IsStateBulbData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "Stabeep");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsStateBeepData = true;
      mqtt_recv_info.StateBeep = params_obj->valueint;
    } else mqtt_recv_info.IsStateBeepData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "StaPump");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsStatePumpData = true;
      mqtt_recv_info.StatePump = params_obj->valueint;
    } else mqtt_recv_info.IsStatePumpData = false;
    
    params_obj = cJSON_GetObjectItem(mqtt_params, "AirLightDown");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsLightDownData = true;
      mqtt_recv_info.LightDown = params_obj->valueint;
    } else mqtt_recv_info.IsLightDownData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "SoilHumidDown");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsSoilHumidDownData = true;
      mqtt_recv_info.SoilHumidDown = params_obj->valueint;
    } else mqtt_recv_info.IsSoilHumidDownData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "AirTempUp");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsAirTempUpData = true;
      mqtt_recv_info.AirTempUp = params_obj->valueint;
    } else mqtt_recv_info.IsAirTempUpData = false;

    params_obj = cJSON_GetObjectItem(mqtt_params, "IsIoTControl");
    if (cJSON_IsNumber(params_obj)) {
      IsReceiveMQTTMsg = 1;
      mqtt_recv_info.IsIotControl = true;
      mqtt_recv_info.IotControl = params_obj->valueint;
    } else mqtt_recv_info.IsIotControl = false;
  } else printf("params error\r\n");

  /** 删除对象，释放内存 **/
  cJSON_Delete(root);
}

/// 串口发送AT命令，并检查应答 发送 AT 指令 + 等待返回 + 判断是否成功
bool ut_AT_send(uint8_t *ptr, uint16_t size, uint16_t wait_time) {
  uint16_t i = 0;
  uint32_t st;
  bool res;

  num_rx = 0;
  memset(ut_rx, 0, MAX_RX_BUFF);
  HAL_UART_Transmit(&huart3, ptr, size, 1000);
  while (1) {

    /// 找到结束
    if (ut_rx[i] == '\r' && ut_rx[i + 1] == '\n' && ut_rx[i + 2] == '\0') {

      /// 找到末尾 等待是否 50ms内 还有数据
      st = HAL_GetTick();
      while ((HAL_GetTick() - st) < wait_time && ut_rx[i + 2] == '\0');

      /// 超时真正结束
      if ((HAL_GetTick() - st) >= wait_time) {
        break;
      }
    }

    i++;
    if (i == (MAX_RX_BUFF - 3))
      i = 0;
  }
  printf("%s", ut_rx);

  /// 查找最后 四个字节是否 OK
  if (ut_rx[i - 2] == 'O' && ut_rx[i - 1] == 'K') {
    res = true;
  } else res = false;

  /** 清空缓冲区 */
  num_rx = 0;
  memset(ut_rx, 0, MAX_RX_BUFF);
  return res;
}

/// 串口接收中断回调函数 每收到一个字节就触发一次
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  static uint8_t ch = 0;
  if (huart->Instance == USART3) {

    /// 读取串口数据
    ut_rx[num_rx] = (uint8_t) USART3->DR;

    /** 限制大小 **/
    num_rx++;
    if (num_rx == MAX_RX_BUFF) num_rx = 0;

    /** 开启下次中断完成 **/
    HAL_UART_Receive_IT(&huart3, &ch, 1);
  }
}
