/**************************************************
 * 文件名:    main_minimal.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      最小测试程序 - 只包含最基本的功能
 **************************************************/

#include <reg52.h>  // 直接使用标准头文件，不依赖config.h

// 直接定义引脚，不依赖config.h
sbit DEBUG_LED1 = P3^6;
sbit DEBUG_LED2 = P3^7;
sbit NS_GREEN = P2^2;
sbit NS_RED = P2^0;
sbit EW_GREEN = P2^5;
sbit EW_RED = P2^3;

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
    
    // 初始化所有引脚为0
    DEBUG_LED1 = 0;
    DEBUG_LED2 = 0;
    NS_GREEN = 0;
    NS_RED = 0;
    EW_GREEN = 0;
    EW_RED = 0;
    
    // 硬件自检 - 所有灯闪烁一次
    DEBUG_LED1 = 1; DEBUG_LED2 = 1;
    NS_GREEN = 1; NS_RED = 1; EW_GREEN = 1; EW_RED = 1;
    Simple_Delay(1000);
    
    DEBUG_LED1 = 0; DEBUG_LED2 = 0;
    NS_GREEN = 0; NS_RED = 0; EW_GREEN = 0; EW_RED = 0;
    Simple_Delay(1000);
    
    // 设置初始状态
    NS_GREEN = 1;
    EW_RED = 1;
    
    // 主循环
    while(1) {
        counter++;
        
        // 调试LED1快速闪烁
        if ((counter % 10000) == 0) {
            DEBUG_LED1 = !DEBUG_LED1;
        }
        
        // 调试LED2和交通灯慢速切换
        if ((counter % 100000) == 0) {
            DEBUG_LED2 = !DEBUG_LED2;
            
            // 交通灯切换
            if (NS_GREEN) {
                NS_GREEN = 0; NS_RED = 1;
                EW_RED = 0; EW_GREEN = 1;
            } else {
                EW_GREEN = 0; EW_RED = 1;
                NS_RED = 0; NS_GREEN = 1;
            }
            
            counter = 0;  // 重置计数器
        }
    }
}