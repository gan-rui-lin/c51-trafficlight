# 智能交通灯系统 - 模块化版本

## 📋 概述

这是基于可工作的单文件版本改造的模块化智能交通灯系统。保持了原有的稳定性，同时提供了更好的代码组织结构。

## 🗂️ 文件结构

```
smart_traffic/
├── main.c                      # 单文件版本（已验证可工作）
├── main_modular.c              # 模块化版本（新）
├── config.h                    # 系统配置文件（精简版）
├── traffic_light_simple.h      # 交通灯模块头文件（精简版）
└── README_MODULAR.md           # 本文档
```

## ⚙️ 核心功能

### 1. 定时中断系统
- **Timer0中断**: 每2ms触发一次
- **精确1秒定时**: 500次中断 = 1秒
- **中断保护**: 进入/退出中断时关闭/开启中断（重要！）

### 2. 交通灯状态机
- **状态0**: 南北绿灯（25秒），东西红灯
- **状态1**: 南北黄灯（3秒），东西红灯  
- **状态2**: 南北红灯，东西绿灯（25秒）
- **状态3**: 南北红灯，东西黄灯（3秒）
- **循环切换**: 0→1→2→3→0

### 3. 闪烁提醒
- **触发条件**: 剩余时间 ≤ 3秒
- **闪烁频率**: 约500ms切换一次
- **闪烁对象**: 当前亮着的灯

### 4. 调试指示
- **P3.6 (DEBUG_1S_PIN)**: 心跳指示，每200ms切换
- **P3.7 (DEBUG_STATE_PIN)**: 状态切换指示

## 🔌 硬件连接

### 交通灯连接
| 引脚  | 功能       | 说明       |
|-------|-----------|-----------|
| P2.0  | NS_RED    | 南北红灯   |
| P2.1  | NS_YELLOW | 南北黄灯   |
| P2.2  | NS_GREEN  | 南北绿灯   |
| P2.3  | EW_RED    | 东西红灯   |
| P2.4  | EW_YELLOW | 东西黄灯   |
| P2.5  | EW_GREEN  | 东西绿灯   |

### 调试LED连接
| 引脚  | 功能            | 说明           |
|-------|----------------|---------------|
| P3.6  | DEBUG_1S_PIN   | 心跳指示LED    |
| P3.7  | DEBUG_STATE_PIN| 状态切换指示LED|

## 💻 代码结构

### config.h
```c
// 寄存器定义
sfr P2, P3, TMOD, TH0, TL0, TCON, IE, IP

// 引脚定义
sbit NS_RED_PIN, NS_YELLOW_PIN, NS_GREEN_PIN
sbit EW_RED_PIN, EW_YELLOW_PIN, EW_GREEN_PIN  
sbit DEBUG_1S_PIN, DEBUG_STATE_PIN

// 参数定义
#define GREEN_LIGHT_TIME    25
#define YELLOW_LIGHT_TIME   3
#define FLASH_START_TIME    3
```

### traffic_light_simple.h
```c
// 函数声明
void SetTrafficLights(unsigned char state);
void HandleTrafficLightFlash(void);
void SwitchToNextState(void);
void Timer0_Init(void);
void System_Init(void);

// 外部变量
extern volatile unsigned char currentState;
extern volatile unsigned char timeLeft;
extern volatile unsigned int timer0Count;
```

### main_modular.c
```c
// 全局变量定义
volatile unsigned char currentState;
volatile unsigned char timeLeft;
volatile unsigned int timer0Count;

// 硬件控制函数
SetTrafficLights()
HandleTrafficLightFlash()
SwitchToNextState()

// 定时器初始化
Timer0_Init()

// 中断服务函数
Timer0_ISR() interrupt 1

// 系统初始化
System_Init()

// 主函数
main()
```

## 🔧 关键技术点

### 1. 中断保护机制
```c
void Timer0_ISR(void) interrupt 1
{
    // 重新装载定时器初值
    TH0 = 0xA9;
    TL0 = 0x6A;
    
    // ⚠️ 关键：关中断保护
    TR0 = 0;  // 暂停计时器
    EA = 0;   // 全局关中断
    
    // ... 中断处理代码 ...
    
    // ⚠️ 关键：恢复中断
    EA = 1;   // 再开中断
    TR0 = 1;  // 恢复定时器
}
```

### 2. Timer0定时计算
```
晶振频率: 11.0592MHz
定时时间: 2ms
机器周期: 22118个
65536 - 22118 = 43418 = 0xA96A
TH0 = 0xA9, TL0 = 0x6A
```

### 3. 1秒定时实现
```c
if (timer0Count >= 500) {  // 500 * 2ms = 1000ms = 1秒
    timer0Count = 0;
    if (timeLeft > 0) {
        timeLeft--;
    }
    if (timeLeft == 0) {
        SwitchToNextState();
    }
}
```

## 📝 编译说明

### 使用Keil uVision
1. 打开项目文件`samrt_traffic_light_system.uvproj`
2. 将`main_modular.c`添加到项目中
3. 移除或排除`main.c`（避免重复main函数）
4. 编译项目

### 编译配置
- **目标**: AT89C52 或 80C51兼容芯片
- **晶振频率**: 11.0592MHz
- **优化级别**: Level 0 (无优化，便于调试)

## 🚀 使用方法

### 1. 硬件准备
- 连接交通灯LED到P2.0-P2.5
- 连接调试LED到P3.6-P3.7
- 确保晶振频率为11.0592MHz

### 2. 烧录程序
- 编译生成HEX文件
- 使用烧录器烧录到单片机
- 或在Proteus ISIS中仿真

### 3. 观察运行
- **硬件自检**: 上电后所有灯亮1秒
- **正常运行**: 交通灯按状态机切换
- **心跳指示**: P3.6快速闪烁
- **闪烁提醒**: 最后3秒开始闪烁

## 🔍 故障排查

### 问题1: 交通灯不切换
- 检查P3.6是否闪烁（验证程序是否运行）
- 检查晶振频率是否正确
- 检查中断是否使能

### 问题2: 定时不准确
- 验证晶振频率为11.0592MHz
- 检查TIMER0_RELOAD值是否正确
- 检查中断保护代码是否正确

### 问题3: LED不亮
- 检查硬件连接
- 验证LED极性
- 检查电阻值（建议240Ω-1kΩ）

## 📌 未来扩展

在主循环中可以添加：
1. **按键处理**: 紧急延时、手动控制
2. **数码管显示**: 倒计时显示
3. **蜂鸣器**: 声音提醒
4. **通信接口**: 远程监控
5. **传感器**: 温度、湿度监测

## 📄 许可证

本项目基于MIT许可证开源。

## 👥 贡献

欢迎提交Issue和Pull Request！

---

**版本**: v1.0  
**更新日期**: 2025-10-07  
**状态**: ✅ 已验证可工作
