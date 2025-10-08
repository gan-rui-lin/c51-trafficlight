/**************************************************
 * 文件名:    timer.c
 * 作者:      
 * 日期:      2025-10-08
 * 描述:      定时器模块实现文件
 *           提供系统延时函数
 **************************************************/

#include "config.h"

/**
 * @brief  毫秒级延时函数
 * @param  ms: 延时时间（毫秒）
 * @retval 无
 * @note   基于11.0592MHz晶振，使用软件延时
 */
void Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        // 每次循环约1ms (基于11.0592MHz晶振)
        for(j = 0; j < 123; j++);
    }
}

/**
 * @brief  1秒延时函数
 * @param  无
 * @retval 无
 */
void Delay_1s(void)
{
    Delay_ms(1000);
}

/**
 * @brief  微秒级延时函数
 * @param  us: 延时时间（微秒）
 * @retval 无
 * @note   基于11.0592MHz晶振，使用软件延时
 */
void Delay_us(unsigned int us)
{
    while(us--);
}
