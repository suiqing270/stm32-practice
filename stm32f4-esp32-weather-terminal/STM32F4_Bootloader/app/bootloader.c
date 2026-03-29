#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "bl_usart.h"
#include "ringbuffer.h"
#include "crc16.h"
#include "crc32.h"
#include "tim_delay.h"
#include "stm32_flash.h"
#include "board.h"
#include "magic_header.h"

#define BL_VERSION      "0.0.1"
#define BL_ADDRESS      0x08000000
#define BL_SIZE         (48 * 1024) //Bootloader 占用大小 48KB
#define BOOT_DELAY      3000        //延时 3000ms，用于在启动时判断是否进入Bootloader或直接跳应用
#define APP_VTOR_ADDR   0x08010000  //应用程序入口地址，Bootloader 完成后跳转的地方
#define RX_BUFFER_SIZE  (5 * 1024)  //接收环形缓冲区大小 5KB
#define RX_TIMEOUT_MS   20          //接收超时时间 20ms
#define PAYLOAD_SIZE_MAX (4096 + 8) // 4096为Program最大数据长度，8为Program指令的地址(4)和长度(4)
                                    //一次最大可接收的 Program 数据包长度 = 4096 数据 + 8 地址/长度。
#define PACKET_SIZE_MAX (4 + PAYLOAD_SIZE_MAX + 2) // header(1) + opcode(1) + length(2) + payload + crc16(2)
                                    //最大完整数据包大小 = 包头(4) + payload + CRC16(2)
//Bootloader 接收串口数据时使用状态机解析数据包
typedef enum
{
    PACKET_STATE_HEADER,    //等待数据包头。
    PACKET_STATE_OPCODE,    //解析操作码（如擦写、烧写、跳转）。
    PACKET_STATE_LENGTH,    //解析 payload 长度。
    PACKET_STATE_PAYLOAD,   //读取 payload 数据。
    PACKET_STATE_CRC16,     //校验包完整性。
} packet_state_machine_t;
//数据包操作码定义
typedef enum
{
    PACKET_OPCODE_INQUERY = 0x01,   //查询Bootloader信息（版本、MTU）。
    PACKET_OPCODE_ERASE = 0x81,     //擦除Flash。
    PACKET_OPCODE_PROGRAM = 0x82,   //写入Flash。
    PACKET_OPCODE_VERIFY  = 0x83,   //验证写入的数据
    PACKET_OPCODE_RESET = 0x21,     //复位 MCU。
    PACKET_OPCODE_BOOT = 0x22,      //跳转到应用程序。
} packet_opcode_t;
//查询子码
typedef enum
{
    INQUERY_SUBCODE_VERSION = 0x00, //获取 Bootloader 版本。
    INQUERY_SUBCODE_MTU = 0x01,     //获取最大数据包长度。
} inquery_subcode_t;
//Bootloader 执行操作后返回给上位机的状态码：
typedef enum
{
    PACKET_ERRCODE_OK = 0,
    PACKET_ERRCODE_OPCODE,          //无效操作码。
    PACKET_ERRCODE_OVERFLOW,        //数据超出缓冲。
    PACKET_ERRCODE_TIMEOUT,         //接收超时。
    PACKET_ERRCODE_FORMAT,          //数据包格式错误。
    PACKET_ERRCODE_VERIFY,          //校验失败。
    PACKET_ERRCODE_PARAM,           //参数错误。
    PACKET_ERRCODE_UNKNOWN = 0xff,  //未知错误。
} packet_errcode_t;

static uint8_t rb_buffer[RX_BUFFER_SIZE];   //环形缓冲区的底层数组。
static rb_t rxrb;                           //环形缓冲区结构体，用于管理读写指针、环形逻辑。

