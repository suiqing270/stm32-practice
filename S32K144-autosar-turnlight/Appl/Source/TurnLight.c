/*
 * File: TurnLight.c
 *
 * Code generated for Simulink model 'TurnLight'.
 *
 * Model version                  : 1.8
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Sat Aug 10 08:20:22 2024
 *
 * Target selection: autosar.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "TurnLight.h"
#include "TurnLight_private.h"

/* Named constants for Chart: '<S2>/HazardCtrlLogic' */
#define TurnLight_IN_NO_ACTIVE_CHILD   ((uint8)0U)
#define TurnLight_IN_OFF               ((uint8)1U)
#define TurnLight_IN_ON_Crash          ((uint8)2U)
#define TurnLight_IN_ON_SW             ((uint8)3U)
#define TurnLight_IN_On_EmergencyBrake ((uint8)4U)

/* Named constants for Chart: '<S2>/TurnLightCtrlLogic' */
#define TurnLight_IN_Check1            ((uint8)1U)
#define TurnLight_IN_Check2            ((uint8)2U)
#define TurnLight_IN_IDLE              ((uint8)1U)
#define TurnLight_IN_LEFT              ((uint8)2U)
#define TurnLight_IN_LongFlash         ((uint8)3U)
#define TurnLight_IN_RIGHT             ((uint8)3U)
#define TurnLight_IN_ShortFlash        ((uint8)4U)

/* Named constants for Chart: '<S2>/TurnLightDiag' */
#define TurnLight_IN_OPEN              ((uint8)2U)
#define TurnLight_IN_SHORT             ((uint8)3U)

/* Named constants for Chart: '<S2>/TurnLightSelector' */
#define TurnLight_IN_ALLOFF            ((uint8)1U)
#define TurnLight_IN_ALLON             ((uint8)2U)
#define TurnLight_IN_LEFTON            ((uint8)3U)
#define TurnLight_IN_RIGHTON           ((uint8)4U)

/* Block signals (default storage) */
B_TurnLight_T TurnLight_B;

/* Block states (default storage) */
DW_TurnLight_T TurnLight_DW;

/* Forward declaration for local functions */
static void TurnLigh_DecideRightLongOrShort(void);
static void TurnLight_DecideLeftLongOrShort(void);

/* Output and update for function-call system: '<S2>/TurnLight_ALLOFF' */
void TurnLight_TurnLight_ALLOFF(void)
{
  TurnLightService rtb_BusCreator_le;

  /* BusCreator: '<S7>/Bus Creator' incorporates:
   *  Constant: '<S7>/Constant'
   *  Constant: '<S7>/Constant1'
   *  Constant: '<S7>/Constant2'
   *  Constant: '<S7>/Constant3'
   *  Constant: '<S7>/Constant4'
   *  Constant: '<S7>/Constant5'
   *  Constant: '<S7>/Constant6'
   */
  rtb_BusCreator_le.TurnLightPosition = ALL;
  rtb_BusCreator_le.TurnLightOnOff = OFF;
  rtb_BusCreator_le.TurnLightDutyCycle = 0U;
  rtb_BusCreator_le.TurnLightCycle = 8U;
  rtb_BusCreator_le.TurnLightTimes = 0U;
  rtb_BusCreator_le.TurnLightNumber = 0U;
  rtb_BusCreator_le.TurnLightPriority = 3U;

  /* FunctionCaller: '<S7>/TurnLightService_TurnLightControl_Operation' incorporates:
   *  BusCreator: '<S7>/Bus Creator'
   */
  Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator_le);
}

/* Output and update for function-call system: '<S2>/TurnLight_ALLON' */
void TurnLight_TurnLight_ALLON(void)
{
  TurnLightService rtb_BusCreator_oj;

  /* BusCreator: '<S8>/Bus Creator' incorporates:
   *  Constant: '<S8>/Constant'
   *  Constant: '<S8>/Constant1'
   *  Constant: '<S8>/Constant2'
   *  Constant: '<S8>/Constant3'
   *  Constant: '<S8>/Constant4'
   *  Constant: '<S8>/Constant5'
   *  Constant: '<S8>/Constant6'
   */
  rtb_BusCreator_oj.TurnLightPosition = ALL;
  rtb_BusCreator_oj.TurnLightOnOff = ON;
  rtb_BusCreator_oj.TurnLightDutyCycle = 0U;
  rtb_BusCreator_oj.TurnLightCycle = 8U;
  rtb_BusCreator_oj.TurnLightTimes = 0U;
  rtb_BusCreator_oj.TurnLightNumber = 0U;
  rtb_BusCreator_oj.TurnLightPriority = 3U;

  /* FunctionCaller: '<S8>/TurnLightService_TurnLightControl_Operation' incorporates:
   *  BusCreator: '<S8>/Bus Creator'
   */
  Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator_oj);
}
//右转灯闪烁模式选择函数。本身不判断“是否右转”，而是在右转请求已经成立后，根据 TurnLight_B.TurnLampSWMode 选择“短闪”还是“长闪”。
/* Function for Chart: '<S2>/TurnLightSelector' */
static void TurnLigh_DecideRightLongOrShort(void)
{
  TurnLightService rtb_BusCreator;

  /*  ������˸ģʽ  */
  if (TurnLight_B.TurnLampSWMode == 1) {
    /* Outputs for Function Call SubSystem: '<S2>/TurnLight_RightShort' */
    /* BusCreator: '<S20>/Bus Creator' incorporates:
     *  Constant: '<S20>/Constant'
     *  Constant: '<S20>/Constant1'
     *  Constant: '<S20>/Constant2'
     *  Constant: '<S20>/Constant3'
     *  Constant: '<S20>/Constant4'
     *  Constant: '<S20>/Constant5'
     *  Constant: '<S20>/Constant6'
     */
    //构造一个 TurnLightService 请求，随后通过Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);
    //发送给灯光仲裁模块。含义如下：以优先级 3 请求打开右侧转向灯，使用普通转向灯周期，并按照短闪模式执行，短闪参数为 3。
    //其中，TurnLightNumber = 3U 是短闪分支和长闪分支的主要区别。长闪分支中该值为 0U：
    //在 TurnLightArb.c 中，TurnLightNumber == 3 会被识别为 SHORT_ON，而 TurnLightNumber == 0 会进入普通转向灯输出逻辑。
    rtb_BusCreator.TurnLightPosition = RIGHT; //控制右侧转向灯，通常包括右前和右后灯
    rtb_BusCreator.TurnLightOnOff = ON;       //请求打开灯光
    rtb_BusCreator.TurnLightDutyCycle = 0U;   //不单独指定 PWM 占空比，使用后级默认控制；仲裁逻辑要求该值为 0
    rtb_BusCreator.TurnLightCycle = 8U;       //使用普通转向灯闪烁周期；诊断中的双闪周期使用 4U
    rtb_BusCreator.TurnLightTimes = 0U;       //不指定额外的总持续时间，具体持续由请求状态控制
    rtb_BusCreator.TurnLightNumber = 3U;      //短闪模式参数，后级仲裁通过 Number == 3 识别为短闪
    rtb_BusCreator.TurnLightPriority = 3U;    //请求优先级；仲裁逻辑中优先级数值越大越容易获得控制权

    /* FunctionCaller: '<S20>/TurnLightService_TurnLightControl_Operation' incorporates:
     *  BusCreator: '<S20>/Bus Creator'
     */
    //实际实现位于 [TurnLightArb.c (line 239)]
    //是 AUTOSAR RTE 的客户端/服务端调用，用于把 TurnLightService 控制请求发送给 TurnLightArb。
    /*在 TurnLightArb 中会被识别为右侧短闪请求，并最终写出右前、右后灯的控制信号。
    实际硬件控制还会继续经过 TurnLightAct 和 IoHwAb，因此 Rte_Call 本身只是“提交灯光控制请求”，不是直接操作 GPIO。*/
    Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

    /* End of Outputs for SubSystem: '<S2>/TurnLight_RightShort' */
    TurnLight_B.TurnLight_Decide = RIGHTSHORT;
  } else {
    /* Outputs for Function Call SubSystem: '<S2>/TurnLight_LeftLong1' */
    /* BusCreator: '<S14>/Bus Creator' incorporates:
     *  Constant: '<S14>/Constant'
     *  Constant: '<S14>/Constant1'
     *  Constant: '<S14>/Constant2'
     *  Constant: '<S14>/Constant3'
     *  Constant: '<S14>/Constant4'
     *  Constant: '<S14>/Constant5'
     *  Constant: '<S14>/Constant6'
     */
    rtb_BusCreator.TurnLightPosition = RIGHT;
    rtb_BusCreator.TurnLightOnOff = ON;
    rtb_BusCreator.TurnLightDutyCycle = 0U;
    rtb_BusCreator.TurnLightCycle = 8U;
    rtb_BusCreator.TurnLightTimes = 0U;
    rtb_BusCreator.TurnLightNumber = 0U;
    rtb_BusCreator.TurnLightPriority = 3U;

    /* FunctionCaller: '<S14>/TurnLightService_TurnLightControl_Operation' incorporates:
     *  BusCreator: '<S14>/Bus Creator'
     */
    Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

    /* End of Outputs for SubSystem: '<S2>/TurnLight_LeftLong1' */
    TurnLight_B.TurnLight_Decide = RIGHTLONG;
  }
}

