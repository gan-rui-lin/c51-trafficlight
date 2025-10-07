/**************************************************
 * 文件名:    main_step1.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      逐步测试版本1 - 只添加Timer模块
 **************************************************/

#include "config.h"
#include "timer.h"  // 只包含timer模块

/*-----------------------函数声明-------------------------*/
void Step1_System_Init(void);
void Step1_Main_Loop(void);

/**
 * @brief  步骤1系统初始化 - 只初始化Timer模块
 */
void Step1_System_Init(void)
{
    // 关闭所有中断
    EA = 0;
    
    // 初始化调试引脚
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    
    // 先初始化Timer模块，避免Delay_ms调用问题
    Timer_Init();
    
    // 硬件自检（现在可以安全使用Delay_ms）
    NS_RED_PIN = 1; NS_YELLOW_PIN = 1; NS_GREEN_PIN = 1;
    EW_RED_PIN = 1; EW_YELLOW_PIN = 1; EW_GREEN_PIN = 1;
    DEBUG_1S_PIN = 1; DEBUG_STATE_PIN = 1;
    Delay_ms(500);  // 现在Timer已初始化，可以安全使用
    
    NS_RED_PIN = 0; NS_YELLOW_PIN = 0; NS_GREEN_PIN = 0;
    EW_RED_PIN = 0; EW_YELLOW_PIN = 0; EW_GREEN_PIN = 0;
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    Delay_ms(500);
    
    // 设置初始状态
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    
    // Timer已经初始化了，这里就不需要再调用
    // Timer_Init();  // 已经在上面调用了
    
    // EA在Timer0_Init中已经设置为1了，这里不需要重复设置
    // EA = 1;
}

/**
 * @brief  步骤1主循环
 */
void Step1_Main_Loop(void)
{
    unsigned int testCounter = 0;
    
    while(1) {
        testCounter++;
        if (testCounter >= 50000) {
            testCounter = 0;
            DEBUG_1S_PIN = !DEBUG_1S_PIN;  // 程序运行指示
        }
        
        // 使用Timer模块的标志检查函数
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
    Step1_System_Init();
    Step1_Main_Loop();
}