static uint8_t packet_buffer[PACKET_SIZE_MAX];  //存放当前解析的数据包。
static uint32_t packet_index;                   //数据包当前填充位置。
static packet_state_machine_t packet_state = PACKET_STATE_HEADER;   //当前状态机状态。
static packet_opcode_t packet_opcode;           //当前数据包操作码。
static uint16_t packet_payload_length;          //当前数据包 payload 长度。
//检查 Flash 中存储的应用程序是否完整有效
static bool application_validate(void)
{   
    //快速判断 Flash 中是否存在合法的应用程序
    if (!magic_header_validate())   //检查应用程序的起始位置是否有特定的 魔术标识，这个标识通常是 Bootloader 写入时添加的
    {   
        printf("magic header invalid\n");
        return false;
    }
    //获取应用程序信息
    uint32_t addr = magic_header_get_address();
    uint32_t size = magic_header_get_length();
    uint32_t crc = magic_header_get_crc32();
    //使用 crc32() 对 Flash 中应用程序的实际内容进行计算
    //和魔术头里保存的 CRC32 比对：一致 → 应用程序完整。
    uint32_t ccrc = crc32((uint8_t *)addr, size);
    if (crc != ccrc)
    {
        printf("application crc error: expected %08X, got %08X\n", crc, ccrc);
        return false;
    }

    return true;
}
//在 Bootloader 验证通过后，把 MCU 控制权交给应用程序
//可以理解为：Bootloader “安全退出”，把 MCU 完全交给应用程序。
static void boot_application(void)
{
    //调用前面定义的 application_validate()。 如果校验失败：打印信息。返回，Bootloader 不跳转，防止运行损坏程序。
    if (!application_validate())
    {
        printf("application validate failed, cannot boot\n");
        return;
    }

    printf("booting application...\n");
    tim_delay_ms(2);
    //停用外设 / 清理状态
    //确保应用程序启动时，硬件外设处于干净状态，不会被 Bootloader 剩余状态干扰。
    led_off(led1);  //关闭状态指示灯
    TIM_DeInit(TIM6);   //把 Bootloader 使用的定时器和串口外设复位
    USART_DeInit(USART1);
    USART_DeInit(USART3);
    NVIC_DisableIRQ(TIM6_DAC_IRQn); //禁用 Bootloader 使用的中断
    NVIC_DisableIRQ(USART1_IRQn);
    NVIC_DisableIRQ(USART3_IRQn);
    //应用程序的中断和异常会指向自己的向量表，而不是 Bootloader 的
    SCB->VTOR = APP_VTOR_ADDR;
    extern void JumpApp(uint32_t base);
    //跳转到应用程序
    JumpApp(APP_VTOR_ADDR);
}
/**
 * @brief 把操作结果和数据打包成 Bootloader 协议格式 并通过串口发送
 * @param 
 *          opcode：操作码，对应上位机请求的命令类型（如 INQUERY、ERASE）
 *          errcode：响应状态码，表示操作结果（如 OK、VERIFY）
 *          data：可选的附加数据，例如版本号或 MTU
 *          length：附加数据长度
 * @return 
 */
//这就像 Bootloader 给上位机回信，告诉“我收到了你发的命令，状态如何，还有你要的数据”，而且信里附了校验码保证内容没错。
static void bl_response(packet_opcode_t opcode, packet_errcode_t errcode, const uint8_t *data, uint16_t length)
{
    uint8_t *response = packet_buffer;  //使用全局 packet_buffer 作为发送缓冲区，避免重复分配内存
    //构建协议头
    response[0] = 0x55;     // 数据包头
    response[1] = opcode;   // 操作码
    response[2] = errcode;  // 错误码
    response[3] = (uint8_t)(length & 0xFF);         // payload长度低字节
    response[4] = (uint8_t)((length >> 8) & 0xFF);  // payload长度高字节
    if (length > 0) memcpy(&response[5], data, length); //把 data 数据拷贝到 response[5] 位置 length 可以为 0，表示没有附加数据
    //使用 CRC16 对 Header + Opcode + ErrCode + Length + Payload 进行校验
    //校验结果附加在数据包末尾（2字节）
    //协议完整性保证：上位机接收后可校验数据是否正确
    uint16_t crc = crc16(response, 5 + length);
    response[5 + length] = (uint8_t)(crc & 0xFF);
    response[6 + length] = (uint8_t)((crc >> 8) & 0xFF);
    //调用 bl_usart_write() 发送整个数据包
    bl_usart_write(response, 7 + length);
}

//static inline void bl_response_ack(packet_opcode_t opcode, packet_errcode_t errcode)
//{
//    bl_response(opcode, errcode, NULL, 0);
//}

