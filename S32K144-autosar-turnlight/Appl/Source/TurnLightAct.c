/*
 * File: TurnLightAct.c
 *
 * Code generated for Simulink model 'TurnLightAct'.
 *
 * Model version                  : 1.16
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Fri Aug  9 20:03:26 2024
 *
 * Target selection: autosar.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

//AUTOSAR 转向灯执行器代码。核心职责是：每 10 ms 读取四路灯光模式，
//根据状态机生成闪烁波形，并通过 RTE 控制实际灯具。

#include "TurnLightAct.h"
#include "TurnLightAct_private.h"

/* Named constants for Chart: '<S3>/TurnLampActuator' */
#define TurnLightAct_IN_Double         ((uint8)1U)
#define TurnLightAct_IN_NO_ACTIVE_CHILD ((uint8)0U)
#define TurnLightAct_IN_Normal         ((uint8)2U)
#define TurnLightAct_IN_OFF            ((uint8)3U)
#define TurnLightAct_IN_OFF_d          ((uint8)1U)
#define TurnLightAct_IN_ON             ((uint8)2U)


//TurnLightAct_B：当前输出值和闪烁次数，如 TurnLampCtrlCmd、Counter
//TurnLightAct_DW：Stateflow 状态，包括当前主状态、子状态、激活标志和时间计数器。
/* Block signals (default storage) */
B_TurnLightAct_T TurnLightAct_B;

/* Block states (default storage) */
DW_TurnLightAct_T TurnLightAct_DW;

//四个辅助函数负责调用底层 IoHwAb
/* Output and update for function-call system: '<S3>/Function-Call Subsystem' */
void TurnLight_FunctionCallSubsystem(uint8 rtu_CtrlCmd)
{
  /* FunctionCaller: '<S7>/IoHwAb_TurnLightCtrlCmd_FL_Operation' incorporates:
   *  DataTypeConversion: '<S7>/Data Type Conversion'
   *  Logic: '<S7>/NOT'
   */
  Rte_Call_IoHwAb_TurnLightCtrlCmd_FL_Operation((uint8)(rtu_CtrlCmd == 0));
}

/* Output and update for function-call system: '<S4>/Function-Call Subsystem1' */
void TurnLigh_FunctionCallSubsystem1(uint8 rtu_CtrlCmd)
{
  /* FunctionCaller: '<S9>/IoHwAb_TurnLightCtrlCmd_FR_Operation' incorporates:
   *  DataTypeConversion: '<S9>/Data Type Conversion'
   *  Logic: '<S9>/Logical Operator1'
   */
  Rte_Call_IoHwAb_TurnLightCtrlCmd_FR_Operation((uint8)(rtu_CtrlCmd == 0));
}

/* Output and update for function-call system: '<S5>/Function-Call Subsystem2' */
void TurnLigh_FunctionCallSubsystem2(uint8 rtu_CtrlCmd)
{
  /* FunctionCaller: '<S11>/IoHwAb_TurnLightCtrlCmd_RL_Operation' incorporates:
   *  DataTypeConversion: '<S11>/Data Type Conversion'
   *  Logic: '<S11>/Logical Operator1'
   */
  Rte_Call_IoHwAb_TurnLightCtrlCmd_RL_Operation((uint8)(rtu_CtrlCmd == 0));
}

/* Output and update for function-call system: '<S6>/Function-Call Subsystem3' */
void TurnLigh_FunctionCallSubsystem3(uint8 rtu_CtrlCmd)
{
  /* FunctionCaller: '<S13>/IoHwAb_TurnLightCtrlCmd_RR_Operation' incorporates:
   *  DataTypeConversion: '<S13>/Data Type Conversion'
   *  Logic: '<S13>/Logical Operator1'
   */
  Rte_Call_IoHwAb_TurnLightCtrlCmd_RR_Operation((uint8)(rtu_CtrlCmd == 0));
}