/* Function for Chart: '<S2>/TurnLightSelector' */
static void TurnLight_DecideLeftLongOrShort(void)
{
  TurnLightService rtb_BusCreator_ak;

  /*  ������˸ģʽ  */
  if (TurnLight_B.TurnLampSWMode == 1) {
    /* Outputs for Function Call SubSystem: '<S2>/TurnLight_LeftShort' */
    /* BusCreator: '<S15>/Bus Creator' incorporates:
     *  Constant: '<S15>/Constant'
     *  Constant: '<S15>/Constant1'
     *  Constant: '<S15>/Constant2'
     *  Constant: '<S15>/Constant3'
     *  Constant: '<S15>/Constant4'
     *  Constant: '<S15>/Constant5'
     *  Constant: '<S15>/Constant6'
     */
    rtb_BusCreator_ak.TurnLightPosition = LEFT;
    rtb_BusCreator_ak.TurnLightOnOff = ON;
    rtb_BusCreator_ak.TurnLightDutyCycle = 0U;
    rtb_BusCreator_ak.TurnLightCycle = 8U;
    rtb_BusCreator_ak.TurnLightTimes = 0U;
    rtb_BusCreator_ak.TurnLightNumber = 3U;
    rtb_BusCreator_ak.TurnLightPriority = 3U;

    /* FunctionCaller: '<S15>/TurnLightService_TurnLightControl_Operation' incorporates:
     *  BusCreator: '<S15>/Bus Creator'
     */
    Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator_ak);

    /* End of Outputs for SubSystem: '<S2>/TurnLight_LeftShort' */
    TurnLight_B.TurnLight_Decide = LEFTSHORT;
  } else {
    /* Outputs for Function Call SubSystem: '<S2>/TurnLight_LeftLong' */
    /* BusCreator: '<S13>/Bus Creator' incorporates:
     *  Constant: '<S13>/Constant'
     *  Constant: '<S13>/Constant1'
     *  Constant: '<S13>/Constant2'
     *  Constant: '<S13>/Constant3'
     *  Constant: '<S13>/Constant4'
     *  Constant: '<S13>/Constant5'
     *  Constant: '<S13>/Constant6'
     */
    rtb_BusCreator_ak.TurnLightPosition = LEFT;
    rtb_BusCreator_ak.TurnLightOnOff = ON;
    rtb_BusCreator_ak.TurnLightDutyCycle = 0U;
    rtb_BusCreator_ak.TurnLightCycle = 8U;
    rtb_BusCreator_ak.TurnLightTimes = 0U;
    rtb_BusCreator_ak.TurnLightNumber = 0U;
    rtb_BusCreator_ak.TurnLightPriority = 3U;

    /* FunctionCaller: '<S13>/TurnLightService_TurnLightControl_Operation' incorporates:
     *  BusCreator: '<S13>/Bus Creator'
     */
    Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator_ak);

    /* End of Outputs for SubSystem: '<S2>/TurnLight_LeftLong' */
    TurnLight_B.TurnLight_Decide = LEFTLONG;
  }
}

