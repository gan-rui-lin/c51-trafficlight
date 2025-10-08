/**************************************************
 * 文件名:    traffic_light.h
 * 作者:
 * 日期:      2025-10-07
 * 描述:      交通灯模块头文件 - 精简版
 *           包含交通灯控制的基本函数声明
 **************************************************/

#ifndef __TRAFFIC_LIGHT_H__
#define __TRAFFIC_LIGHT_H__

#include "config.h"

/*==============================================
 *                函数声明
 *==============================================*/

/**
 * @brief  设置交通灯状态
 * @param  state: 交通灯状态(0-3)
 * @retval 无
 */
void SetTrafficLights(unsigned char state);

/**
 * @brief  处理交通灯闪烁逻辑
 * @param  无
 * @retval 无
 */
void HandleTrafficLightFlash(void);

/**
 * @brief  切换到下一个交通灯状态
 * @param  无
 * @retval 无
 */
void SwitchToNextState(void);

/**
 * @brief  Timer0初始化 - 配置为2ms中断
 * @param  无
 * @retval 无
 */
void Timer0_Init(void);

/**
 * @brief  系统初始化
 * @param  无
 * @retval 无
 */
void System_Init(void);

/*==============================================
 *                外部变量声明
 *==============================================*/
extern volatile unsigned char currentState; // 当前交通灯状态
extern volatile unsigned char timeLeft;     // 当前状态剩余时间
extern volatile unsigned char isFlashing;   // 闪烁标志
extern volatile unsigned int timer0Count;   // Timer0中断计数器
extern volatile unsigned int flashCount;    // 闪烁计数器
extern unsigned char stateTimeTable[4];     // 状态时间配置表

/*=======================新增：设置模式支持=======================*/
// 设置模式标志：1=正在设置（暂停倒计时），0=正常
extern volatile unsigned char g_isSettingMode;
// 当前选择的颜色：0=红,1=黄,2=绿 （循环按键切换）
extern volatile unsigned char g_selectedColor;
// 可调节的三个颜色时间（秒），红灯时间=对向绿+黄，保持一致
extern volatile unsigned char g_time_red;
extern volatile unsigned char g_time_yellow;
extern volatile unsigned char g_time_green;

/**
 * @brief  根据当前颜色时间刷新状态时间表 stateTimeTable
 */
void UpdateStateTimeTable(void);

/**
 * @brief  设置模式下点亮对应颜色的灯作为指示（两个方向同色灯亮）
 */
void ShowSettingColorLights(void);

#endif /* __TRAFFIC_LIGHT_H__ */
