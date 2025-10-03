/**************************************************
 * 文件名:    key_handler.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      按键处理模块实现文件（精简版本）
 *           只保留紧急按键功能
 **************************************************/

#include "key_handler.h"
#include "traffic_light.h"
#include "timer.h"

/*-----------------------函数实现-----------------------------*/

/**
 * @brief  按键模块初始化
 */
void Key_Init(void)
{
    // 设置按键端口为输入模式
    KEY_EMERGENCY = 1;
}

/**
 * @brief  按键扫描处理
 */
void Key_Scan(void)
{
    // 只处理紧急按键
    if (!KEY_EMERGENCY) {
        Delay_ms(20);  // 消抖
        if (!KEY_EMERGENCY) {
            // 紧急延时处理
            TrafficLight_EmergencyExtend();
            
            // 等待按键释放
            while (!KEY_EMERGENCY) {
                Delay_ms(10);
            }
        }
    }
}