/*TurnLight_Runnable_10ms(void) 是 Simulink 生成的 AUTOSAR 周期任务，每 10 ms 执行一次。
它并不直接控制 GPIO，而是读取车辆信号、运行四组状态机，再通过 RTE 服务向转向灯仲裁模块提交控制请求。*/
/* Model step function for TID1 */
void TurnLight_Runnable_10ms(void)  /* Explicit Task: TurnLight_Runnable_10ms */
{
  TurnLightService rtb_BusCreator;
  sint32 tmp;
  uint16 tmpRead_a;
  uint8 HazardRqst;
  uint8 tmpRead;
  uint8 tmpRead_0;
  uint8 tmpRead_1;
  uint8 tmpRead_2;
  uint8 tmpRead_3;
  uint8 tmpRead_4;
  uint8 tmpRead_5;
  uint8 tmpRead_6;
  uint8 tmpRead_7;
  uint8 tmpRead_8;
  uint8 tmpRead_9;
  uint8 tmpRead_b;
  boolean guard1 = false;
  boolean guard2 = false;
  //读取输入
  /* Inport: '<Root>/HazardSw_u8_Signal' */
  (void)Rte_Read_HazardSw_u8_Signal(&HazardRqst);//双闪开关

  /* Inport: '<Root>/EmergencyBrakeLight_u8_Signal' */
  (void)Rte_Read_EmergencyBrakeLight_u8_Signal(&tmpRead_1);//紧急制动双闪请求

  /* Inport: '<Root>/CrashIntensity_u8_Signal' */
  (void)Rte_Read_CrashIntensity_u8_Signal(&tmpRead_0);//碰撞强度

  /* RootInportFunctionCallGenerator generated from: '<Root>/TurnLight_Runnable_10ms' incorporates:
   *  SubSystem: '<Root>/TurnLight_Runnable_10ms_sys'
   */
  /* Chart: '<S2>/TurnLightCtrlLogic' incorporates:
   *  Inport: '<Root>/CountSignal_Left_u8_Signal' 
   *  Inport: '<Root>/IGN_Status_u8_signal'       
   *  Inport: '<Root>/TurnLampSw_u8_Signal'       
   */
  if (TurnLight_DW.is_active_c3_TurnLight == 0U) {
    TurnLight_DW.is_active_c3_TurnLight = 1U;
    TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
    TurnLight_B.TurnLampSWMode = 0U;
    TurnLight_B.TurnLampSWRst = 0U;
    TurnLight_DW.TurnLapmCount = 0U;              //拨杆保持时间，每次调用增加一次，即每次约 10 ms
  } else {
    (void)Rte_Read_CountSignal_Left_u8_Signal(&tmpRead);  //短闪完成计数
    (void)Rte_Read_IGN_Status_u8_signal(&tmpRead_2);  //读取点火状态
    (void)Rte_Read_TurnLampSw_u8_Signal(&tmpRead_9);  //读取转向拨杆
    switch (TurnLight_DW.is_c3_TurnLight) {
     case TurnLight_IN_IDLE:
      TurnLight_B.TurnLampSWMode = 0U;
      TurnLight_B.TurnLampSWRst = 0U;
      //点火状态为 2 或 3;拨杆为 1;进入 LEFT.Check1;TurnLapmCount 每 10 ms 加一
      //拨杆过早松开则返回 IDLE;保持超过 10 个周期后，确认请求有效;也就是大约需要保持 100～120 ms 才进入下一阶段。
      if (((tmpRead_2 == 2) || (tmpRead_2 == 3)) && (tmpRead_9 == 1)) {//检测到点火有效且拨杆向左
        TurnLight_DW.is_c3_TurnLight = TurnLight_IN_LEFT;//转向灯主状态机从 IDLE 切换到 LEFT
        //LEFT 是一个包含子状态的复合状态，这里将左转子状态设置为 Check1
        //Check1 是第一阶段确认，也可以理解为拨杆防抖阶段：拨杆必须保持向左一段时间，才会进一步确认左转请求。
        TurnLight_DW.is_LEFT = TurnLight_IN_Check1;
        tmp = TurnLight_DW.TurnLapmCount + 1;           //启动防抖计时
        //TurnLapmCount 是拨杆保持时间计数器。由于任务每 10 ms 执行一次，因此：
        //计数增加 1 ≈ 经过 10 ms
        //因为 TurnLapmCount 是 uint8，最大值只能是 255。这段代码保证计数达到 255 后不再增加，避免转换回 uint8 时从 255 溢出为 0。
        if (TurnLight_DW.TurnLapmCount + 1 > 255) {
          tmp = 255;
        }

        TurnLight_DW.TurnLapmCount = (uint8)tmp;//保存结果
      } else {
        if (((tmpRead_2 == 2) || (tmpRead_2 == 3)) && (tmpRead_9 == 2)) {
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_RIGHT;
          TurnLight_DW.is_RIGHT = TurnLight_IN_Check1;
          tmp = TurnLight_DW.TurnLapmCount + 1;
          if (TurnLight_DW.TurnLapmCount + 1 > 255) {
            tmp = 255;
          }
          TurnLight_DW.TurnLapmCount = (uint8)tmp;
        }
      }
      break;

     case TurnLight_IN_LEFT:
      switch (TurnLight_DW.is_LEFT) {
       case TurnLight_IN_Check1:
        if (tmpRead_9 != 1) { //拨杆不再向左
          //此时认为左转操作持续时间太短或者已经取消，执行完整复位

          //退出 LEFT 内部的子状态。NO_ACTIVE_CHILD 表示当前没有活跃的左转子状态。
          TurnLight_DW.is_LEFT = TurnLight_IN_NO_ACTIVE_CHILD;
          //主状态机由 LEFT 返回空闲状态 IDLE
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
          //清除转向灯模式和转向请求  
          TurnLight_B.TurnLampSWMode = 0U;  //没有短闪或长闪模式
          TurnLight_B.TurnLampSWRst = 0U;   //没有左转或右转请求
          TurnLight_DW.TurnLapmCount = 0U;  //清零拨杆保持计数器，为下一次操作做准备。
        } else if (TurnLight_DW.TurnLapmCount > 10) { //任务每 10 ms 执行一次，所以计数超过 10 表示拨杆大约保持了 110 ms
          //进入第二阶段 Check2。这个阶段将继续判断用户要的是短闪还是长闪
          TurnLight_DW.is_LEFT = TurnLight_IN_Check2; 
          TurnLight_B.TurnLampSWRst = 1U; //正式生成左转请求
        } else {  //拨杆仍然向左，并且 TurnLapmCount <= 10
          tmp = TurnLight_DW.TurnLapmCount + 1;//此时继续计时
          if (TurnLight_DW.TurnLapmCount + 1 > 255) { //最大值保护
            tmp = 255;
          }
          /*不过在这个具体状态中，计数超过 10 就会进入 Check2，正常情况下根本不会增长到 255。
          这个饱和判断主要是 Simulink 针对 uint8 加法自动生成的通用溢出保护*/

          TurnLight_DW.TurnLapmCount = (uint8)tmp;  //最后写回
        }
        break;

       case TurnLight_IN_Check2://左转灯状态机的第二阶段判断：根据拨杆保持时长，决定进入“短闪”还是“长闪”
        TurnLight_B.TurnLampSWRst = 1U; //左转请求
        if ((TurnLight_DW.TurnLapmCount <= 70) && (tmpRead_9 == 0)) {//如果大约在 700 ms 内松开左转拨杆
          TurnLight_DW.is_LEFT = TurnLight_IN_ShortFlash;
          TurnLight_B.TurnLampSWMode = 1U;  //进入短闪模式。后续代码会等待灯闪烁 3 次后自动关闭
        } else if ((TurnLight_DW.TurnLapmCount > 70) && (tmpRead_9 == 1)) {//如果拨杆保持向左超过约 700 ms
          TurnLight_DW.is_LEFT = TurnLight_IN_LongFlash;
          TurnLight_B.TurnLampSWMode = 2U;  //进入长闪模式。长闪会一直持续，直到驾驶员松开拨杆或点火状态关闭
        } else {  //继续计数
          tmp = TurnLight_DW.TurnLapmCount + 1;
          if (TurnLight_DW.TurnLapmCount + 1 > 255) {
            tmp = 255;
          }

          TurnLight_DW.TurnLapmCount = (uint8)tmp;
        }
        break;

       case TurnLight_IN_LongFlash: //左转的“长闪”状态
        TurnLight_B.TurnLampSWMode = 2U;  //表示持续长闪
        if ((tmpRead_9 == 0) || (tmpRead_2 == 0)) { //左转拨杆回到中间位置或点火状态关闭
          //停止长闪，执行完整复位：
          TurnLight_DW.is_LEFT = TurnLight_IN_NO_ACTIVE_CHILD;
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
          TurnLight_B.TurnLampSWMode = 0U;
          TurnLight_B.TurnLampSWRst = 0U;
          TurnLight_DW.TurnLapmCount = 0U;
        }
        break;

       default:     //左转的“短闪”状态.前面的状态已经逐一判断，剩余状态默认认为是 ShortFlash
        /* case IN_ShortFlash: */
        TurnLight_B.TurnLampSWMode = 1U;  //短闪通常是驾驶员轻拨后松开，系统自动完成固定次数闪烁
        if (tmpRead == 3) { //tmpRead 是左转灯已完成闪烁次数 CountSignal_Left。当完成第 3 次闪烁时，短闪结束
          //同样执行完整复位:
          TurnLight_DW.is_LEFT = TurnLight_IN_NO_ACTIVE_CHILD;
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
          TurnLight_B.TurnLampSWMode = 0U;
          TurnLight_B.TurnLampSWRst = 0U;
          TurnLight_DW.TurnLapmCount = 0U;
        }
        break;
      }
      break;

     default:
      /* case IN_RIGHT: */
      switch (TurnLight_DW.is_RIGHT) {
       case TurnLight_IN_Check1:
        if (tmpRead_9 != 2) {
          TurnLight_DW.is_RIGHT = TurnLight_IN_NO_ACTIVE_CHILD;
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
          TurnLight_B.TurnLampSWMode = 0U;
          TurnLight_B.TurnLampSWRst = 0U;
          TurnLight_DW.TurnLapmCount = 0U;
        } else if (TurnLight_DW.TurnLapmCount > 10) {
          TurnLight_DW.is_RIGHT = TurnLight_IN_Check2;
          TurnLight_B.TurnLampSWRst = 2U;
        } else {
          tmp = TurnLight_DW.TurnLapmCount + 1;
          if (TurnLight_DW.TurnLapmCount + 1 > 255) {
            tmp = 255;
          }

          TurnLight_DW.TurnLapmCount = (uint8)tmp;
        }
        break;

       case TurnLight_IN_Check2:
        TurnLight_B.TurnLampSWRst = 2U;
        if ((TurnLight_DW.TurnLapmCount <= 70) && (tmpRead_9 == 0)) {
          TurnLight_DW.is_RIGHT = TurnLight_IN_ShortFlash;
          TurnLight_B.TurnLampSWMode = 1U;
        } else if ((TurnLight_DW.TurnLapmCount > 70) && (tmpRead_9 == 2)) {
          TurnLight_DW.is_RIGHT = TurnLight_IN_LongFlash;
          TurnLight_B.TurnLampSWMode = 2U;
        } else {
          tmp = TurnLight_DW.TurnLapmCount + 1;
          if (TurnLight_DW.TurnLapmCount + 1 > 255) {
            tmp = 255;
          }

          TurnLight_DW.TurnLapmCount = (uint8)tmp;
        }
        break;

       case TurnLight_IN_LongFlash:
        TurnLight_B.TurnLampSWMode = 2U;
        if ((tmpRead_9 == 0) || (tmpRead_2 == 0)) {
          TurnLight_DW.is_RIGHT = TurnLight_IN_NO_ACTIVE_CHILD;
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
          TurnLight_B.TurnLampSWMode = 0U;
          TurnLight_B.TurnLampSWRst = 0U;
          TurnLight_DW.TurnLapmCount = 0U;
        }
        break;

       default:
        /* case IN_ShortFlash: */
        TurnLight_B.TurnLampSWMode = 1U;
        if (tmpRead == 3) {
          TurnLight_DW.is_RIGHT = TurnLight_IN_NO_ACTIVE_CHILD;
          TurnLight_DW.is_c3_TurnLight = TurnLight_IN_IDLE;
          TurnLight_B.TurnLampSWMode = 0U;
          TurnLight_B.TurnLampSWRst = 0U;
          TurnLight_DW.TurnLapmCount = 0U;
        }
        break;
      }
      break;
    }
  }

  /* End of Chart: '<S2>/TurnLightCtrlLogic' */

  /* Chart: '<S2>/HazardCtrlLogic' incorporates:
   *  Inport: '<Root>/VehicleSpeed_Valid_u8_Signal'
   *  Inport: '<Root>/VehicleSpeed_u16_signal'
   */
  /*这段是 Simulink/Stateflow 生成的 HazardCtrlLogic 状态机
  ，作用是根据碰撞、紧急制动和双闪开关，计算是否输出双闪请求 HazardRqst
  每 10 ms 执行一次*/
  //temporalCounter_i1 每次运行加 1，并限制在 131071，避免计数器溢出
  if (TurnLight_DW.temporalCounter_i1 < 131071U) {
    TurnLight_DW.temporalCounter_i1++;
  }
  //保存碰撞强度的“上一次值”和“当前值”。另外两组变量同理
  TurnLight_DW.CrashIntensity_prev = TurnLight_DW.CrashIntensity_start;
  TurnLight_DW.CrashIntensity_start = tmpRead_0;
  //双闪开关
  TurnLight_DW.HazardSw_prev = TurnLight_DW.HazardSw_start;
  TurnLight_DW.HazardSw_start = HazardRqst;
  //紧急制动双闪请求
  TurnLight_DW.EmergencyBrakeLight_prev = TurnLight_DW.EmergencyBrakeLight_start;
  TurnLight_DW.EmergencyBrakeLight_start = tmpRead_1;
  //prev != current && current == 1  =>信号从 0 变为 1=>检测到上升沿

  //状态机首次启动,尚未初始化
  //避免第一次运行时因为“历史值不存在”而误判成信号上升沿
  if (TurnLight_DW.is_active_c1_TurnLight == 0U) {
    //历史输入设置为当前输入
    TurnLight_DW.CrashIntensity_prev = tmpRead_0;
    TurnLight_DW.HazardSw_prev = HazardRqst;
    TurnLight_DW.EmergencyBrakeLight_prev = tmpRead_1;
    TurnLight_DW.is_active_c1_TurnLight = 1U;
    TurnLight_DW.is_c1_TurnLight = TurnLight_IN_OFF;//设置当前状态为 OFF
    HazardRqst = 0U;  //输出 HazardRqst = 0
  } else {  //按优先级检测事件:
    switch (TurnLight_DW.is_c1_TurnLight) {
     case TurnLight_IN_OFF:
      HazardRqst = 0U;
      //碰撞强度从 0 变成非 0 时
      if ((TurnLight_DW.CrashIntensity_prev != TurnLight_DW.CrashIntensity_start)
          && (TurnLight_DW.CrashIntensity_prev == 0)) {
        TurnLight_DW.is_c1_TurnLight = TurnLight_IN_ON_Crash; //进入 ON_Crash
        TurnLight_DW.temporalCounter_i1 = 0U;       //清零计时器
        HazardRqst = 1U;    //输出双闪请求
      } 
      //紧急制动信号出现上升沿时
      else if ((TurnLight_DW.EmergencyBrakeLight_prev !=
                  TurnLight_DW.EmergencyBrakeLight_start) &&
                 (TurnLight_DW.EmergencyBrakeLight_start == 1)) {
        TurnLight_DW.is_c1_TurnLight = TurnLight_IN_On_EmergencyBrake;  //进入 On_EmergencyBrake
        HazardRqst = 1U;
      } 
      //双闪开关从关闭变为打开时
      else {
        if ((TurnLight_DW.HazardSw_prev != TurnLight_DW.HazardSw_start) &&
            (TurnLight_DW.HazardSw_start == 1)) {
          TurnLight_DW.is_c1_TurnLight = TurnLight_IN_ON_SW;  //进入 ON_SW
          HazardRqst = 1U;
        }
      }
      break;

     case TurnLight_IN_ON_Crash:  //碰撞状态下持续输出双闪。
      HazardRqst = 1U;
      //碰撞信号仍为非 0、持续至少约 5 秒后，如果用户再次打开双闪开关，则退出碰撞状态
      //或碰撞状态持续约 15 分钟后自动退出。
      if (((tmpRead_0 != 0) && (TurnLight_DW.temporalCounter_i1 >= 500U) &&
           ((TurnLight_DW.HazardSw_prev != TurnLight_DW.HazardSw_start) &&
            (TurnLight_DW.HazardSw_start == 1))) || ((tmpRead_0 != 0) &&
           (TurnLight_DW.temporalCounter_i1 >= 90000U))) {
        TurnLight_DW.is_c1_TurnLight = TurnLight_IN_OFF;
        HazardRqst = 0U;
      }
      break;

     case TurnLight_IN_ON_SW:     //手动双闪打开后，持续输出双闪
      HazardRqst = 1U;
      //再次操作开关来关闭双闪
      if ((TurnLight_DW.HazardSw_prev != TurnLight_DW.HazardSw_start) &&
          (TurnLight_DW.HazardSw_start == 1)) {
        TurnLight_DW.is_c1_TurnLight = TurnLight_IN_OFF;
        HazardRqst = 0U;
      }
      break;

     default:   //紧急制动双闪请求有效期间
      /* case IN_On_EmergencyBrake: */
      HazardRqst = 1U;
      (void)Rte_Read_VehicleSpeed_u16_signal(&tmpRead_a);
      (void)Rte_Read_VehicleSpeed_Valid_u8_Signal(&tmpRead_b);
      //用户操作双闪开关，或者车辆速度有效且超过 15
      //就退出紧急制动双闪状态，并将 HazardRqst 清零
      if (((tmpRead_1 == 1) && ((TurnLight_DW.HazardSw_prev !=
             TurnLight_DW.HazardSw_start) && (TurnLight_DW.HazardSw_start == 1)))
          || ((tmpRead_1 == 1) && (tmpRead_a > 15) && (tmpRead_b == 1))) {
        TurnLight_DW.is_c1_TurnLight = TurnLight_IN_OFF;
        HazardRqst = 0U;
      }
      break;
    }
  }

  /* End of Chart: '<S2>/HazardCtrlLogic' */

  /* Chart: '<S2>/TurnLightSelector' */
  //负责在“全关、左转、右转、双闪”之间进行仲裁

  //HazardRqst 双闪请求 ;TurnLampSWRst 转向请求：0 无，1 左转，2 右转
  //TurnLampSWMode 转向模式：1 短闪，其他值按长闪处理 ;is_c2_TurnLight 当前状态 
  //TurnLight_Decide 最终灯光决策，供后续诊断逻辑使用 ;LeftRetry/RightRetry 左/右灯故障恢复后的重发标志

  //保存当前值与历史值,为了检测信号边沿
  TurnLight_DW.HazardLightRqst_prev = TurnLight_DW.HazardLightRqst_start;
  TurnLight_DW.HazardLightRqst_start = HazardRqst;
  TurnLight_DW.TurnLightSwRqst_prev = TurnLight_DW.TurnLightSwRqst_start;
  TurnLight_DW.TurnLightSwRqst_start = TurnLight_B.TurnLampSWRst;
  if (TurnLight_DW.is_active_c2_TurnLight == 0U) {  //第一次进入状态机时
    TurnLight_DW.HazardLightRqst_prev = HazardRqst; //将历史输入设置为当前输入，避免误判边沿
    TurnLight_DW.TurnLightSwRqst_prev = TurnLight_B.TurnLampSWRst;
    TurnLight_DW.is_active_c2_TurnLight = 1U;
    TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLOFF; //初始状态设置为 ALLOFF

    /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLOFF' */
    //调用 TurnLight_TurnLight_ALLOFF()，向灯光仲裁模块提交“全部关闭”请求。
    TurnLight_TurnLight_ALLOFF(); 

    /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLOFF' */
    TurnLight_B.TurnLight_Decide = ALLOFF;
  } else {
    //guard1 代表“稍后进入右转”，guard2 代表“稍后进入左转”
    guard1 = false;
    guard2 = false;
    switch (TurnLight_DW.is_c2_TurnLight) {
     case TurnLight_IN_ALLOFF:  //当前所有转向灯关闭时，按以下优先级判断：
      if (TurnLight_B.TurnLampSWRst == 1) {         //左转请求
        TurnLight_DW.is_c2_TurnLight = TurnLight_IN_LEFTON;
        TurnLight_DW.LeftRetry = 0U;
        //根据 TurnLampSWMode 决定短闪还是长闪，并向灯光服务发送控制请求
        TurnLight_DecideLeftLongOrShort();
      } else if (TurnLight_B.TurnLampSWRst == 2) {  //右转请求
        TurnLight_DW.is_c2_TurnLight = TurnLight_IN_RIGHTON;
        TurnLight_DW.RightRetry = 0U;
        TurnLigh_DecideRightLongOrShort();
      } else {                                      //双闪请求
        if (HazardRqst == 1) {
          TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLON;

          /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLON' */
          TurnLight_TurnLight_ALLON();

          /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLON' */
          TurnLight_B.TurnLight_Decide = ALLON;
        }
      }
      break;

     case TurnLight_IN_ALLON: //ALLON 双闪状态
     //双闪期间新操作右转
     //双闪仍然有效，但转向请求刚切换到右转，因此设置：guard1 = true;
     //这里不会立即执行右转，而是在 switch 后统一处理
     //决定双闪之后切换到右转、左转，还是全部关闭
      if ((HazardRqst == 1) && ((TurnLight_DW.TurnLightSwRqst_prev !=
            TurnLight_DW.TurnLightSwRqst_start) &&
           (TurnLight_DW.TurnLightSwRqst_start == 2))) {
        guard1 = true;
      } 
      else if ((HazardRqst == 1) && ((TurnLight_DW.TurnLightSwRqst_prev !=
                   TurnLight_DW.TurnLightSwRqst_start) &&
                  (TurnLight_DW.TurnLightSwRqst_start == 1))) {
        guard2 = true;
      } 
      //双闪关闭后恢复右转
      //双闪刚关闭 + 右转开关仍保持 → 恢复右转灯
      else if ((TurnLight_B.TurnLampSWRst == 2) &&
                 ((TurnLight_DW.HazardLightRqst_prev !=
                   TurnLight_DW.HazardLightRqst_start) &&
                  (TurnLight_DW.HazardLightRqst_start == 0))) {
        guard1 = true;
      } else if ((TurnLight_B.TurnLampSWRst == 1) &&
                 ((TurnLight_DW.HazardLightRqst_prev !=
                   TurnLight_DW.HazardLightRqst_start) &&
                  (TurnLight_DW.HazardLightRqst_start == 0))) {
        guard2 = true;
      } else {  //最后一种情况：双闪关闭且没有转向请求
        if (HazardRqst == 0) {
          TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLOFF;

          /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLOFF' */
          TurnLight_TurnLight_ALLOFF();

          /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLOFF' */
          TurnLight_B.TurnLight_Decide = ALLOFF;
        }
      }
      break;

     case TurnLight_IN_LEFTON:  //LEFTON 左转状态
     //切换到双闪
     /*满足以下任意条件进入 ALLON：
      - 左转期间双闪请求出现上升沿；
      - 双闪已经有效，左转请求又发生变化，例如松开左转开关。*/
      if (((TurnLight_B.TurnLampSWRst == 1) &&
           ((TurnLight_DW.HazardLightRqst_prev !=
             TurnLight_DW.HazardLightRqst_start) &&
            (TurnLight_DW.HazardLightRqst_start == 1))) || ((HazardRqst == 1) &&
           ((TurnLight_DW.TurnLightSwRqst_prev !=
             TurnLight_DW.TurnLightSwRqst_start) &&
            (TurnLight_DW.TurnLightSwRqst_prev == 1)))) {
        TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLON;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLON' */
        TurnLight_TurnLight_ALLON();

        /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLON' */
        TurnLight_B.TurnLight_Decide = ALLON;
      } else if (TurnLight_B.TurnLampSWRst == 0) {
        TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLOFF;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLOFF' */
        TurnLight_TurnLight_ALLOFF();

        /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLOFF' */
        TurnLight_B.TurnLight_Decide = ALLOFF;
      } else {
        if (TurnLight_DW.LeftRetry == 1) {
          TurnLight_DW.is_c2_TurnLight = TurnLight_IN_LEFTON;
          TurnLight_DW.LeftRetry = 0U;
          TurnLight_DecideLeftLongOrShort();
        }
      }
      break;

     default:   //RIGHTON 右转状态
      /* case IN_RIGHTON: */    
      if (((TurnLight_B.TurnLampSWRst == 2) &&
           ((TurnLight_DW.HazardLightRqst_prev !=
             TurnLight_DW.HazardLightRqst_start) &&
            (TurnLight_DW.HazardLightRqst_start == 1))) || ((HazardRqst == 1) &&
           ((TurnLight_DW.TurnLightSwRqst_prev !=
             TurnLight_DW.TurnLightSwRqst_start) &&
            (TurnLight_DW.TurnLightSwRqst_prev == 2)))) {
        TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLON;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLON' */
        TurnLight_TurnLight_ALLON();

        /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLON' */
        TurnLight_B.TurnLight_Decide = ALLON;
      } else if (TurnLight_B.TurnLampSWRst == 0) {
        TurnLight_DW.is_c2_TurnLight = TurnLight_IN_ALLOFF;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_ALLOFF' */
        TurnLight_TurnLight_ALLOFF();

        /* End of Outputs for SubSystem: '<S2>/TurnLight_ALLOFF' */
        TurnLight_B.TurnLight_Decide = ALLOFF;
      } else {
        if (TurnLight_DW.RightRetry == 1) {
          TurnLight_DW.is_c2_TurnLight = TurnLight_IN_RIGHTON;
          TurnLight_DW.RightRetry = 0U;
          TurnLigh_DecideRightLongOrShort();
        }
      }
      break;
    }
    //把 ALLON → LEFTON/RIGHTON 的公共操作统一放到 switch 后面，避免在多个条件分支中重复生成代码
    if (guard2) { //双闪状态 + 新的左转操作 → 左转状态
      TurnLight_DW.is_c2_TurnLight = TurnLight_IN_LEFTON;
      TurnLight_DW.LeftRetry = 0U;
      TurnLight_DecideLeftLongOrShort();
    }

    if (guard1) { //双闪状态 + 新的右转操作 → 右转状态
      TurnLight_DW.is_c2_TurnLight = TurnLight_IN_RIGHTON;
      TurnLight_DW.RightRetry = 0U;
      TurnLigh_DecideRightLongOrShort();
    }
  }

  /* End of Chart: '<S2>/TurnLightSelector' */

  /* Chart: '<S2>/TurnLightDiag' incorporates:
   *  Inport: '<Root>/TurnLampCtrlCmd_FL_u8_signal'
   *  Inport: '<Root>/TurnLampCtrlCmd_FR_u8_signa'
   *  Inport: '<Root>/TurnLampCtrlCmd_RL_u8_signal'
   *  Inport: '<Root>/TurnLampFb_FL_u8_Signal'
   *  Inport: '<Root>/TurnLampFb_FR_u8_Signal'
   *  Inport: '<Root>/TurnLampFb_RL_u8_Signal'
   */
  //TurnLightDiag 诊断状态机。它由 TurnLight_Runnable_10ms() 每 10 ms 执行一次
  //，读取各灯的“控制命令”和“反馈状态”，发现短路或断路后，通过 RTE 服务下发处理命令。


  /*这段代码是 TurnLightDiag 诊断状态机中“前左灯 FL”部分的核心逻辑.它的作用是：
    1. 第一次运行时初始化诊断状态；
    2. 后续每 10 ms 读取前左灯的反馈和控制命令；
    3. 判断是否短路或断路；
    4. 通过 RTE 服务发送对应的关闭、开启或重试请求。*/
  if (TurnLight_DW.is_active_c4_TurnLight == 0U) {  //TurnLightDiag 这个 Chart 是否已经初始化
    TurnLight_DW.is_active_c4_TurnLight = 1U;
    TurnLight_DW.is_FLDIAG = TurnLight_IN_IDLE; //is_FLDIAG：前左灯当前处于什么诊断状态；
    TurnLight_DW.is_FRDIAG = TurnLight_IN_IDLE; //is_FRDIAG：前右灯诊断状态；
    TurnLight_DW.is_RLDIAG = TurnLight_IN_IDLE; //
    TurnLight_DW.is_RRDIAG = TurnLight_IN_IDLE; //
    //这些变量会跨越多个 10 ms 周期保存，不能定义成普通局部变量，否则状态每次调用都会丢失。
  } else {
    (void)Rte_Read_TurnLampFb_FL_u8_Signal(&tmpRead_6); //将前左灯反馈写入 tmpRead_6;tmpRead_6 == 0：正常; == 1：短路 ; == 2：断路
    (void)Rte_Read_TurnLampCtrlCmd_FL_u8_signal(&tmpRead_3);  //将前左灯当前控制命令写入 tmpRead_3
    /*本段代码只在：tmpRead_3 == 1时处理短路或断路，通常可以理解为：
    系统当前确实要求前左灯点亮*/
    switch (TurnLight_DW.is_FLDIAG) { //根据前左灯上一个周期保存的诊断状态决定本周期如何处理
     case TurnLight_IN_IDLE:  //IDLE 状态：检测短路或断路
      if ((tmpRead_6 == 1) && (tmpRead_3 == 1)) { //前左灯反馈 = SHORT并且前左灯控制命令 = ON
        //也就是：系统要求灯点亮，但反馈报告短路
        TurnLight_DW.is_FLDIAG = TurnLight_IN_SHORT;//进入短路状态

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_FLOFF_Short' */
        /* BusCreator: '<S10>/Bus Creator' incorporates:
         *  Constant: '<S10>/Constant'
         *  Constant: '<S10>/Constant1'
         *  Constant: '<S10>/Constant2'
         *  Constant: '<S10>/Constant3'
         *  Constant: '<S10>/Constant4'
         *  Constant: '<S10>/Constant5'
         *  Constant: '<S10>/Constant6'
         */
        //rtb_BusCreator 是一个 TurnLightService 请求结构体，用于向灯光服务传递完整控制信息。
        rtb_BusCreator.TurnLightPosition = FRONT_LEFT;  //要处理哪一个灯
        rtb_BusCreator.TurnLightOnOff = OFF;            //开启或关闭
        rtb_BusCreator.TurnLightDutyCycle = 0U;         //PWM 占空比
        rtb_BusCreator.TurnLightCycle = 0U;             //闪烁周期或模式参数
        rtb_BusCreator.TurnLightTimes = 0U;             //执行次数
        rtb_BusCreator.TurnLightNumber = 0U;            //灯光编号或附加参数
        rtb_BusCreator.TurnLightPriority = 2U;          //仲裁优先级
        //短路处理请求的关键值是关闭前左灯,目的是避免继续给故障灯施加驱动
        //其它字段设置为 0U，表示本次短路关闭请求不需要闪烁、周期、次数等额外动作。
        /* FunctionCaller: '<S10>/TurnLightService_TurnLightControl_Operation' incorporates:
         *  BusCreator: '<S10>/Bus Creator'
         */
        //这不是直接操作 GPIO，而是调用 AUTOSAR RTE 的 Client-Server 接口
        Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

        /* End of Outputs for SubSystem: '<S2>/TurnLight_FLOFF_Short' */
      } else {//检测断路
        if ((tmpRead_6 == 2) && (tmpRead_3 == 1)) { //系统要求灯点亮，但反馈报告断路
          TurnLight_DW.is_FLDIAG = TurnLight_IN_OPEN;

          /* Outputs for Function Call SubSystem: '<S2>/TurnLight_FLDouble_Open' */
          /* BusCreator: '<S9>/Bus Creator' incorporates:
           *  Constant: '<S9>/Constant'
           *  Constant: '<S9>/Constant1'
           *  Constant: '<S9>/Constant2'
           *  Constant: '<S9>/Constant3'
           *  Constant: '<S9>/Constant4'
           *  Constant: '<S9>/Constant5'
           *  Constant: '<S9>/Constant6'
           */
          rtb_BusCreator.TurnLightPosition = FRONT_LEFT;
          rtb_BusCreator.TurnLightOnOff = ON;
          rtb_BusCreator.TurnLightDutyCycle = 0U;
          rtb_BusCreator.TurnLightCycle = 4U;
          rtb_BusCreator.TurnLightTimes = 0U;
          rtb_BusCreator.TurnLightNumber = 0U;
          rtb_BusCreator.TurnLightPriority = 2U;

          /* FunctionCaller: '<S9>/TurnLightService_TurnLightControl_Operation' incorporates:
           *  BusCreator: '<S9>/Bus Creator'
           */
          //对前左灯发送开启/重试类请求
          Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

          /* End of Outputs for SubSystem: '<S2>/TurnLight_FLDouble_Open' */
        }
      }
      break;

     case TurnLight_IN_OPEN:  //OPEN 状态：等待反馈恢复正常；
      if (tmpRead_6 == 0) {
        TurnLight_DW.LeftRetry = 1U;
        TurnLight_DW.is_FLDIAG = TurnLight_IN_IDLE;
      }
      break;

     default:
      /* case IN_SHORT: */    //SHORT 状态：等待 TurnLight_B.TurnLight_Decide == ALLOFF 后复位。
      if (TurnLight_B.TurnLight_Decide == ALLOFF) {
        TurnLight_DW.is_FLDIAG = TurnLight_IN_IDLE;
      }
      break;
    }

    (void)Rte_Read_TurnLampFb_FR_u8_Signal(&tmpRead_7);
    switch (TurnLight_DW.is_FRDIAG) { //前右灯 FR
     case TurnLight_IN_IDLE:
      (void)Rte_Read_TurnLampCtrlCmd_FR_u8_signa(&tmpRead_4);
      if ((tmpRead_7 == 1) && (tmpRead_4 == 1)) {
        TurnLight_DW.is_FRDIAG = TurnLight_IN_SHORT;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_FROFF_Short' */
        /* BusCreator: '<S12>/Bus Creator' incorporates:
         *  Constant: '<S12>/Constant'
         *  Constant: '<S12>/Constant1'
         *  Constant: '<S12>/Constant2'
         *  Constant: '<S12>/Constant3'
         *  Constant: '<S12>/Constant4'
         *  Constant: '<S12>/Constant5'
         *  Constant: '<S12>/Constant6'
         */
        rtb_BusCreator.TurnLightPosition = FRONT_RIGHT;
        rtb_BusCreator.TurnLightOnOff = OFF;
        rtb_BusCreator.TurnLightDutyCycle = 0U;
        rtb_BusCreator.TurnLightCycle = 0U;
        rtb_BusCreator.TurnLightTimes = 0U;
        rtb_BusCreator.TurnLightNumber = 0U;
        rtb_BusCreator.TurnLightPriority = 2U;

        /* FunctionCaller: '<S12>/TurnLightService_TurnLightControl_Operation' incorporates:
         *  BusCreator: '<S12>/Bus Creator'
         */
        Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

        /* End of Outputs for SubSystem: '<S2>/TurnLight_FROFF_Short' */
      } else {
        if ((tmpRead_7 == 2) && (tmpRead_4 == 1)) {
          TurnLight_DW.is_FRDIAG = TurnLight_IN_OPEN;

          /* Outputs for Function Call SubSystem: '<S2>/TurnLight_FRDouble_Open' */
          /* BusCreator: '<S11>/Bus Creator' incorporates:
           *  Constant: '<S11>/Constant'
           *  Constant: '<S11>/Constant1'
           *  Constant: '<S11>/Constant2'
           *  Constant: '<S11>/Constant3'
           *  Constant: '<S11>/Constant4'
           *  Constant: '<S11>/Constant5'
           *  Constant: '<S11>/Constant6'
           */
          rtb_BusCreator.TurnLightPosition = FRONT_RIGHT;
          rtb_BusCreator.TurnLightOnOff = ON;
          rtb_BusCreator.TurnLightDutyCycle = 0U;
          rtb_BusCreator.TurnLightCycle = 4U;
          rtb_BusCreator.TurnLightTimes = 0U;
          rtb_BusCreator.TurnLightNumber = 0U;
          rtb_BusCreator.TurnLightPriority = 2U;

          /* FunctionCaller: '<S11>/TurnLightService_TurnLightControl_Operation' incorporates:
           *  BusCreator: '<S11>/Bus Creator'
           */
          Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

          /* End of Outputs for SubSystem: '<S2>/TurnLight_FRDouble_Open' */
        }
      }
      break;

     case TurnLight_IN_OPEN:
      if (tmpRead_7 == 0) {
        TurnLight_DW.RightRetry = 1U;
        TurnLight_DW.is_FRDIAG = TurnLight_IN_IDLE;
      }
      break;

     default:
      /* case IN_SHORT: */
      if (TurnLight_B.TurnLight_Decide == ALLOFF) {
        TurnLight_DW.is_FRDIAG = TurnLight_IN_IDLE;
      }
      break;
    }

    (void)Rte_Read_TurnLampFb_RL_u8_Signal(&tmpRead_8);
    switch (TurnLight_DW.is_RLDIAG) {
     case TurnLight_IN_IDLE:
      (void)Rte_Read_TurnLampCtrlCmd_RL_u8_signal(&tmpRead_5);
      if ((tmpRead_8 == 1) && (tmpRead_5 == 1)) {
        TurnLight_DW.is_RLDIAG = TurnLight_IN_SHORT;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_RLOFF_Short' */
        /* BusCreator: '<S17>/Bus Creator' incorporates:
         *  Constant: '<S17>/Constant'
         *  Constant: '<S17>/Constant1'
         *  Constant: '<S17>/Constant2'
         *  Constant: '<S17>/Constant3'
         *  Constant: '<S17>/Constant4'
         *  Constant: '<S17>/Constant5'
         *  Constant: '<S17>/Constant6'
         */
        rtb_BusCreator.TurnLightPosition = REAR_LEFT;
        rtb_BusCreator.TurnLightOnOff = OFF;
        rtb_BusCreator.TurnLightDutyCycle = 0U;
        rtb_BusCreator.TurnLightCycle = 0U;
        rtb_BusCreator.TurnLightTimes = 0U;
        rtb_BusCreator.TurnLightNumber = 0U;
        rtb_BusCreator.TurnLightPriority = 2U;

        /* FunctionCaller: '<S17>/TurnLightService_TurnLightControl_Operation' incorporates:
         *  BusCreator: '<S17>/Bus Creator'
         */
        Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

        /* End of Outputs for SubSystem: '<S2>/TurnLight_RLOFF_Short' */
      } else {
        if ((tmpRead_8 == 2) && (tmpRead_3 == 1)) {
          TurnLight_DW.is_RLDIAG = TurnLight_IN_OPEN;

          /* Outputs for Function Call SubSystem: '<S2>/TurnLight_RLDouble_Open' */
          /* BusCreator: '<S16>/Bus Creator' incorporates:
           *  Constant: '<S16>/Constant'
           *  Constant: '<S16>/Constant1'
           *  Constant: '<S16>/Constant2'
           *  Constant: '<S16>/Constant3'
           *  Constant: '<S16>/Constant4'
           *  Constant: '<S16>/Constant5'
           *  Constant: '<S16>/Constant6'
           */
          rtb_BusCreator.TurnLightPosition = REAR_LEFT;
          rtb_BusCreator.TurnLightOnOff = ON;
          rtb_BusCreator.TurnLightDutyCycle = 0U;
          rtb_BusCreator.TurnLightCycle = 4U;
          rtb_BusCreator.TurnLightTimes = 0U;
          rtb_BusCreator.TurnLightNumber = 0U;
          rtb_BusCreator.TurnLightPriority = 2U;

          /* FunctionCaller: '<S16>/TurnLightService_TurnLightControl_Operation' incorporates:
           *  BusCreator: '<S16>/Bus Creator'
           */
          Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

          /* End of Outputs for SubSystem: '<S2>/TurnLight_RLDouble_Open' */
        }
      }
      break;

     case TurnLight_IN_OPEN:
      if (tmpRead_8 == 0) {
        TurnLight_DW.LeftRetry = 1U;
        TurnLight_DW.is_RLDIAG = TurnLight_IN_IDLE;
      }
      break;

     default:
      /* case IN_SHORT: */
      if (TurnLight_B.TurnLight_Decide == ALLOFF) {
        TurnLight_DW.is_RLDIAG = TurnLight_IN_IDLE;
      }
      break;
    }

    switch (TurnLight_DW.is_RRDIAG) {
     case TurnLight_IN_IDLE:
      if ((tmpRead_6 == 1) && (tmpRead_3 == 1)) {
        TurnLight_DW.is_RRDIAG = TurnLight_IN_SHORT;

        /* Outputs for Function Call SubSystem: '<S2>/TurnLight_RRDouble_Short' */
        /* BusCreator: '<S19>/Bus Creator' incorporates:
         *  Constant: '<S19>/Constant'
         *  Constant: '<S19>/Constant1'
         *  Constant: '<S19>/Constant2'
         *  Constant: '<S19>/Constant3'
         *  Constant: '<S19>/Constant4'
         *  Constant: '<S19>/Constant5'
         *  Constant: '<S19>/Constant6'
         */
        rtb_BusCreator.TurnLightPosition = REAR_RIGHT;
        rtb_BusCreator.TurnLightOnOff = OFF;
        rtb_BusCreator.TurnLightDutyCycle = 0U;
        rtb_BusCreator.TurnLightCycle = 0U;
        rtb_BusCreator.TurnLightTimes = 0U;
        rtb_BusCreator.TurnLightNumber = 0U;
        rtb_BusCreator.TurnLightPriority = 2U;

        /* FunctionCaller: '<S19>/TurnLightService_TurnLightControl_Operation' incorporates:
         *  BusCreator: '<S19>/Bus Creator'
         */
        Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

        /* End of Outputs for SubSystem: '<S2>/TurnLight_RRDouble_Short' */
      } else {
        if ((tmpRead_6 == 2) && (tmpRead_3 == 1)) {
          TurnLight_DW.is_RRDIAG = TurnLight_IN_OPEN;

          /* Outputs for Function Call SubSystem: '<S2>/TurnLight_RRDouble_Open' */
          /* BusCreator: '<S18>/Bus Creator' incorporates:
           *  Constant: '<S18>/Constant'
           *  Constant: '<S18>/Constant1'
           *  Constant: '<S18>/Constant2'
           *  Constant: '<S18>/Constant3'
           *  Constant: '<S18>/Constant4'
           *  Constant: '<S18>/Constant5'
           *  Constant: '<S18>/Constant6'
           */
          rtb_BusCreator.TurnLightPosition = REAR_RIGHT;
          rtb_BusCreator.TurnLightOnOff = ON;
          rtb_BusCreator.TurnLightDutyCycle = 0U;
          rtb_BusCreator.TurnLightCycle = 4U;
          rtb_BusCreator.TurnLightTimes = 0U;
          rtb_BusCreator.TurnLightNumber = 0U;
          rtb_BusCreator.TurnLightPriority = 2U;

          /* FunctionCaller: '<S18>/TurnLightService_TurnLightControl_Operation' incorporates:
           *  BusCreator: '<S18>/Bus Creator'
           */
          Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator);

          /* End of Outputs for SubSystem: '<S2>/TurnLight_RRDouble_Open' */
        }
      }
      break;

     case TurnLight_IN_OPEN:
      if (tmpRead_6 == 0) {
        TurnLight_DW.RightRetry = 1U;
        TurnLight_DW.is_RRDIAG = TurnLight_IN_IDLE;
      }
      break;

     default:
      /* case IN_SHORT: */
      if (TurnLight_B.TurnLight_Decide == ALLOFF) {
        TurnLight_DW.is_RRDIAG = TurnLight_IN_IDLE;
      }
      break;
    }
  }

  /* End of Chart: '<S2>/TurnLightDiag' */
  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/TurnLight_Runnable_10ms' */

  /* Outport: '<Root>/TurnLampSwMode_u8_signa' */
  (void)Rte_Write_TurnLampSwMode_u8_signa(0U);
}

/* Model initialize function */
void TurnLight_Init(void)
{
  /* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
