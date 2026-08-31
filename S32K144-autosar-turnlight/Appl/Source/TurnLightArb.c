/*
 * File: TurnLightArb.c
 *
 * Code generated for Simulink model 'TurnLightArb'.
 *
 * Model version                  : 1.20
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Sat Aug 10 07:33:58 2024
 */

/*实现了一个转向灯控制仲裁模块（TurnLightArb）。它负责接收外部传入的转向灯控制命令
（包括位置、开关、优先级、闪烁模式等参数），并根据当前状态和优先级规则，输出四个转向灯
（左前 FL、右前 FR、左后 RL、右后 RR）的最终驱动信号（OFF、常亮、双闪、短闪等）。
代码采用周期性任务（10ms）与外部服务调用结合的方式运行.

TurnLightArb.c 是一个由 Simulink 生成的“转向灯命令仲裁器”。它不直接产生闪烁波形，
而是接收灯光请求，按优先级筛选后，为四个灯输出模式编号。
*/

#include "TurnLightArb.h"
#include "TurnLightArb_private.h"

/* Named constants for Chart: '<S1>/TurnLightOutArb' */
#define TurnLightArb_IN_CheckPriority  ((uint8)1U)
#define TurnLightArb_IN_Idle           ((uint8)2U)
//这里只输出“模式”，真正的亮灭周期通常由下游 TurnLightAct 等执行模块完成。
#define TurnLightArb_TLOUT_OFF         ((uint8)0U)  //关闭
#define TurnLightArb_TLOUT_NORMAL_ON   ((uint8)1U)  //普通闪烁
#define TurnLightArb_TLOUT_DOUBLE_ON   ((uint8)2U)  //双闪模式
#define TurnLightArb_TLOUT_SHORT_ON    ((uint8)3U)  //短闪模式

/* Block signals (default storage) */
B_TurnLightArb_T TurnLightArb_B;    //输出信号，四个灯的最终输出

/* Block states (default storage) */
DW_TurnLightArb_T TurnLightArb_DW;  //保存当前命令、当前优先级和状态机状态。

/* Forward declaration for local functions */
static void TurnLightArb_JurgeOutputMode(const uint8 *DataTypeConversion, const
  uint8 *DataTypeConversion1);
static void TurnLightArb_CheckPriority_flow(const uint8 *DataTypeConversion,
  const uint8 *DataTypeConversion1);

