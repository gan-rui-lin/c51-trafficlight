/**************************************************
 * 文件名:    buzzer.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      蜂鸣器模块实现文件（精简版本）
 *           只保留基本的蜂鸣器功能
 **************************************************/

#include "buzzer.h"
#include "timer.h"

/*-----------------------全局变量定义-------------------------*/
static unsigned char buzzerEnabled = 1;    // 蜂鸣器使能标志

/*-----------------------函数实现-----------------------------*/

/**
 * @brief  蜂鸣器模块初始化
 */
void Buzzer_Init(void)
{
    // 初始化蜂鸣器端口
    BUZZER_PIN = 0;     // 关闭蜂鸣器
    buzzerEnabled = 1;  // 默认启用蜂鸣器
}

/**
 * @brief  蜂鸣器响一声
 */
void Buzzer_Beep(void)
{
    if (buzzerEnabled) {
        BUZZER_PIN = 1;   // 开启蜂鸣器
        Delay_ms(100);    // 延时100ms
        BUZZER_PIN = 0;   // 关闭蜂鸣器
    }
}

