/*********************************************void Timer0_Init(void)
{
    // 设置Timer0为模式1（16位定时器）
    TMOD &= 0xF0;  // 清除Timer0控制位
    TMOD |= 0x01;  // 设置Timer0为模式1
    
    // 计算定时器初值（1ms @ 11.0592MHz）
    // 11.0592MHz: 1ms = 11059.2个时钟周期，取整数11059
    // 65536 - 11059 = 54477 = 0xD4ED
    // 但为了更好的精度，使用常用的0xFC66配置
    TH0 = 0xFC;    // 霒0位
    TL0 = 0x66;    // 低8位 (65536-1000=64536=0xFC66)
    
    // 启用Timer0中断
    ET0 = 1;
    
    // 启动Timer0
    TR0 = 1;
} timer.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      定时器模块实现文件（精简版本）
 *           只保留核心定时功能
 **************************************************/

#include "timer.h"
#include <intrins.h>

/*-----------------------全局变量定义-------------------------*/
volatile unsigned char timer1msFlag = 0;   // 1ms定时标志
volatile unsigned char timer10msFlag = 0;  // 10ms定时标志
volatile unsigned char timer100msFlag = 0; // 100ms定时标志
volatile unsigned char timer1sFlag = 0;    // 1s定时标志

// 内部计数器
static volatile unsigned char counter10ms = 0;
static volatile unsigned char counter100ms = 0;
static volatile unsigned int counter1s = 0;

/*-----------------------函数实现-----------------------------*/

/**
 * @brief  定时器模块初始化
 */
void Timer_Init(void)
{
    Timer0_Init();
    
    // 初始化标志位
    timer1msFlag = 0;
    timer10msFlag = 0;
    timer100msFlag = 0;
    timer1sFlag = 0;
    
    // 初始化计数器
    counter10ms = 0;
    counter100ms = 0;
    counter1s = 0;
}

/**
 * @brief  Timer0初始化（1ms定时）
 */
void Timer0_Init(void)
{
    // 设置Timer0为模式1（16位定时器）
    TMOD &= 0xF0;  // 清除Timer0控制位
    TMOD |= 0x01;  // 设置Timer0为模式1
    
    // 计算定时器初值（1ms @ 11.0592MHz）
    TH0 = 0xFC;    // 高8位
    TL0 = 0x66;    // 低8位（(65536-1000) = 64536 = 0xFC66）
    
    // 中断配置
    TF0 = 0;       // 清除Timer0溢出标志
    ET0 = 1;       // 启用Timer0中断
    PT0 = 1;       // 设置Timer0为高优先级中断
    EA = 1;        // 启用全局中断
    
    // 启动Timer0
    TR0 = 1;
}

/**
 * @brief  Timer0中断服务函数
 */
void Timer0_ISR(void) interrupt 1
{
    // 重新装载定时器初值（11.0592MHz配置）
    TH0 = 0xFC;
    TL0 = 0x66;
    
    // 设置1ms标志
    timer1msFlag = 1;
    
    // 处理10ms计数
    counter10ms++;
    if (counter10ms >= 10) {
        counter10ms = 0;
        timer10msFlag = 1;
    }
    
    // 处理100ms计数
    counter100ms++;
    if (counter100ms >= 100) {
        counter100ms = 0;
        timer100msFlag = 1;
    }
    
    // 处理1s计数
    counter1s++;
    if (counter1s >= 1000) {
        counter1s = 0;
        timer1sFlag = 1;
    }
}

/**
 * @brief  毫秒延时函数
 */
void Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 113; j++) {  // 约1ms延时 @ 11.0592MHz
            _nop_();
        }
    }
}

/**
 * @brief  微秒延时函数
 */
void Delay_us(unsigned int us)
{
    unsigned int i;
    for(i = 0; i < us; i++) {
        _nop_();  // 约1us延时（需根据晶振频率调整）
    }
}

/**
 * @brief  获取1秒定时标志
 */
unsigned char Timer_GetFlag1s(void)
{
    if (timer1sFlag) {
        timer1sFlag = 0;  // 清除标志
        return 1;
    }
    return 0;
}

/**
 * @brief  获取100毫秒定时标志
 */
unsigned char Timer_GetFlag100ms(void)
{
    if (timer100msFlag) {
        timer100msFlag = 0;  // 清除标志
        return 1;
    }
    return 0;
}

/**
 * @brief  获取10毫秒定时标志
 */
unsigned char Timer_GetFlag10ms(void)
{
    if (timer10msFlag) {
        timer10msFlag = 0;  // 清除标志
        return 1;
    }
    return 0;
}