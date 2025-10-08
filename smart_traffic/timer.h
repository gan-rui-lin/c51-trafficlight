/**************************************************
 * 文件名:    timer.h
 * 作者:      GitHub Copilot
 * 日期:      2025-10-08
 * 描述:      定时器和延时函数模块头文件
 *           提供标准延时函数和时间测量功能
 **************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include "config.h"

/*==============================================
 *                函数声明
 *==============================================*/

/**
 * @brief  延时指定毫秒数（使用Timer1实现精确延时）
 * @param  ms: 延时时间，单位毫秒（建议范围: 1-65535ms）
 * @retval 无
 * @note   基于11.0592MHz晶振，使用Timer1模式1
 */
void Delay_ms(unsigned int ms);

/**
 * @brief  延时指定微秒数（使用软件延时）
 * @param  us: 延时时间，单位微秒（建议范围: 1-65535us）
 * @retval 无
 * @note   基于11.0592MHz晶振，软件延时，精度约±5%
 */
void Delay_us(unsigned int us);

/**
 * @brief  延时指定秒数（使用Delay_ms实现）
 * @param  sec: 延时时间，单位秒（建议范围: 1-255秒）
 * @retval 无
 */
void Delay_s(unsigned char sec);

/**
 * @brief  获取系统运行时间（秒）
 * @param  无
 * @retval 系统运行秒数（会在65535秒后溢出重置）
 * @note   需要先调用Timer0_Init()启动定时器
 */
unsigned int Get_SystemTime_s(void);

/**
 * @brief  重置系统运行时间计数器
 * @param  无
 * @retval 无
 */
void Reset_SystemTime(void);

/**
 * @brief  测试延时精度（在数码管上显示倒计时）
 * @param  testSeconds: 测试秒数（0-9）
 * @retval 无
 * @note   通过数码管显示倒计时来验证延时是否准确
 */
void Test_DelayAccuracy(unsigned char testSeconds);

/*==============================================
 *                外部变量声明
 *==============================================*/
extern volatile unsigned int systemTime_s;  // 系统运行时间（秒）

#endif /* __TIMER_H__ */