/* Function for Chart: '<S1>/TurnLightOutArb' */
/*输出模式判决
该函数根据输入的 TurnLightPosition 和 TurnLightOnOff，结合命令中的其他参数
（占空比、周期、次数、灯数量），决定每个灯的最终输出模式。
逻辑本质上是一个大型条件判断树，覆盖了各种组合情况*/
//根据“控制位置 + 开关状态 + 闪烁参数”，选择四个转向灯各自的输出模式
//*DataTypeConversion   // TurnLightPosition，控制哪个灯
//*DataTypeConversion1  // TurnLightOnOff，打开或关闭
//其他判断参数直接从全局命令中读取：TurnLightDutyCycle;TurnLightCycle;TurnLightTimes;TurnLightNumber
//最终修改四个全局输出：TurnLightArb_B.TurnLight_FL_Out ......
static void TurnLightArb_JurgeOutputMode(const uint8 *DataTypeConversion, const
  uint8 *DataTypeConversion1)
{
  boolean aVarTruthTableCondition_1;
  boolean aVarTruthTableCondition_10;
  boolean aVarTruthTableCondition_11;
  boolean aVarTruthTableCondition_12;
  boolean aVarTruthTableCondition_13;
  boolean aVarTruthTableCondition_14;
  boolean aVarTruthTableCondition_15;
  boolean aVarTruthTableCondition_3;
  boolean aVarTruthTableCondition_4;
  boolean aVarTruthTableCondition_5;
  boolean aVarTruthTableCondition_6;
  boolean aVarTruthTableCondition_7;
  boolean aVarTruthTableCondition_8;
  boolean aVarTruthTableCondition_9;
  boolean tmp;
  boolean tmp_0;
  //开头把复杂判断拆成许多布尔变量
  //这是 Simulink Truth Table 自动生成代码的典型形式，所以变量名可读性较差。
  aVarTruthTableCondition_1 = *DataTypeConversion == LEFT;
  aVarTruthTableCondition_3 = *DataTypeConversion == ALL;
  aVarTruthTableCondition_4 = *DataTypeConversion == FRONT_LEFT;
  aVarTruthTableCondition_5 = *DataTypeConversion == FRONT_RIGHT;
  aVarTruthTableCondition_6 = *DataTypeConversion == REAR_LEFT;
  aVarTruthTableCondition_7 = *DataTypeConversion == REAR_RIGHT;
  aVarTruthTableCondition_8 = *DataTypeConversion1 == OFF;
  aVarTruthTableCondition_9 = *DataTypeConversion1 == ON;

  /* DataStoreRead: '<S1>/Data Store Read' */
  aVarTruthTableCondition_10 =
    TurnLightArb_DW.TurnLightCtrlCmd.TurnLightDutyCycle == 0; //DutyCycle==0
  aVarTruthTableCondition_11 = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightCycle ==
    8;  //Cycle == 8
  aVarTruthTableCondition_12 = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightCycle ==
    4;  //Cycle == 4
  aVarTruthTableCondition_13 = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightTimes ==
    0;  //Times == 0
  aVarTruthTableCondition_14 = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightNumber ==
    0;  //Number == 0
  aVarTruthTableCondition_15 = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightNumber ==
    3;  //Number == 3
  if (aVarTruthTableCondition_3 && aVarTruthTableCondition_8) {
      //等价于if (position == ALL && onOff == OFF)
    /*  'ALL_OFF':  */
    TurnLightArb_B.TurnLight_FL_Out = TurnLightArb_TLOUT_OFF;
    TurnLightArb_B.TurnLight_FR_Out = TurnLightArb_TLOUT_OFF;
    TurnLightArb_B.TurnLight_RL_Out = TurnLightArb_TLOUT_OFF;
    TurnLightArb_B.TurnLight_RR_Out = TurnLightArb_TLOUT_OFF;
  } else if (aVarTruthTableCondition_4 && aVarTruthTableCondition_8) {
    /*  'FL_OFF':  */
    TurnLightArb_B.TurnLight_FL_Out = TurnLightArb_TLOUT_OFF;
  } else if (aVarTruthTableCondition_5 && aVarTruthTableCondition_8) {
    /*  'FR_OFF':  */
    TurnLightArb_B.TurnLight_FR_Out = TurnLightArb_TLOUT_OFF;
  } else if (aVarTruthTableCondition_6 && aVarTruthTableCondition_8) {
    /*  'RL_OFF':  */
    TurnLightArb_B.TurnLight_RL_Out = TurnLightArb_TLOUT_OFF;
  } else if (aVarTruthTableCondition_7 && aVarTruthTableCondition_8) {
    /*  'RR_OFF':  */
    TurnLightArb_B.TurnLight_RR_Out = TurnLightArb_TLOUT_OFF;
  } else {
    aVarTruthTableCondition_8 = !aVarTruthTableCondition_12;
    tmp = !aVarTruthTableCondition_15;
    if (aVarTruthTableCondition_1 && aVarTruthTableCondition_9 &&
        aVarTruthTableCondition_10 && aVarTruthTableCondition_11 &&
        aVarTruthTableCondition_8 && aVarTruthTableCondition_13 &&
        aVarTruthTableCondition_14 && tmp) {
      /*  'LEFT_NORMAL_ON':  */
      TurnLightArb_B.TurnLight_FL_Out = TurnLightArb_TLOUT_NORMAL_ON;
      TurnLightArb_B.TurnLight_RL_Out = TurnLightArb_TLOUT_NORMAL_ON;
    } else {
      tmp_0 = *DataTypeConversion == RIGHT && aVarTruthTableCondition_9 &&
        aVarTruthTableCondition_10 && aVarTruthTableCondition_11 &&
        aVarTruthTableCondition_8 && aVarTruthTableCondition_13;
      if (tmp_0 && aVarTruthTableCondition_14 && tmp) {
        /*  'RIGHT_NORMAL_ON':  */
        TurnLightArb_B.TurnLight_FR_Out = TurnLightArb_TLOUT_NORMAL_ON;
        TurnLightArb_B.TurnLight_RR_Out = TurnLightArb_TLOUT_NORMAL_ON;
      } else if (aVarTruthTableCondition_3 && aVarTruthTableCondition_9 &&
                 aVarTruthTableCondition_10 && aVarTruthTableCondition_11 &&
                 aVarTruthTableCondition_8 && aVarTruthTableCondition_13 &&
                 aVarTruthTableCondition_14 && tmp) {
        /*  'ALL_NORMAL_ON':  */
        TurnLightArb_B.TurnLight_FL_Out = TurnLightArb_TLOUT_NORMAL_ON;
        TurnLightArb_B.TurnLight_RL_Out = TurnLightArb_TLOUT_NORMAL_ON;
        TurnLightArb_B.TurnLight_FR_Out = TurnLightArb_TLOUT_NORMAL_ON;
        TurnLightArb_B.TurnLight_RR_Out = TurnLightArb_TLOUT_NORMAL_ON;
      } else {
        aVarTruthTableCondition_3 = !aVarTruthTableCondition_14;
        if (aVarTruthTableCondition_1 && aVarTruthTableCondition_9 &&
            aVarTruthTableCondition_10 && aVarTruthTableCondition_11 &&
            aVarTruthTableCondition_8 && aVarTruthTableCondition_13 &&
            aVarTruthTableCondition_3 && aVarTruthTableCondition_15) {
          /*  'LEFT_SHORT_ON':  */
          TurnLightArb_B.TurnLight_FL_Out = TurnLightArb_TLOUT_SHORT_ON;
          TurnLightArb_B.TurnLight_RL_Out = TurnLightArb_TLOUT_SHORT_ON;
        } else if (tmp_0 && aVarTruthTableCondition_3 &&
                   aVarTruthTableCondition_15) {
          /*  'RIGHT_SHORT_ON':  */
          TurnLightArb_B.TurnLight_FR_Out = TurnLightArb_TLOUT_SHORT_ON;
          TurnLightArb_B.TurnLight_RR_Out = TurnLightArb_TLOUT_SHORT_ON;
        } else {
          aVarTruthTableCondition_8 = !aVarTruthTableCondition_11;
          if (aVarTruthTableCondition_4 && aVarTruthTableCondition_9 &&
              aVarTruthTableCondition_10 && aVarTruthTableCondition_8 &&
              aVarTruthTableCondition_12 && aVarTruthTableCondition_13 &&
              aVarTruthTableCondition_14 && tmp) {
            /*  'FL_DOUBLE':  */
            TurnLightArb_B.TurnLight_FL_Out = TurnLightArb_TLOUT_DOUBLE_ON;
          } else if (aVarTruthTableCondition_5 && aVarTruthTableCondition_9 &&
                     aVarTruthTableCondition_10 && aVarTruthTableCondition_8 &&
                     aVarTruthTableCondition_12 && aVarTruthTableCondition_13 &&
                     aVarTruthTableCondition_14 && tmp) {
            /*  'FR_DOUBLE':  */
            TurnLightArb_B.TurnLight_FR_Out = TurnLightArb_TLOUT_DOUBLE_ON;
          } else if (aVarTruthTableCondition_6 && aVarTruthTableCondition_9 &&
                     aVarTruthTableCondition_10 && aVarTruthTableCondition_8 &&
                     aVarTruthTableCondition_12 && aVarTruthTableCondition_13 &&
                     aVarTruthTableCondition_14) {
            /*  'RL_DOUBLE':  */
            TurnLightArb_B.TurnLight_RL_Out = TurnLightArb_TLOUT_DOUBLE_ON;
          } else {
            if (aVarTruthTableCondition_7 && aVarTruthTableCondition_9 &&
                aVarTruthTableCondition_10 && aVarTruthTableCondition_8 &&
                aVarTruthTableCondition_12 && aVarTruthTableCondition_13 &&
                aVarTruthTableCondition_14) {
              /*  'RR_DOUBLE':  */
              TurnLightArb_B.TurnLight_RR_Out = TurnLightArb_TLOUT_DOUBLE_ON;
            }
          }
        }
      }
    }
  }
}

