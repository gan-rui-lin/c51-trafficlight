/**************************************************
 * 文件名:    main_step1_safe.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      安全测试版本1 - 不使用Delay_ms，避免Timer冲突
 **************************************************/

#include "config.h"
#include "timer.h"  // 包含timer模块
#include <intrins.h>  // 添加这个头文件以支持_nop_()函数

/*-----------------------本地延时函数-------------------------*/
void Local_Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 113; j++) {
            _nop_();
        }
    }
}

/*-----------------------函数声明-------------------------*/
void Step1_Safe_System_Init(void);
void Step1_Safe_Main_Loop(void);

/**
 * @brief  安全系统初始化
 */
void Step1_Safe_System_Init(void)
{
    // 关闭所有中断
    EA = 0;
    
    // 初始化调试引脚
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    
    // 硬件自检（使用本地延时函数，避免Timer冲突）
    NS_RED_PIN = 1; NS_YELLOW_PIN = 1; NS_GREEN_PIN = 1;
    EW_RED_PIN = 1; EW_YELLOW_PIN = 1; EW_GREEN_PIN = 1;
    DEBUG_1S_PIN = 1; DEBUG_STATE_PIN = 1;
    Local_Delay_ms(500);
    
    NS_RED_PIN = 0; NS_YELLOW_PIN = 0; NS_GREEN_PIN = 0;
    EW_RED_PIN = 0; EW_YELLOW_PIN = 0; EW_GREEN_PIN = 0;
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    Local_Delay_ms(500);
    
    // 设置初始状态
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    
    // 测试Timer模块初始化
    Timer_Init();
    
    // 注意：Timer0_Init中会设置EA=1，所以这里不需要再设置
}

/**
 * @brief  安全主循环
 */
void Step1_Safe_Main_Loop(void)
{
    unsigned int testCounter = 0;
    
    while(1) {
        testCounter++;
        if (testCounter >= 50000) {
            testCounter = 0;
            DEBUG_1S_PIN = !DEBUG_1S_PIN;  // 程序运行指示
        }
        
        // 测试Timer_GetFlag1s函数
        if (Timer_GetFlag1s()) {
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;  // 中断工作指示
            
            // 简单交通灯切换
            if (NS_GREEN_PIN) {
                NS_GREEN_PIN = 0; NS_RED_PIN = 1;
                EW_RED_PIN = 0; EW_GREEN_PIN = 1;
            } else {
                EW_GREEN_PIN = 0; EW_RED_PIN = 1;
                NS_RED_PIN = 0; NS_GREEN_PIN = 1;
            }
        }
    }
}

/**
 * @brief  主函数
 */
void main(void)
{
    Step1_Safe_System_Init();
    Step1_Safe_Main_Loop();
}