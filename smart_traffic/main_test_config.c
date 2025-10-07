/**************************************************
 * 文件名:    main_test_config.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      测试config.h - 只包含config.h，不包含其他模块
 **************************************************/

#include "config.h"  // 测试config.h是否有问题

/*-----------------------最简延时函数-------------------------*/
void Simple_Delay(unsigned int count)
{
    unsigned int i, j;
    for(i = 0; i < count; i++) {
        for(j = 0; j < 200; j++) {
            // 空循环延时
        }
    }
}

/*-----------------------主函数-----------------------------*/
void main(void)
{
    unsigned int counter = 0;
    
    // 使用config.h中定义的宏
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    NS_GREEN_PIN = 0;
    NS_RED_PIN = 0;
    EW_GREEN_PIN = 0;
    EW_RED_PIN = 0;
    
    // 硬件自检
    DEBUG_1S_PIN = 1; DEBUG_STATE_PIN = 1;
    NS_GREEN_PIN = 1; NS_RED_PIN = 1; 
    EW_GREEN_PIN = 1; EW_RED_PIN = 1;
    Simple_Delay(1000);
    
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    NS_GREEN_PIN = 0; NS_RED_PIN = 0; 
    EW_GREEN_PIN = 0; EW_RED_PIN = 0;
    Simple_Delay(1000);
    
    // 设置初始状态
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    
    // 主循环
    while(1) {
        counter++;
        
        // 调试LED1快速闪烁
        if ((counter % 10000) == 0) {
            DEBUG_1S_PIN = !DEBUG_1S_PIN;
        }
        
        // 调试LED2和交通灯慢速切换
        if ((counter % 100000) == 0) {
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;
            
            // 交通灯切换
            if (NS_GREEN_PIN) {
                NS_GREEN_PIN = 0; NS_RED_PIN = 1;
                EW_RED_PIN = 0; EW_GREEN_PIN = 1;
            } else {
                EW_GREEN_PIN = 0; EW_RED_PIN = 1;
                NS_RED_PIN = 0; NS_GREEN_PIN = 1;
            }
            
            counter = 0;
        }
    }
}