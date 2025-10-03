/**************************************************
 * 文件名:    main.c
 * 作者:      GitHub Copilot
 * 日期:      2025-10-03
 * 描述:      智能交通灯系统主程序 - 利用已实现模块
 *           调用各模块已实现的功能，简化主控制逻辑
 **************************************************/

#include "config.h"
#include "timer.h"
#include "buzzer.h"
#include "display.h"
#include "traffic_light.h"
#include "key_handler.h"
#include "extension.h"

/*-----------------------函数声明-----------------------------*/
void System_Init(void);
void Main_Loop(void);

/*-----------------------主程序实现---------------------------*/

/**
 * @brief  系统初始化
 */
void System_Init(void)
{
    // 关闭所有中断，防止初始化过程中被打断
    EA = 0;
    
    // 初始化各个模块
    Timer_Init();           // 定时器模块
    Buzzer_Init();          // 蜂鸣器模块
    Display_Init();         // 显示模块
    TrafficLight_Init();    // 交通灯模块
    
    // 如需启用扩展功能，取消下面的注释
    Key_Init();          // 按键处理模块
    // Extension_Init();    // 扩展功能模块
    
    // 开启全局中断
    EA = 1;
}

/**
 * @brief  主控制循环 - 调用各模块已实现的处理函数
 */
void Main_Loop(void)
{
    unsigned char nsTime, ewTime;  // 在函数开头声明变量
    
    while(1) {
        // 1秒定时处理 - 交通灯状态机
        if (Timer_GetFlag1s()) {
            // 调用交通灯模块的定时处理函数
            TrafficLight_TimerHandler();
            
            // 获取当前状态和时间进行显示
            nsTime = TrafficLight_GetTimeLeft(DIRECTION_NS);
            ewTime = TrafficLight_GetTimeLeft(DIRECTION_EW);
            
            // 如需启用显示，取消下面的注释
            Display_ShowTime(nsTime, ewTime);
        }
        
        // 100ms定时处理 - 闪烁和蜂鸣器
        if (Timer_GetFlag100ms()) {
            // 调用交通灯闪烁处理（需要取消注释）
            TrafficLight_BlinkHandler();
            
            // 获取剩余时间进行蜂鸣器提示
            nsTime = TrafficLight_GetTimeLeft(DIRECTION_NS);
            ewTime = TrafficLight_GetTimeLeft(DIRECTION_EW);
            
            // 最后3秒蜂鸣器提示（需要取消注释）
            if ((nsTime <= 3 && nsTime > 0) || (ewTime <= 3 && ewTime > 0)) {
                Buzzer_Beep();
            }
        }
        
        // 10ms定时处理 - 按键扫描
        if (Timer_GetFlag10ms()) {
            // 调用按键扫描处理（需要取消注释）
            Key_Scan();
            
            // 简化版本：直接处理紧急按键
            if (!KEY_EMERGENCY) {
                Delay_ms(20);  // 防抖
                if (!KEY_EMERGENCY) {
                    // 调用紧急延时功能
                    TrafficLight_EmergencyExtend();
                    
                    // 等待按键释放
                    while (!KEY_EMERGENCY) {
                        Delay_ms(10);
                    }
                }
            }
        }
        
        // 短暂延时，避免CPU占用过高
        Delay_ms(5);
    }
}

/**
 * @brief  主函数
 */
void main(void)
{
    // 系统初始化
    System_Init();
    
    // 进入主循环
    Main_Loop();
}