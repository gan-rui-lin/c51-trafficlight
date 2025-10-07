/**************************************************
 * 文件名:    main_bypass_timer.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      绕过Timer模块测试 - 完全不调用Timer_Init()
 **************************************************/

#include "config.h"
#include <intrins.h>

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
void Bypass_System_Init(void);
void Bypass_Main_Loop(void);

/**
 * @brief  绕过Timer模块的系统初始化
 */
void Bypass_System_Init(void)
{
    // 关闭所有中断
    EA = 0;
    
    // 初始化调试引脚
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    
    // 硬件自检
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
    
    // 完全不调用Timer_Init()！
    // Timer_Init();  // 注释掉，测试是否这里有问题
    
    // 保持中断关闭状态进行测试
    // EA = 1;
}

/**
 * @brief  简单主循环（无定时器依赖）
 */
void Bypass_Main_Loop(void)
{
    unsigned int testCounter = 0;
    unsigned int switchCounter = 0;
    
    while(1) {
        testCounter++;
        switchCounter++;
        
        // 调试LED快速闪烁（程序运行指示）
        if (testCounter >= 30000) {
            testCounter = 0;
            DEBUG_1S_PIN = !DEBUG_1S_PIN;
        }
        
        // 交通灯慢速切换（状态指示）
        if (switchCounter >= 200000) {  // 大约每几秒切换一次
            switchCounter = 0;
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;
            
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
    Bypass_System_Init();
    Bypass_Main_Loop();
}