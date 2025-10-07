/**************************************************
 * 文件名:    main_no_interrupt.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      无中断版本 - 纯轮询实现定时
 **************************************************/

#include "config.h"

/*-----------------------主函数-------------------------*/
void main(void)
{
    unsigned long mainCounter = 0;
    unsigned char ledState = 0;
    
    // 初始化引脚
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    NS_GREEN_PIN = 0; NS_RED_PIN = 0;
    EW_GREEN_PIN = 0; EW_RED_PIN = 0;
    
    // 硬件自检
    DEBUG_1S_PIN = 1; DEBUG_STATE_PIN = 1;
    NS_GREEN_PIN = 1; NS_RED_PIN = 1;
    EW_GREEN_PIN = 1; EW_RED_PIN = 1;
    
    // 长延时
    for(mainCounter = 0; mainCounter < 100000UL; mainCounter++) {
        // 自检延时
    }
    
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    NS_GREEN_PIN = 0; NS_RED_PIN = 0;
    EW_GREEN_PIN = 0; EW_RED_PIN = 0;
    
    // 再次长延时
    for(mainCounter = 0; mainCounter < 100000UL; mainCounter++) {
        // 自检延时
    }
    
    // 设置初始状态
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    mainCounter = 0;
    
    // 主循环 - 完全基于计数的定时
    while(1) {
        mainCounter++;
        
        // 快速闪烁DEBUG_1S_PIN（程序运行指示）
        if ((mainCounter % 50000UL) == 0) {
            DEBUG_1S_PIN = !DEBUG_1S_PIN;
        }
        
        // 慢速切换（约每200万次循环）
        if ((mainCounter % 2000000UL) == 0) {
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;
            ledState = !ledState;
            
            if (ledState) {
                // 状态1：NS红，EW绿
                NS_GREEN_PIN = 0; NS_RED_PIN = 1;
                EW_RED_PIN = 0; EW_GREEN_PIN = 1;
            } else {
                // 状态0：NS绿，EW红
                NS_RED_PIN = 0; NS_GREEN_PIN = 1;
                EW_GREEN_PIN = 0; EW_RED_PIN = 1;
            }
            
            mainCounter = 0;  // 重置计数器避免溢出
        }
    }
}