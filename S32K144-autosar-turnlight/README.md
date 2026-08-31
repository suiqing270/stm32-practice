# AUTOSAR TurnLight 源码学习与注释

> 本仓库是我在学习 AUTOSAR Classic Platform 时整理的个人学习版本。项目代码来自 GitHub 上的原始项目，我的主要工作是阅读源码、梳理调用链，并在部分应用层代码中添加中文注释。本仓库不代表我参与了原项目的开发。

## 仓库说明

- **学习目的**：理解 AUTOSAR CP 的分层、SW-C 业务逻辑、RTE/OS 调度、CAN 通信、UDS 诊断以及 Dio 硬件输出路径。
- **我的修改**：在 `Appl/Source/` 中的部分转向灯应用代码中增加了中文理解性注释，并整理了学习路线。
- **学习记录**：完整的阅读步骤、调用链和学习目标见 [`project_learning.txt`](project_learning.txt)。
- **验证边界**：目前只完成源码和配置文件研读，没有在本地重新生成代码、完整编译、下载到开发板或使用 CANoe 进行实车/台架验证。



## 项目概览

该项目是一个面向 NXP S32K144 的 AUTOSAR Classic 转向灯控制示例。应用层包含转向灯控制、输出仲裁和执行器控制，并通过 RTE 连接通信、诊断和硬件抽象相关模块。

| 项目 | 内容 |
| --- | --- |
| MCU/配置目标 | NXP S32K144（ARM Cortex-M4F） |
| 软件架构 | AUTOSAR Classic Platform 4.x |
| 应用层 | TurnLight / TurnLightArb / TurnLightAct |
| 配置与生成代码 | ARXML、RTE、BSW 与 MCAL 相关文件 |
| 源仓库工具链 | IAR、DaVinci Developer/Configurator、EB tresos、Simulink/Stateflow |

## AUTOSAR 分层

```text
TurnLight / TurnLightArb / TurnLightAct
                  |
                 RTE
          +-------+-------+
          |               |
 Com / PduR / CanIf    CDD / IoHwAb
 CanTp / Dcm / Dem         |
 EcuM / BswM / OS        Dio / MCAL
                          |
                       S32K144
```

- **SW-C** 负责转向灯业务状态机、输出仲裁和执行器逻辑。
- **RTE** 连接 SW-C 与其他组件/BSW，提供 `Rte_Read`、`Rte_Write` 和 `Rte_Call` 接口。
- **OS** 通过 Task/Event 调度 Runnable。
- **BSW** 承担通信、诊断、ECU 状态管理等通用服务。
- **CDD/IoHwAb** 连接上层控制请求与具体硬件访问。
- **MCAL** 封装 S32K144 外设驱动和寄存器访问。

## 主要功能

### 转向灯控制

`Appl/Source/TurnLight.c` 处理转向灯开关、危险警报、碰撞/紧急制动触发、长闪/短闪时序以及开路/短路诊断状态。

### 输出仲裁

`Appl/Source/TurnLightArb.c` 对正常转向、短闪和危险警报等多来源请求进行优先级仲裁，得到最终的四路灯输出模式。

### 执行器与硬件输出

`Appl/Source/TurnLightAct.c` 处理 FL、FR、RL、RR 四路灯状态，通过 IoHwAb/CDD 调用 `Dio_WriteChannel` 输出到 GPIO。

| 逻辑灯位 | Dio 通道 | S32K144 引脚 |
| --- | --- | --- |
| FL（左前） | `DioChannel_PTD0` | PTD0 |
| FR（右前） | `DioChannel_PTD1` | PTD1 |
| RL（左后） | `DioChannel_PTD15` | PTD15 |
| RR（右后） | `DioChannel_PTD16` | PTD16 |

## 重点调用链

### 转向灯业务链

```text
输入信号
  -> TurnLight_Runnable_10ms
  -> Rte_Read
  -> TurnLight 控制逻辑
  -> TurnLightArb 优先级仲裁
  -> Rte_Write 四路灯输出
  -> TurnLightAct
  -> IoHwAb / CDD
  -> Dio_WriteChannel
  -> PTD0 / PTD1 / PTD15 / PTD16
```

### 10 ms Runnable 调度链

```text
System Timer
  -> OS Alarm / Event
  -> OS Task
  -> Rte.c
  -> TurnLight_Runnable_10ms
  -> TurnLightArb_Runnable_10ms
  -> TurnLightAct_Runnable_10ms
```

`TurnLight_Runnable_10ms()` 不是由应用代码主动循环调用，而是由 OS 任务收到周期事件后，通过生成的 `Appl/GenData/Rte.c` 调用。

### CAN 与诊断链

```text
CAN 信号接收: Can -> CanIf -> PduR -> Com -> Rte_Read -> SW-C
CAN 信号发送: SW-C -> Rte_Write -> Com -> PduR -> CanIf -> Can
UDS 诊断请求: Can -> CanIf -> PduR -> CanTp -> PduR -> Dcm -> Dem/DID 回调
```

仓库中存在 Dcm/Dem 及相关诊断配置，但这只能说明配置和代码存在，不代表我已完成实际 UDS 通信测试。

## 项目结构

```text
.
├── Appl/
│   ├── Source/       # SW-C、CDD 及辅助代码（中文学习注释主要位于此处）
│   └── GenData/      # RTE 等生成代码
├── BSW/                  # AUTOSAR 基础软件模块
├── MCAL/                 # NXP S32K1xx MCAL 驱动包
├── Config/               # ECUC、SW-C、通信和诊断 ARXML 配置
├── Device/               # 启动、链接与芯片相关文件
├── Inputfiles/           # DBC 等输入文件
└── project_learning.txt  # 个人学习路线与记录
```

## 如何阅读

1. 先阅读 `project_learning.txt`，了解项目边界和三条学习主线。
2. 按 `TurnLight.c` -> `TurnLightArb.c` -> `TurnLightAct.c` -> `Cdd_IoHwAb.c` 追踪业务输出。
3. 在 `Appl/GenData/Rte.c` 中查看 10 ms Runnable 的调度映射。
4. 结合 `Config/ECUC/` 和 `Config/System/` 追踪 CAN、Dio 与诊断配置。
5. 阅读时区分“源码确认”“配置确认”和“尚未验证”，避免把静态分析结论当成硬件实测结论。

## 编译与工具链说明

代码和配置显示原项目使用过 IAR、Vector DaVinci、EB tresos 和 Simulink 相关工具链。但当前仓库中没有 IAR `.eww/.ewp` 工程文件和 Simulink `.slx` 模型，因此不提供未经验证的“开箱即编译”步骤。

## 版权与使用提醒

本仓库仅用于个人学习和源码阅读记录。仓库根目录未发现统一的开源许可证，且部分 BSW、RTE 和 MCAL 文件包含 Vector Informatik、NXP/Freescale 等权利人的版权或专有软件声明。

在复制、修改、分发或公开上传任何代码前，请先核对原项目的授权范围及各第三方组件的许可条款。我添加的学习注释不改变原代码的版权归属，也不构成对原代码的再授权。

## 致谢

感谢原项目作者与相关工具/软件供应商提供的项目代码和学习素材。公开发布本学习版本前，应补充原项目的准确链接、作者和许可信息。