/* Function for Chart: '<S1>/TurnLightOutArb' */
//功能是：比较新请求与当前控制命令的优先级，决定是否接受并执行新请求
/*满足:"新请求优先级 >= 当前优先级"时才会：
1. 更新 CurrentPriority
2. 清除 IsFunctionCall
3. 调用输出模式判断函数
因此，数值越大的优先级越高；相同优先级也允许覆盖。
值得注意的是，CurrentPriority 没有正常的复位逻辑。
一旦接受高优先级请求，后续低优先级请求将无法生效。*/
static void TurnLightArb_CheckPriority_flow(const uint8 *DataTypeConversion,
  const uint8 *DataTypeConversion1)
{
  /* DataStoreRead: '<S1>/Data Store Read' */
  if (TurnLightArb_DW.TurnLightCtrlCmd.TurnLightPriority >=
      TurnLightArb_DW.CurrentPriority) {
    TurnLightArb_DW.CurrentPriority =
      TurnLightArb_DW.TurnLightCtrlCmd.TurnLightPriority;//使用新请求的优先级更新
    TurnLightArb_DW.IsFunctionCall = false; //清除待处理标志
    //判断并更新灯光输出模式
    TurnLightArb_JurgeOutputMode(DataTypeConversion, DataTypeConversion1);
  }

  /* End of DataStoreRead: '<S1>/Data Store Read' */
}