//上位机发送查询命令，Bootloader 返回版本号或能力信息（如 MTU）
//上位机问“你是谁？能发多大数据？” → Bootloader 回答
static void bl_opcode_inquery_handler(void)
{
    printf("inquery handler\n");
    //检查数据长度是否合法
    if (packet_payload_length != 1)
    {
        printf("inquery packet length error\n");
        return;
    }
    //读取子命令（subcode）
    //数据包结构回顾[0] header [1] opcode [2] length_L [3] length_H [4] payload (第1字节)
    //所以：packet_buffer[4] → payload 第一个字节 → subcode
    uint8_t subcode = packet_buffer[4];
    //根据 subcode 分发处理 Bootloader 支持两个查询子命令
    switch (subcode)
    {   //查询版本号
        case INQUERY_SUBCODE_VERSION:
        {
            //返回 Bootloader 版本字符串 "0.0.1"
            bl_response(PACKET_OPCODE_INQUERY, PACKET_ERRCODE_OK, (const uint8_t *)BL_VERSION, strlen(BL_VERSION));
            break;
        }
        //查询 MTU（最大传输单元）
        case INQUERY_SUBCODE_MTU:
        {   //返回最大 payload 长度（MTU）
            //用 小端格式拆成两个字节
            uint8_t bmtu[2] = {PAYLOAD_SIZE_MAX & 0xFF, (PAYLOAD_SIZE_MAX >> 8) & 0xFF};
            bl_response(PACKET_OPCODE_INQUERY, PACKET_ERRCODE_OK, (const uint8_t *)&bmtu, sizeof(bmtu));
            break;
        }
        default:
        {
            printf("unknown inquery subcode: %02X\n", subcode);
            break;
        }
    }
}
//这一段代码是 Bootloader 中 擦除 Flash（ERASE 指令） 的处理函数，
//是固件升级流程里非常关键的一步（在写入程序之前必须先擦除）
//根据上位机给的地址和长度，擦除指定的 Flash 区域（但保护 Bootloader 自己）。
static void bl_opcode_erase_handler(void)
{
    printf("erase handler\n");
    //检查 payload 长度 [4 bytes] address [4 bytes] size
    if (packet_payload_length != 8)
    {
        printf("erase packet length error: %d\n", packet_payload_length);
        bl_response(PACKET_OPCODE_ERASE, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }
    //解析 address 和 size
    //packet_buffer:[0]header [1]opcode [2]len_L [3]len_H [4~7]address [8~11]size
    uint32_t address = (packet_buffer[7] << 24) | (packet_buffer[6] << 16) | (packet_buffer[5] << 8) | packet_buffer[4];
    uint32_t size = (packet_buffer[11] << 24) | (packet_buffer[10] << 16) | (packet_buffer[9] << 8) | packet_buffer[8];
    //检查地址是否合法  小于 Flash 起始地址    超出 Flash 末尾
    if (address < STM32_FLASH_BASE || address + size > STM32_FLASH_BASE + STM32_FLASH_SIZE)
    {
        printf("erase address=0x%08X, size=%u out of range\n", address, size);
        bl_response(PACKET_OPCODE_ERASE, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }
    //保护 Bootloader 区域   如果擦除范围落在 Bootloader 区域
    if (address >= BL_ADDRESS && address < BL_ADDRESS + BL_SIZE)
    {
        printf("address 0x%08X is protected\n", address);
        bl_response(PACKET_OPCODE_ERASE, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }

    printf("erase address=0x%08X, size=%u\n", address, size);
    //执行 Flash 擦除
    stm32_flash_unlock();
    stm32_flash_erase(address, size);
    stm32_flash_lock();
    //返回成功响应
    bl_response(PACKET_OPCODE_ERASE, PACKET_ERRCODE_OK, NULL, 0);
}
//把固件数据真正写进 Flash。它和前面的 ERASE 一起构成完整升级流程的主体
//把上位机发来的固件数据写入指定 Flash 地址。
static void bl_opcode_program_handler(void)
{
    printf("program handler\n");
    //前 8 字节是“控制信息”，后面才是数据  ≤8 → 没有数据
    if (packet_payload_length <= 8)
    {
        printf("program packet length error: %d\n", packet_payload_length);
        bl_response(PACKET_OPCODE_PROGRAM, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }
    //解析 address / size / data
    uint32_t address = (packet_buffer[7] << 24) | (packet_buffer[6] << 16) | (packet_buffer[5] << 8) | packet_buffer[4];
    uint32_t size = (packet_buffer[11] << 24) | (packet_buffer[10] << 16) | (packet_buffer[9] << 8) | packet_buffer[8];
    uint8_t *data = &packet_buffer[12];
    //检查 Flash 地址范围
    if (address < STM32_FLASH_BASE || address + size > STM32_FLASH_BASE + STM32_FLASH_SIZE)
    {
        printf("program address=0x%08X, size=%u out of range\n", address, size);
        bl_response(PACKET_OPCODE_PROGRAM, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }
    //保护 Bootloader 区域
    if (address >= BL_ADDRESS && address < BL_ADDRESS + BL_SIZE)
    {
        printf("address 0x%08X is protected\n", address);
        bl_response(PACKET_OPCODE_PROGRAM, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }
    //校验 size 是否一致（非常关键）    payload长度 = 8 + data长度
    if (size != packet_payload_length - 8)
    {
        printf("program size %u does not match payload length %u\n", size, packet_payload_length - 8);
        bl_response(PACKET_OPCODE_PROGRAM, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }

    printf("program address=0x%08X, size=%u\n", address, size);
    //执行 Flash 写入
    stm32_flash_unlock();
    stm32_flash_program(address, data, size);
    stm32_flash_lock();
    //返回成功
    bl_response(PACKET_OPCODE_PROGRAM, PACKET_ERRCODE_OK, NULL, 0);
}
//Bootloader 升级流程中的最终校验步骤（VERIFY 指令），它的作用是确保刚刚写入 Flash 的固件是完整且正确的
//PROGRAM 是“写进去”，VERIFY 是“确认没写坏”。
//对指定 Flash 区域计算 CRC32，并与上位机提供的 CRC 比较
//PROGRAM 用 CRC16（通信校验）  VERIFY 用 CRC32（数据完整性）
static void bl_opcode_verify_handler(void)
{
    printf("verify handler\n");
    //[4 bytes] address [4 bytes] size [4 bytes] crc32 共 12 字节
    if (packet_payload_length != 12)
    {
        printf("verify packet length error: %d\n", packet_payload_length);
        bl_response(PACKET_OPCODE_VERIFY, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }

    uint32_t address = (packet_buffer[7] << 24) | (packet_buffer[6] << 16) | (packet_buffer[5] << 8) | packet_buffer[4];
    uint32_t size = (packet_buffer[11] << 24) | (packet_buffer[10] << 16) | (packet_buffer[9] << 8) | packet_buffer[8];
    uint32_t crc = (packet_buffer[15] << 24) | (packet_buffer[14] << 16) | (packet_buffer[13] << 8) | packet_buffer[12];

    if (address < STM32_FLASH_BASE || address + size > STM32_FLASH_BASE + STM32_FLASH_SIZE)
    {
        printf("verify address=0x%08X, size=%u out of range\n", address, size);
        bl_response(PACKET_OPCODE_VERIFY, PACKET_ERRCODE_PARAM, NULL, 0);
        return;
    }

    printf("verify address=0x%08X, size=%u, crc=0x%08X\n", address, size, crc);
    //计算实际 CRC
    uint32_t ccrc = crc32((uint8_t *)address, size);
    //写入数据错误 Flash 损坏 或通信问题
    if (ccrc != crc)
    {
        printf("verify failed: expected 0x%08X, got 0x%08X\n", crc, ccrc);
        bl_response(PACKET_OPCODE_VERIFY, PACKET_ERRCODE_VERIFY, NULL, 0);
        return;
    }

    bl_response(PACKET_OPCODE_VERIFY, PACKET_ERRCODE_OK, NULL, 0);
}
//让 MCU 软件复位（重启）
//如果顺序反了：先 reset → MCU立刻重启 → 上位机收不到响应
//复位后：程序从 0x08000000（Bootloader）重新开始执行
//RESET 用途 升级完成后重新初始化系统 回到 Bootloader 状态
static void bl_opcode_reset_handler(void)
{
    printf("reset handler\n");
    bl_response(PACKET_OPCODE_RESET, PACKET_ERRCODE_OK, NULL, 0);
    printf("system resetting...\n");
    tim_delay_ms(2);

    NVIC_SystemReset();
}
//跳转到应用程序运行
//升级成功后立即运行新固件
static void bl_opcode_boot_handler(void)
{
    printf("boot handler\n");
    //保证上位机知道“可以启动了”
    bl_response(PACKET_OPCODE_BOOT, PACKET_ERRCODE_OK, NULL, 0);
    //Bootloader “安全退出”，把 MCU 完全交给应用程序
    boot_application();
}
//整个协议处理的核心调度器
//根据解析出来的 opcode，调用对应的处理函数  每个 opcode 对应一个功能函数
static void bl_packet_handler(void)
{
    switch (packet_opcode)
    {
        case PACKET_OPCODE_INQUERY:
            bl_opcode_inquery_handler();
            break;
        case PACKET_OPCODE_ERASE:
            bl_opcode_erase_handler();
            break;
        case PACKET_OPCODE_PROGRAM:
            bl_opcode_program_handler();
            break;
        case PACKET_OPCODE_VERIFY:
            bl_opcode_verify_handler();
            break;
        case PACKET_OPCODE_RESET:
            bl_opcode_reset_handler();
            break;
        case PACKET_OPCODE_BOOT:
            bl_opcode_boot_handler();
            break;
        default:
            // 未知指令
            printf("Unknown command: %02X\n", packet_opcode);
            break;
    }
}
//串口字节流 → 完整数据包（packet）的解析状态机
//每收到 1个字节，就调用一次这个函数
static bool bl_byte_handler(uint8_t byte)
{
    bool full_packet = false;
    // 处理字节数据超时接收
    static uint64_t last_byte_ms;
    uint64_t now_ms = tim_get_ms();
    if (now_ms - last_byte_ms > RX_TIMEOUT_MS)
    {
        if (packet_state != PACKET_STATE_HEADER)
            printf("last packet rx timeout\n");
        //从头重新接收
        packet_index = 0;
        packet_state = PACKET_STATE_HEADER;
    }
    last_byte_ms = now_ms;

    // printf("recv: %02X\n", byte);

    // 字节接收状态机处理
    packet_buffer[packet_index++] = byte;   //每个字节都存入 buffer
    switch (packet_state)
    {   
        case PACKET_STATE_HEADER:   //状态1：HEADER
            if (packet_buffer[0] == 0xAA)   //0xAA → 合法包开始
            {
                printf("header ok\n");
                packet_state = PACKET_STATE_OPCODE;
            }
            else
            {
                printf("header error: %02X\n", packet_buffer[0]);
                packet_index = 0;
                packet_state = PACKET_STATE_HEADER;
            }
            break;
        case PACKET_STATE_OPCODE:   //状态2：OPCODE 检查 opcode 是否合法：
            if (packet_buffer[1] == PACKET_OPCODE_INQUERY ||
                packet_buffer[1] == PACKET_OPCODE_ERASE ||
                packet_buffer[1] == PACKET_OPCODE_PROGRAM ||
                packet_buffer[1] == PACKET_OPCODE_VERIFY ||
                packet_buffer[1] == PACKET_OPCODE_RESET ||
                packet_buffer[1] == PACKET_OPCODE_BOOT)
            {
                printf("opcode ok: %02X\n", packet_buffer[1]);
                packet_opcode = (packet_opcode_t)packet_buffer[1];
                packet_state = PACKET_STATE_LENGTH;
            }
            else
            {
                printf("opcode error: %02X\n", packet_buffer[1]);
                packet_index = 0;
                packet_state = PACKET_STATE_HEADER;
            }
            break;
        case PACKET_STATE_LENGTH:   //状态3：LENGTH
            if (packet_index == 4)
            {
                uint16_t payload_length = (packet_buffer[3] << 8) | packet_buffer[2];   //解析长度
                if (payload_length <= PAYLOAD_SIZE_MAX) //长度合法
                {
                    printf("length ok: %u\n", payload_length);
                    packet_payload_length = payload_length;
                    if (packet_payload_length > 0)
                        packet_state = PACKET_STATE_PAYLOAD;
                    else
                        packet_state = PACKET_STATE_CRC16;
                }
                else
                {
                    printf("length error: %u\n", payload_length);
                    packet_index = 0;
                    packet_state = PACKET_STATE_HEADER;
                }
            }
            break;
        case PACKET_STATE_PAYLOAD:  //状态4：PAYLOAD
            if (packet_index == 4 + packet_payload_length)  //判断是否收完
            {
                printf("payload receive ok\n");
                packet_state = PACKET_STATE_CRC16;
            }
            break;
        case PACKET_STATE_CRC16:    //状态5：CRC16（最终校验）
            if (packet_index == 4 + packet_payload_length + 2)  //判断是否完整包：
            {
                uint16_t crc = (packet_buffer[4 + packet_payload_length + 1] << 8) |
                                packet_buffer[4 + packet_payload_length];
                uint16_t ccrc = crc16(packet_buffer, 4 + packet_payload_length);
                if (crc == ccrc)
                {
                    full_packet = true; //完整合法数据包
                    printf("crc16 ok: %04X\n", crc);
                    printf("packet received: opcode=%02X, length=%u\n", packet_opcode, packet_payload_length);
                    // printf("payload: ");
                    // for (uint32_t i = 0; i < packet_payload_length; i++)
                    // {
                    //     printf("%02X ", packet_buffer[4 + i]);
                    // }
                    printf("\n");
                }
                else
                {
                    printf("crc16 error: expected %04X, got %04X\n", crc, ccrc);
                }
                //准备接收下一个包
                packet_index = 0;
                packet_state = PACKET_STATE_HEADER;
            }
            break;
        default:
            break;
    }

    return full_packet;
}
//把串口收到的数据放入环形缓冲区
//串口是“异步”的 数据随时来（中断触发） 主循环不一定及时处理
static void bl_usart_rx_handler(const uint8_t *data, uint32_t length)
{
    rb_puts(rxrb, data, length);
}
//上电时检测按键，决定是否“停留在 Bootloader”
//决定启动时是“进 Bootloader”还是“跳应用”
static bool key_trap_check(void)
{
    for (uint32_t t = 0; t < BOOT_DELAY; t+=10)
    {
        tim_delay_ms(10);
        if (!key_read(key2))
            return false;
    }
    printf("key pressed, trap into boot\n");
    return true;
}
//一直等到按键“松开”为止
static void wait_key_release(void)
{
    while (key_read(key2))
        tim_delay_ms(10);
}
//判断按键是否“稳定按下”
static bool key_press_check(void)
{
    if (!key_read(key2))
        return false;

    tim_delay_ms(10);
    if (!key_read(key2))
        return false;

    return true;
}
//根据应用程序是否合法，决定是否强制停留在 Bootloader
bool magic_header_trap_boot(void)
{   //检查 Magic Header
    if (!magic_header_validate())
    {
        printf("magic header invalid, trap into boot\n");
        return true;
    }
    //检查应用完整性
    if (!application_validate())
    {
        printf("application validate failed, trap into boot\n");
        return true;
    }

    return false;
}
//初始化系统 → 判断是否跳转应用 → 否则进入升级模式循环
void bootloader_main(void)
{
    printf("Bootloader started.\n");
    //创建环形缓冲区 用来缓存串口数据
    rxrb = rb_new(rb_buffer, RX_BUFFER_SIZE);
    //初始化串口 用于接收升级数据 发送响应（bl_response）
    bl_usart_init();
    //注册接收回调 串口收到数据时：自动调用 bl_usart_rx_handler → 写入 ringbuffer
    bl_usart_register_rx_callback(bl_usart_rx_handler);
    //用于：控制是否进入 Bootloader  运行时复位
    key_init(key2);
    //是否进入 Bootloader
    bool trapboot = key_trap_check();   //决定是否“人为进入 Bootloader”
    if (!trapboot)  //只有在“不强制进Boot”时才检查 应用是否存在 CRC 是否正确
        trapboot = magic_header_trap_boot();

    if (!trapboot)  //没按键要求进入 Bootloader 应用程序也合法
    {
        boot_application(); //直接跳应用程序运行
    }
//-------------------------------------------------------
//正常设备：上电 → 直接运行应用 
//异常/升级：上电 → 进入 Bootloader
//-------------------------------------------------------
    led_init(led1);
    led_on(led1);
    wait_key_release();

    while (1)
    {
        if (key_press_check())  //用户按键 → 设备重启
        {
            printf("key pressed, rebooting...\n");
            tim_delay_ms(2);
            NVIC_SystemReset();
        }

        if (!rb_empty(rxrb))    //从 ringbuffer 取出 1 字节
        {
            uint8_t byte;
            rb_get(rxrb, &byte);
            if (bl_byte_handler(byte))  //协议解析（状态机）
            {
                bl_packet_handler();    //完整包处理
            }
        }
    }
}
