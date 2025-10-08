/**************************************************
 * 文件名:    config.h
 * 作者:      GitHub Copilot
 * 日期:      2025-10-07
 * 描述:      智能交通灯系统配置文件 - 精简版
 *           只包含核心功能的配置，基于可工作的单文件版本
 **************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <reg52.h>
/*=======================硬件配置宏定义=======================*/

// 单片机型号说明：80C51兼容芯片
// 主要指标：工作电压5V，晶振频率12MHz

/*==============================================
 *                引脚定义
 *==============================================*/
// 调试LED引脚定义  
sbit DEBUG_1S_PIN    = P3^6;  // 1秒指示灯（心跳）
sbit DEBUG_STATE_PIN = P3^7;  // 状态指示灯

// 南北方向交通灯引脚定义
sbit NS_RED_PIN      = P2^0;  // 南北红灯
sbit NS_YELLOW_PIN   = P2^1;  // 南北黄灯  
sbit NS_GREEN_PIN    = P2^2;  // 南北绿灯

// 东西方向交通灯引脚定义
sbit EW_RED_PIN      = P2^3;  // 东西红灯
sbit EW_YELLOW_PIN   = P2^4;  // 东西黄灯
sbit EW_GREEN_PIN    = P2^5;  // 东西绿灯

/*==============================================
 *                系统参数定义
 *==============================================*/
// 交通灯状态定义
#define STATE_NS_GREEN_EW_RED     0  // 南北绿灯，东西红灯
#define STATE_NS_YELLOW_EW_RED    1  // 南北黄灯，东西红灯  
#define STATE_NS_RED_EW_GREEN     2  // 南北红灯，东西绿灯
#define STATE_NS_RED_EW_YELLOW    3  // 南北红灯，东西黄灯

// 时间配置（单位：秒）
#define GREEN_LIGHT_TIME    3    // 绿灯时间
#define YELLOW_LIGHT_TIME   3     // 黄灯时间
#define FLASH_START_TIME    3     // 开始闪烁的剩余时间

// Timer0配置（2ms中断 @ 11.0592MHz）
#define TIMER0_RELOAD_H     0xA9  // 定时器0重装值高8位
#define TIMER0_RELOAD_L     0x6A  // 定时器0重装值低8位

/*-----------------------显示器硬件配置-----------------------*/
// 数码管控制端口
#define DISPLAY_DATA_PORT P1 // 数码管段码数据端口 (a-g, dp)

// ⚠️ 当前配置：共阳极数码管（段码已在代码中取反）
// 如果改用共阴极数码管，需要修改display.c中的段码表

// 2-4译码器控制信号（74HC139 或 74LS139）
sbit DISPLAY_SEL_A = P2 ^ 6; // 译码器地址输入A (低位)
sbit DISPLAY_SEL_B = P2 ^ 7; // 译码器地址输入B (高位)

// 译码器使能控制（如果需要软件控制，否则硬件接地）
// sbit DECODER_ENABLE = P3 ^ 0; // 可选的译码器使能控制（低电平有效）

/*
 * 2-4译码器真值表 (74HC139)：
 * B A | Y0 Y1 Y2 Y3 | 选中的数码管位
 * ----+-------------+--------------
 * 0 0 |  0  1  1  1 | 第0位 (南北十位)
 * 0 1 |  1  0  1  1 | 第1位 (南北个位)
 * 1 0 |  1  1  0  1 | 第2位 (东西十位)
 * 1 1 |  1  1  1  0 | 第3位 (东西个位)
 * 
 * 注：Y0-Y3输出为低电平有效(选中时为0)
 * 
 * 74HC139是双2-4译码器，包含两组独立的译码器：
 * - 1Y组：A1(pin2), B1(pin3), E1(pin1), 输出1Y0-1Y3(pin4-7)
 * - 2Y组：A2(pin14), B2(pin13), E2(pin15), 输出2Y0-2Y3(pin9-12)
 * 
 * 本设计只使用其中一组（推荐使用1Y组）
 */

/*-----------------------按键硬件配置-------------------------*/
// 操作员按键端口定义
sbit KEY_SET_MODE = P3 ^ 2;  // 模式设置键（外部中断0）
sbit KEY_CONFIRM = P3 ^ 3;   // 确认键
sbit KEY_UP = P0 ^ 0;        // 增加键
sbit KEY_DOWN = P0 ^ 1;      // 减少键
sbit KEY_EMERGENCY = P0 ^ 2; // 紧急延时键

/*-----------------------蜂鸣器配置---------------------------*/
sbit BUZZER_PIN = P0 ^ 3; // 蜂鸣器控制端口

/*-----------------------扩展接口配置-------------------------*/
// 预留蓝牙模块接口
sbit BLUETOOTH_RX = P3 ^ 4; // 蓝牙接收端口
sbit BLUETOOTH_TX = P3 ^ 5; // 蓝牙发送端口

// // 预留WiFi/网络模块接口
// sbit WIFI_CS = P1 ^ 4;  // WiFi片选
// sbit WIFI_RST = P1 ^ 5; // WiFi复位

// // DS18B20温度传感器接口
// sbit DS18B20_DQ = P1 ^ 6; // DS18B20数据线

// // 风扇控制接口
// sbit FAN_CONTROL = P1 ^ 7; // 风扇控制端口

// 红外遥控接口
sbit IR_RECEIVER = P3 ^ 6;    // 红外接收端口
sbit IR_RECEIVE_PIN = P3 ^ 6; // 红外接收端口（别名）

// 扩展模块电源控制
// sbit EXT_POWER_PIN = P1 ^ 3; // 扩展模块电源控制

// 预留传感器接口
// #define SENSOR_DATA P1 ^ 6   // 传感器数据端口
// #define SENSOR_ENABLE P1 ^ 7 // 传感器使能端口

/*=======================系统参数配置=======================*/

/*-----------------------交通灯时间配置-----------------------*/
// 默认时间配置（单位：秒）
#define DEFAULT_GREEN_TIME 3 // 默认绿灯时间
#define DEFAULT_YELLOW_TIME 3 // 默认黄灯时间
#define DEFAULT_RED_TIME 6   // 默认红灯时间（绿灯+黄灯）

// 时间限制配置
#define MIN_LIGHT_TIME 5        // 最小灯时间
#define MAX_LIGHT_TIME 99       // 最大灯时间
#define EMERGENCY_EXTEND_TIME 5 // 紧急延时时间

/*-----------------------显示和提示配置-----------------------*/
#define BLINK_THRESHOLD 3  // 开始闪烁的剩余时间
#define BUZZER_THRESHOLD 5 // 开始蜂鸣器提示的剩余时间
#define BLINK_INTERVAL 500 // 闪烁间隔（毫秒）


/*-----------------------系统状态定义-------------------------*/
// 交通灯状态
typedef enum { LIGHT_RED = 0, LIGHT_YELLOW = 1, LIGHT_GREEN = 2 } LightState_t;

// 系统工作模式
typedef enum {
  MODE_NORMAL = 0,    // 正常运行模式
  MODE_SETTING = 1,   // 参数设置模式
  MODE_EMERGENCY = 2, // 紧急模式
  MODE_MANUAL = 3     // 手动控制模式
} SystemMode_t;

// 交通方向定义
typedef enum {
  DIRECTION_NS = 0, // 南北方向
  DIRECTION_EW = 1  // 东西方向
} Direction_t;

#endif // __CONFIG_H__