/*周期性运行任务 TurnLightArb_Runnable_10ms,每 10ms 被调度一次（TID1 周期任务）。
它负责读取待处理命令、驱动状态机、执行优先级仲裁，并通过 RTE 输出四个灯的模式。*/
/* Model step function for TID1 */
void TurnLightArb_Runnable_10ms(void)  /* Explicit Task: Runnable_10ms */
{
  uint8 DataTypeConversion;
  uint8 DataTypeConversion1;

  /* RootInportFunctionCallGenerator generated from: '<Root>/Runnable_10ms' incorporates:
   *  SubSystem: '<Root>/Runnable_10ms_sys'
   */
  /* DataTypeConversion: '<S1>/Data Type Conversion' incorporates:
   *  DataStoreRead: '<S1>/Data Store Read'
   */
  //DataTypeConversion：控制位置，如 LEFT、RIGHT、ALL
  DataTypeConversion = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightPosition;

  /* DataTypeConversion: '<S1>/Data Type Conversion1' incorporates:
   *  DataStoreRead: '<S1>/Data Store Read'
   */
  //- DataTypeConversion1：开关状态，即 ON 或 OFF
  DataTypeConversion1 = TurnLightArb_DW.TurnLightCtrlCmd.TurnLightOnOff;


  /*内部状态机（Chart 模块 TurnLightOutArb）
  包含两个主要状态：Idle（空闲）和 CheckPriority（检查优先级）。
  - Idle：发现 IsFunctionCall=true 后进入仲裁。
  - CheckPriority：当请求标志被清除后回到 Idle。
  无论有没有新请求，每10 ms都会通过 RTE 写出当前四个灯的模式。
  首次运行只会初始化状态机为 Idle，不会立即处理已到达的请求；请求会在下一周期处理。*/
  /* Chart: '<S1>/TurnLightOutArb' */
  if (TurnLightArb_DW.is_active_c3_TurnLightArb == 0U) {
    TurnLightArb_DW.is_active_c3_TurnLightArb = 1U;
    TurnLightArb_DW.is_c3_TurnLightArb = TurnLightArb_IN_Idle;
  } else if (TurnLightArb_DW.is_c3_TurnLightArb == TurnLightArb_IN_CheckPriority)
  { //CheckPriority状态
    if (!TurnLightArb_DW.IsFunctionCall) {  //如果IsFunctionCall 清零
      TurnLightArb_DW.is_c3_TurnLightArb = TurnLightArb_IN_Idle;  //状态机回到 Idle
    }
  } else {
    /* case IN_Idle: */ //当服务接口设置 IsFunctionCall=true，表示有新请求到达
    if (TurnLightArb_DW.IsFunctionCall) {
      TurnLightArb_DW.is_c3_TurnLightArb = TurnLightArb_IN_CheckPriority;//进入 CheckPriority
      //检查请求优先级
      //优先级检查通过时更新灯光输出
      TurnLightArb_CheckPriority_flow(&DataTypeConversion, &DataTypeConversion1);
    }
  }

  /* End of Chart: '<S1>/TurnLightOutArb' */
  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/Runnable_10ms' */
  //状态机执行完后，无论本周期是否收到新命令，函数都会通过 RTE 写出当前输出:
  /* Outport: '<Root>/TurnLamp_FL_Out_u8_Signal' */
  (void)Rte_Write_TurnLamp_FL_Out_u8_Signal(TurnLightArb_B.TurnLight_FL_Out);

  /* Outport: '<Root>/TurnLamp_RL_Out_u8_Signal' */
  (void)Rte_Write_TurnLamp_RL_Out_u8_Signal(TurnLightArb_B.TurnLight_RL_Out);

  /* Outport: '<Root>/TurnLapm_FR_Out_u8_Signal' */
  (void)Rte_Write_TurnLapm_FR_Out_u8_Signal(TurnLightArb_B.TurnLight_FR_Out);

  /* Outport: '<Root>/TurnLapme_RR_Out_u8_Signal' */
  (void)Rte_Write_TurnLapme_RR_Out_u8_Signal(TurnLightArb_B.TurnLight_RR_Out);
}

