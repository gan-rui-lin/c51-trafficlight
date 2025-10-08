/**************************************************
 * 文件名:    timer.h
 * 作者:      
 * 日期:      2025-10-08
 * 描述:      定时器模块头文件
 *           定时器和延时函数声明
 **************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

/**
 * @brief  毫秒级延时函数
 * @param  ms: 延时时间（毫秒）
 * @retval 无
 */
void Delay_ms(unsigned int ms);

/**
 * @brief  1秒延时函数
 * @param  无
 * @retval 无
 */
void Delay_1s(void);

/**
 * @brief  微秒级延时函数
 * @param  us: 延时时间（微秒）
 * @retval 无
 */
void Delay_us(unsigned int us);

#endif /* __TIMER_H__ */
