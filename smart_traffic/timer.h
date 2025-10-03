/**************************************************
 * 文件名:    timer.h
 * 作者:
 * 日期:      2025-10-02
 * 描述:      定时器模块头文件
 *           提供系统定时功能，包括主控制定时和显示刷新定时
 **************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include "config.h"

/*-----------------------定时器类型定义-------------------------*/
typedef enum {
  TIMER_1MS = 0,   // 1毫秒定时器（显示刷新用）
  TIMER_10MS = 1,  // 10毫秒定时器（按键扫描用）
  TIMER_100MS = 2, // 100毫秒定时器（闪烁控制用）
  TIMER_1S = 3     // 1秒定时器（主控制用）
} TimerType_t;

/*-----------------------全局变量声明-------------------------*/
extern volatile unsigned char timer1msFlag;
extern volatile unsigned char timer10msFlag;
extern volatile unsigned char timer100msFlag;
extern volatile unsigned char timer1sFlag;

/*-----------------------函数声明-----------------------------*/

/**
 * @brief  定时器模块初始化
 * @param  无
 * @retval 无
 */
void Timer_Init(void);

/**
 * @brief  定时器0初始化（1ms定时）
 * @param  无
 * @retval 无
 */
void Timer0_Init(void);

/**
 * @brief  定时器1初始化（预留）
 * @param  无
 * @retval 无
 */
void Timer1_Init(void);

/**
 * @brief  启动定时器
 * @param  timer: 定时器类型
 * @retval 无
 */
void Timer_Start(TimerType_t timer);

/**
 * @brief  停止定时器
 * @param  timer: 定时器类型
 * @retval 无
 */
void Timer_Stop(TimerType_t timer);

/**
 * @brief  获取定时器状态
 * @param  timer: 定时器类型
 * @retval 定时器标志状态
 */
unsigned char Timer_GetFlag(TimerType_t timer);

/**
 * @brief  清除定时器标志
 * @param  timer: 定时器类型
 * @retval 无
 */
void Timer_ClearFlag(TimerType_t timer);

/**
 * @brief  延时函数（毫秒级）
 * @param  ms: 延时毫秒数
 * @retval 无
 */
void Delay_ms(unsigned int ms);

/**
 * @brief  延时函数（微秒级）
 * @param  us: 延时微秒数
 * @retval 无
 */
void Delay_us(unsigned int us);

/**
 * @brief  获取1秒定时标志
 * @retval 1秒标志状态
 */
unsigned char Timer_GetFlag1s(void);

/**
 * @brief  获取100毫秒定时标志
 * @retval 100ms标志状态
 */
unsigned char Timer_GetFlag100ms(void);

/**
 * @brief  获取10毫秒定时标志
 * @retval 10ms标志状态
 */
unsigned char Timer_GetFlag10ms(void);

#endif /* __TIMER_H__ */