/*外部服务调用接口 TurnLightService_TurnLightControl_Operation
供上层或其它模块调用，用于更新转向灯控制参数（如新命令到达）。
调用时，将传入的结构体 TurnLightService *TurnLightCtrlPara 写入
数据存储 TurnLightArb_DW.TurnLightCtrlCmd，并设置 IsFunctionCall = true 标志，表示有新的请求待处理。*/

/* Output function */
//Rte_Call_TurnLightService_TurnLightControl_Operation(&rtb_BusCreator_oj);
void TurnLightService_TurnLightControl_Operation(const TurnLightService
  *TurnLightCtrlPara)
{
  /* Outputs for Function Call SubSystem: '<S3>/TurnLightService_TurnLightControl_Operation_sys' */
  /* DataStoreWrite: '<S5>/Data Store Write1' incorporates:
   *  Constant: '<S5>/Constant'
   */
  TurnLightArb_DW.IsFunctionCall = true;//表示有新的灯光服务请求到达。

  /* SignalConversion generated from: '<S5>/TurnLightCtrlPara' incorporates:
   *  DataStoreWrite: '<S5>/Data Store Write'
   */
  TurnLightArb_DW.TurnLightCtrlCmd = *TurnLightCtrlPara;//把 rtb_BusCreator 指向的整个结构体复制到仲裁模块的内部存储中。

  /* End of Outputs for SubSystem: '<S3>/TurnLightService_TurnLightControl_Operation_sys' */
}

//把当前输出写入 RTE。全局变量依靠 C 的静态初始化默认变为0
//所以初始状态通常为：四灯关闭,当前优先级 = 0,无待处理请求,状态机未激活
/* Model initialize function */
void TurnLightArb_Init(void)
{
  /* SystemInitialize for Outport: '<Root>/TurnLamp_FL_Out_u8_Signal' */
  (void)Rte_Write_TurnLamp_FL_Out_u8_Signal(TurnLightArb_B.TurnLight_FL_Out);

  /* SystemInitialize for Outport: '<Root>/TurnLamp_RL_Out_u8_Signal' */
  (void)Rte_Write_TurnLamp_RL_Out_u8_Signal(TurnLightArb_B.TurnLight_RL_Out);

  /* SystemInitialize for Outport: '<Root>/TurnLapm_FR_Out_u8_Signal' */
  (void)Rte_Write_TurnLapm_FR_Out_u8_Signal(TurnLightArb_B.TurnLight_FR_Out);

  /* SystemInitialize for Outport: '<Root>/TurnLapme_RR_Out_u8_Signal' */
  (void)Rte_Write_TurnLapme_RR_Out_u8_Signal(TurnLightArb_B.TurnLight_RR_Out);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