/* Model step function for TID1 */
void TurnLightAct_Runnable_10ms(void)  /* Explicit Task: Runnable_10ms */
{
  sint32 tmp;
  uint8 tmpRead;
  uint8 tmpRead_0;
  uint8 tmpRead_1;
  uint8 tmpRead_2;

  /* RootInportFunctionCallGenerator generated from: '<Root>/Runnable_10ms' incorporates:
   *  SubSystem: '<Root>/Runnable_10ms_sys'
   */

  //这一段是左前转向灯 FL 的完整 Stateflow 状态机。它每 10 ms 执行一次，
  //根据 TurnLamp_FL_Out_u8_Signal 生成左前灯的开关波形

  /* Chart: '<S3>/TurnLampActuator' incorporates:
   *  Inport: '<Root>/TurnLamp_FL_Out_u8_Signal'
   */
  //Runnable 每 10 ms 调用一次,因此计数每+1，代表10ms
  if (TurnLightAct_DW.temporalCounter_i1_n < 63U) {
    TurnLightAct_DW.temporalCounter_i1_n++;
  }

  if (TurnLightAct_DW.is_active_c3_TurnLightActLib_f == 0U) {
    //第一次运行：初始化为 OFF
    TurnLightAct_DW.is_active_c3_TurnLightActLib_f = 1U;//将状态机标记为已激活。
    TurnLightAct_DW.is_c3_TurnLightActLib_g = TurnLightAct_IN_OFF;//将状态机标记为已激活。

    /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
    (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);//左转向灯 CAN 状态写为 0。
    TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;//左前灯命令设为 0。

    /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
    TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);//调用底层硬件接口关闭灯。

    /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
  } else {
    (void)Rte_Read_TurnLamp_FL_Out_u8_Signal(&tmpRead);//读取左前灯模式
    //把地址传给 RTE 后，RTE 就可以通过这个地址修改 tmpRead
    //简化之后就是tmpRead = Rte_TurnLightArb_TurnLamp_FL_Out_u8_Signal
    //共享缓冲变量是：Rte_TurnLightArb_TurnLamp_FL_Out_u8_Signal
    //这个变量就是 TurnLightArb 和 TurnLightAct 之间的 RTE 数据缓冲区
    //TurnLightArb 中有：Rte_Write_TurnLamp_FL_Out_u8_Signal
    //就是通过这个函数来从Arb发送到Act的


    switch (TurnLightAct_DW.is_c3_TurnLightActLib_g) {
     case TurnLightAct_IN_Double://快速闪烁状态
      if (tmpRead == 0) { //输入变为 0：退出快速闪烁
        TurnLightAct_DW.is_Double_d = TurnLightAct_IN_NO_ACTIVE_CHILD;
        TurnLightAct_DW.is_c3_TurnLightActLib_g = TurnLightAct_IN_OFF;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

        /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
        TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);

        /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
      } else if (TurnLightAct_DW.is_Double_d == TurnLightAct_IN_OFF_d) {
        //当前处于 Double.OFF
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);
        if (TurnLightAct_DW.temporalCounter_i1_n >= 20) {
          ////保持灭灯约 200 ms，然后切换到亮灯。
          TurnLightAct_DW.is_Double_d = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1_n = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c2 = 1U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(1U);

          /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
          TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);//调用硬件开灯

          /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
        }
      } else {//当前处于 Double.ON
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 1U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(1U);
        if (TurnLightAct_DW.temporalCounter_i1_n >= 20) {
          //保持亮灯约 200 ms，然后切换到灭灯
          TurnLightAct_DW.is_Double_d = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_n = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);

          /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
          TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);

          /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
        }
      }
      break;//保持亮灯约 200 ms，然后切换到灭灯。因此快速模式时序是:OFF 200 ms → ON 200 ms → OFF 200 ms → ON 200 ms → ...

     case TurnLightAct_IN_Normal://普通闪烁状态
      if (((tmpRead == 2) && (TurnLightAct_B.Counter_e == 3)) || (tmpRead == 0))
      //输入为 2 且已经闪烁 3 次：自动停止;输入为 0：立即停止。
      {
        if (TurnLightAct_DW.is_Normal_n == TurnLightAct_IN_ON) {
          //如果停止时灯正处于 ON 子状态，则把当前这次点亮计入闪烁次数，然后清除子状态
          tmp = TurnLightAct_B.Counter_e + 1;
          if (TurnLightAct_B.Counter_e + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter_e = (uint8)tmp;
          TurnLightAct_DW.is_Normal_n = TurnLightAct_IN_NO_ACTIVE_CHILD;
        } else {
          TurnLightAct_DW.is_Normal_n = TurnLightAct_IN_NO_ACTIVE_CHILD;
        }

        TurnLightAct_DW.is_c3_TurnLightActLib_g = TurnLightAct_IN_OFF;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

        /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
        TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);

        /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
      } else if (TurnLightAct_DW.is_Normal_n == TurnLightAct_IN_OFF_d) {
        //当前处于 Normal.OFF:
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);
        if (TurnLightAct_DW.temporalCounter_i1_n >= 40) {
          //保持灭灯约 400 ms，然后切换为亮灯。
          TurnLightAct_DW.is_Normal_n = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1_n = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c2 = 1U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(1U);

          /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
          TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);

          /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
        }
      } else {//当前处于 Normal.ON
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 1U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(1U);
        if (TurnLightAct_DW.temporalCounter_i1_n >= 40) {
          //保持亮灯约 400 ms，然后:
          tmp = TurnLightAct_B.Counter_e + 1;//闪烁次数 Counter_e 加一
          if (TurnLightAct_B.Counter_e + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter_e = (uint8)tmp;
          TurnLightAct_DW.is_Normal_n = TurnLightAct_IN_OFF_d;//子状态回到 OFF
          TurnLightAct_DW.temporalCounter_i1_n = 0U;//时间计数器清零
          TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;//关闭灯

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);

          /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
          TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);

          /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
        }
      }
      break;//因此普通模式时序是：OFF 400 ms → ON 400 ms → OFF 400 ms → ON 400 ms → ...闪3次然后结束。检测到 Counter == 3，退出 Normal

     default://default 实际代表 TurnLightAct_IN_OFF
      /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
      /* case IN_OFF: */
      //每个周期首先保证：CAN状态 = 0;灯命令 = 0;
      (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);
      TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;
      if ((tmpRead == 1) || (tmpRead == 2)) {
        //输入为 1 或 2，Normal模式
        TurnLightAct_B.Counter_e = 0U;
        TurnLightAct_DW.is_c3_TurnLightActLib_g = TurnLightAct_IN_Normal;//进入Normal
        TurnLightAct_DW.is_Normal_n = TurnLightAct_IN_OFF_d;
        TurnLightAct_DW.temporalCounter_i1_n = 0U;
        TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);//用于告诉其他软件模块“左转向灯当前亮还是灭”

        /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
        TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);//第二条是实际硬件命令

        /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
      } else {  //输入为3，快速模式
        if (tmpRead == 3) {
          TurnLightAct_DW.is_c3_TurnLightActLib_g = TurnLightAct_IN_Double;
          TurnLightAct_DW.is_Double_d = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_n = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c2 = 0U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampL_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampL_u8_signal(0U);

          /* Outputs for Function Call SubSystem: '<S3>/Function-Call Subsystem' */
          TurnLight_FunctionCallSubsystem(TurnLightAct_B.TurnLampCtrlCmd_c2);

          /* End of Outputs for SubSystem: '<S3>/Function-Call Subsystem' */
        }
      }
      break;
    }
  }

  /* End of Chart: '<S3>/TurnLampActuator' */

  /* Chart: '<S5>/TurnLampActuator' incorporates:
   *  Inport: '<Root>/TurnLamp_RL_Out_u8_Signal'
   */
  if (TurnLightAct_DW.temporalCounter_i1_l < 63U) {
    TurnLightAct_DW.temporalCounter_i1_l++;
  }

  if (TurnLightAct_DW.is_active_c3_TurnLightActLib == 0U) {
    TurnLightAct_DW.is_active_c3_TurnLightActLib = 1U;
    TurnLightAct_DW.is_c3_TurnLightActLib = TurnLightAct_IN_OFF;

    /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
    (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);
    TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

    /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
    TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

    /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
  } else {
    (void)Rte_Read_TurnLamp_RL_Out_u8_Signal(&tmpRead_0);
    switch (TurnLightAct_DW.is_c3_TurnLightActLib) {
     case TurnLightAct_IN_Double:
      if (tmpRead_0 == 0) {
        TurnLightAct_DW.is_Double_b = TurnLightAct_IN_NO_ACTIVE_CHILD;
        TurnLightAct_DW.is_c3_TurnLightActLib = TurnLightAct_IN_OFF;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);
        TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

        /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
        TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

        /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
      } else if (TurnLightAct_DW.is_Double_b == TurnLightAct_IN_OFF_d) {
        TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);
        if (TurnLightAct_DW.temporalCounter_i1_l >= 20) {
          TurnLightAct_DW.is_Double_b = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1_l = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c = 1U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(1U);

          /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
          TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

          /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
        }
      } else {
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd_c = 1U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(1U);
        if (TurnLightAct_DW.temporalCounter_i1_l >= 20) {
          TurnLightAct_DW.is_Double_b = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_l = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);

          /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
          TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

          /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
        }
      }
      break;

     case TurnLightAct_IN_Normal:
      if (((tmpRead_0 == 2) && (TurnLightAct_B.Counter_c == 3)) || (tmpRead_0 ==
           0)) {
        if (TurnLightAct_DW.is_Normal_p == TurnLightAct_IN_ON) {
          tmp = TurnLightAct_B.Counter_c + 1;
          if (TurnLightAct_B.Counter_c + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter_c = (uint8)tmp;
          TurnLightAct_DW.is_Normal_p = TurnLightAct_IN_NO_ACTIVE_CHILD;
        } else {
          TurnLightAct_DW.is_Normal_p = TurnLightAct_IN_NO_ACTIVE_CHILD;
        }

        TurnLightAct_DW.is_c3_TurnLightActLib = TurnLightAct_IN_OFF;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);
        TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

        /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
        TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

        /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
      } else if (TurnLightAct_DW.is_Normal_p == TurnLightAct_IN_OFF_d) {
        TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);
        if (TurnLightAct_DW.temporalCounter_i1_l >= 40) {
          TurnLightAct_DW.is_Normal_p = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1_l = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c = 1U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(1U);

          /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
          TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

          /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
        }
      } else {
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd_c = 1U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(1U);
        if (TurnLightAct_DW.temporalCounter_i1_l >= 40) {
          tmp = TurnLightAct_B.Counter_c + 1;
          if (TurnLightAct_B.Counter_c + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter_c = (uint8)tmp;
          TurnLightAct_DW.is_Normal_p = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_l = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);

          /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
          TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

          /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
        }
      }
      break;

     default:
      /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
      /* case IN_OFF: */
      (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);
      TurnLightAct_B.TurnLampCtrlCmd_c = 0U;
      if ((tmpRead_0 == 1) || (tmpRead_0 == 2)) {
        TurnLightAct_B.Counter_c = 0U;
        TurnLightAct_DW.is_c3_TurnLightActLib = TurnLightAct_IN_Normal;
        TurnLightAct_DW.is_Normal_p = TurnLightAct_IN_OFF_d;
        TurnLightAct_DW.temporalCounter_i1_l = 0U;
        TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

        /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
        (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);

        /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
        TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

        /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
      } else {
        if (tmpRead_0 == 3) {
          TurnLightAct_DW.is_c3_TurnLightActLib = TurnLightAct_IN_Double;
          TurnLightAct_DW.is_Double_b = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_l = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_c = 0U;

          /* Outport: '<Root>/Com_CAN_LA_St_DirectionLampR_u8_signal' */
          (void)Rte_Write_Com_CAN_LA_St_DirectionLampR_u8_signal(0U);

          /* Outputs for Function Call SubSystem: '<S5>/Function-Call Subsystem2' */
          TurnLigh_FunctionCallSubsystem2(TurnLightAct_B.TurnLampCtrlCmd_c);

          /* End of Outputs for SubSystem: '<S5>/Function-Call Subsystem2' */
        }
      }
      break;
    }
  }

  /* End of Chart: '<S5>/TurnLampActuator' */

  /* Chart: '<S4>/TurnLampActuator1' incorporates:
   *  Inport: '<Root>/TurnLapm_FR_Out_u8_Signal'
   */
  if (TurnLightAct_DW.temporalCounter_i1_h < 63U) {
    TurnLightAct_DW.temporalCounter_i1_h++;
  }

  if (TurnLightAct_DW.is_active_c1_TurnLightActLib_g == 0U) {
    TurnLightAct_DW.is_active_c1_TurnLightActLib_g = 1U;
    TurnLightAct_DW.is_c1_TurnLightActLib_k = TurnLightAct_IN_OFF;
    TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

    /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
    TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

    /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
  } else {
    (void)Rte_Read_TurnLapm_FR_Out_u8_Signal(&tmpRead_1);
    switch (TurnLightAct_DW.is_c1_TurnLightActLib_k) {
     case TurnLightAct_IN_Double:
      if (tmpRead_1 == 0) {
        TurnLightAct_DW.is_Double_l = TurnLightAct_IN_NO_ACTIVE_CHILD;
        TurnLightAct_DW.is_c1_TurnLightActLib_k = TurnLightAct_IN_OFF;
        TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

        /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
        TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

        /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
      } else if (TurnLightAct_DW.is_Double_l == TurnLightAct_IN_OFF_d) {
        TurnLightAct_B.TurnLampCtrlCmd_n = 0U;
        if (TurnLightAct_DW.temporalCounter_i1_h >= 20) {
          TurnLightAct_DW.is_Double_l = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1_h = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_n = 1U;

          /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
          TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

          /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
        }
      } else {
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd_n = 1U;
        if (TurnLightAct_DW.temporalCounter_i1_h >= 20) {
          TurnLightAct_DW.is_Double_l = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_h = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

          /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
          TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

          /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
        }
      }
      break;

     case TurnLightAct_IN_Normal:
      if (((tmpRead_1 == 2) && (TurnLightAct_B.Counter_f == 3)) || (tmpRead_1 ==
           0)) {
        if (TurnLightAct_DW.is_Normal_c == TurnLightAct_IN_ON) {
          tmp = TurnLightAct_B.Counter_f + 1;
          if (TurnLightAct_B.Counter_f + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter_f = (uint8)tmp;
          TurnLightAct_DW.is_Normal_c = TurnLightAct_IN_NO_ACTIVE_CHILD;
        } else {
          TurnLightAct_DW.is_Normal_c = TurnLightAct_IN_NO_ACTIVE_CHILD;
        }

        TurnLightAct_DW.is_c1_TurnLightActLib_k = TurnLightAct_IN_OFF;
        TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

        /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
        TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

        /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
      } else if (TurnLightAct_DW.is_Normal_c == TurnLightAct_IN_OFF_d) {
        TurnLightAct_B.TurnLampCtrlCmd_n = 0U;
        if (TurnLightAct_DW.temporalCounter_i1_h >= 40) {
          TurnLightAct_DW.is_Normal_c = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1_h = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_n = 1U;

          /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
          TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

          /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
        }
      } else {
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd_n = 1U;
        if (TurnLightAct_DW.temporalCounter_i1_h >= 40) {
          tmp = TurnLightAct_B.Counter_f + 1;
          if (TurnLightAct_B.Counter_f + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter_f = (uint8)tmp;
          TurnLightAct_DW.is_Normal_c = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_h = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

          /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
          TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

          /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
        }
      }
      break;

     default:
      /* case IN_OFF: */
      TurnLightAct_B.TurnLampCtrlCmd_n = 0U;
      if ((tmpRead_1 == 1) || (tmpRead_1 == 2)) {
        TurnLightAct_B.Counter_f = 0U;
        TurnLightAct_DW.is_c1_TurnLightActLib_k = TurnLightAct_IN_Normal;
        TurnLightAct_DW.is_Normal_c = TurnLightAct_IN_OFF_d;
        TurnLightAct_DW.temporalCounter_i1_h = 0U;
        TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

        /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
        TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

        /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
      } else {
        if (tmpRead_1 == 3) {
          TurnLightAct_DW.is_c1_TurnLightActLib_k = TurnLightAct_IN_Double;
          TurnLightAct_DW.is_Double_l = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1_h = 0U;
          TurnLightAct_B.TurnLampCtrlCmd_n = 0U;

          /* Outputs for Function Call SubSystem: '<S4>/Function-Call Subsystem1' */
          TurnLigh_FunctionCallSubsystem1(TurnLightAct_B.TurnLampCtrlCmd_n);

          /* End of Outputs for SubSystem: '<S4>/Function-Call Subsystem1' */
        }
      }
      break;
    }
  }

  /* End of Chart: '<S4>/TurnLampActuator1' */

  /* Chart: '<S6>/TurnLampActuator1' incorporates:
   *  Inport: '<Root>/TurnLapme_RR_Out_u8_Signal'
   */
  if (TurnLightAct_DW.temporalCounter_i1 < 63U) {
    TurnLightAct_DW.temporalCounter_i1++;
  }

  if (TurnLightAct_DW.is_active_c1_TurnLightActLib == 0U) {
    TurnLightAct_DW.is_active_c1_TurnLightActLib = 1U;
    TurnLightAct_DW.is_c1_TurnLightActLib = TurnLightAct_IN_OFF;
    TurnLightAct_B.TurnLampCtrlCmd = 0U;

    /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
    TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

    /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
  } else {
    (void)Rte_Read_TurnLapme_RR_Out_u8_Signal(&tmpRead_2);
    switch (TurnLightAct_DW.is_c1_TurnLightActLib) {
     case TurnLightAct_IN_Double:
      if (tmpRead_2 == 0) {
        TurnLightAct_DW.is_Double = TurnLightAct_IN_NO_ACTIVE_CHILD;
        TurnLightAct_DW.is_c1_TurnLightActLib = TurnLightAct_IN_OFF;
        TurnLightAct_B.TurnLampCtrlCmd = 0U;

        /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
        TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

        /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
      } else if (TurnLightAct_DW.is_Double == TurnLightAct_IN_OFF_d) {
        TurnLightAct_B.TurnLampCtrlCmd = 0U;
        if (TurnLightAct_DW.temporalCounter_i1 >= 20) {
          TurnLightAct_DW.is_Double = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1 = 0U;
          TurnLightAct_B.TurnLampCtrlCmd = 1U;

          /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
          TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

          /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
        }
      } else {
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd = 1U;
        if (TurnLightAct_DW.temporalCounter_i1 >= 20) {
          TurnLightAct_DW.is_Double = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1 = 0U;
          TurnLightAct_B.TurnLampCtrlCmd = 0U;

          /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
          TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

          /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
        }
      }
      break;

     case TurnLightAct_IN_Normal:
      if (((tmpRead_2 == 2) && (TurnLightAct_B.Counter == 3)) || (tmpRead_2 == 0))
      {
        if (TurnLightAct_DW.is_Normal == TurnLightAct_IN_ON) {
          tmp = TurnLightAct_B.Counter + 1;
          if (TurnLightAct_B.Counter + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter = (uint8)tmp;
          TurnLightAct_DW.is_Normal = TurnLightAct_IN_NO_ACTIVE_CHILD;
        } else {
          TurnLightAct_DW.is_Normal = TurnLightAct_IN_NO_ACTIVE_CHILD;
        }

        TurnLightAct_DW.is_c1_TurnLightActLib = TurnLightAct_IN_OFF;
        TurnLightAct_B.TurnLampCtrlCmd = 0U;

        /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
        TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

        /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
      } else if (TurnLightAct_DW.is_Normal == TurnLightAct_IN_OFF_d) {
        TurnLightAct_B.TurnLampCtrlCmd = 0U;
        if (TurnLightAct_DW.temporalCounter_i1 >= 40) {
          TurnLightAct_DW.is_Normal = TurnLightAct_IN_ON;
          TurnLightAct_DW.temporalCounter_i1 = 0U;
          TurnLightAct_B.TurnLampCtrlCmd = 1U;

          /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
          TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

          /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
        }
      } else {
        /* case IN_ON: */
        TurnLightAct_B.TurnLampCtrlCmd = 1U;
        if (TurnLightAct_DW.temporalCounter_i1 >= 40) {
          tmp = TurnLightAct_B.Counter + 1;
          if (TurnLightAct_B.Counter + 1 > 255) {
            tmp = 255;
          }

          TurnLightAct_B.Counter = (uint8)tmp;
          TurnLightAct_DW.is_Normal = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1 = 0U;
          TurnLightAct_B.TurnLampCtrlCmd = 0U;

          /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
          TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

          /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
        }
      }
      break;

     default:
      /* case IN_OFF: */
      TurnLightAct_B.TurnLampCtrlCmd = 0U;
      if ((tmpRead_2 == 1) || (tmpRead_2 == 2)) {
        TurnLightAct_B.Counter = 0U;
        TurnLightAct_DW.is_c1_TurnLightActLib = TurnLightAct_IN_Normal;
        TurnLightAct_DW.is_Normal = TurnLightAct_IN_OFF_d;
        TurnLightAct_DW.temporalCounter_i1 = 0U;
        TurnLightAct_B.TurnLampCtrlCmd = 0U;

        /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
        TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

        /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
      } else {
        if (tmpRead_2 == 3) {
          TurnLightAct_DW.is_c1_TurnLightActLib = TurnLightAct_IN_Double;
          TurnLightAct_DW.is_Double = TurnLightAct_IN_OFF_d;
          TurnLightAct_DW.temporalCounter_i1 = 0U;
          TurnLightAct_B.TurnLampCtrlCmd = 0U;

          /* Outputs for Function Call SubSystem: '<S6>/Function-Call Subsystem3' */
          TurnLigh_FunctionCallSubsystem3(TurnLightAct_B.TurnLampCtrlCmd);

          /* End of Outputs for SubSystem: '<S6>/Function-Call Subsystem3' */
        }
      }
      break;
    }
  }

  /* End of Chart: '<S6>/TurnLampActuator1' */
  /* End of Outputs for RootInportFunctionCallGenerator generated from: '<Root>/Runnable_10ms' */

  /* Outport: '<Root>/CountSignal_Left_u8_Signal' */
  (void)Rte_Write_CountSignal_Left_u8_Signal(TurnLightAct_B.Counter_e);

  /* Outport: '<Root>/CountSignal_Right_u8_Signal' */
  (void)Rte_Write_CountSignal_Right_u8_Signal(TurnLightAct_B.Counter_c);

  /* Outport: '<Root>/TurnLampCtrlCmd_FL_u8_signal' */
  (void)Rte_Write_TurnLampCtrlCmd_FL_u8_signal(TurnLightAct_B.TurnLampCtrlCmd_c2);

  /* Outport: '<Root>/TurnLampCtrlCmd_FR_u8_signa' */
  (void)Rte_Write_TurnLampCtrlCmd_FR_u8_signa(TurnLightAct_B.TurnLampCtrlCmd_n);

  /* Outport: '<Root>/TurnLampCtrlCmd_RL_u8_signal' */
  (void)Rte_Write_TurnLampCtrlCmd_RL_u8_signal(TurnLightAct_B.TurnLampCtrlCmd_c);

  /* Outport: '<Root>/TurnLampCtrlCmd_RR_u8_signal' */
  (void)Rte_Write_TurnLampCtrlCmd_RR_u8_signal(TurnLightAct_B.TurnLampCtrlCmd);
}

/* Model initialize function */
void TurnLightAct_Init(void)
{
  /* SystemInitialize for Outport: '<Root>/CountSignal_Left_u8_Signal' */
  (void)Rte_Write_CountSignal_Left_u8_Signal(TurnLightAct_B.Counter_e);

  /* SystemInitialize for Outport: '<Root>/CountSignal_Right_u8_Signal' */
  (void)Rte_Write_CountSignal_Right_u8_Signal(TurnLightAct_B.Counter_c);

  /* SystemInitialize for Outport: '<Root>/TurnLampCtrlCmd_FL_u8_signal' */
  (void)Rte_Write_TurnLampCtrlCmd_FL_u8_signal(TurnLightAct_B.TurnLampCtrlCmd_c2);

  /* SystemInitialize for Outport: '<Root>/TurnLampCtrlCmd_FR_u8_signa' */
  (void)Rte_Write_TurnLampCtrlCmd_FR_u8_signa(TurnLightAct_B.TurnLampCtrlCmd_n);

  /* SystemInitialize for Outport: '<Root>/TurnLampCtrlCmd_RL_u8_signal' */
  (void)Rte_Write_TurnLampCtrlCmd_RL_u8_signal(TurnLightAct_B.TurnLampCtrlCmd_c);

  /* SystemInitialize for Outport: '<Root>/TurnLampCtrlCmd_RR_u8_signal' */
  (void)Rte_Write_TurnLampCtrlCmd_RR_u8_signal(TurnLightAct_B.TurnLampCtrlCmd);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
