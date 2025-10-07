/**************************************************
 * 文件名:    main_timer_debug.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      定时器调试版本 - 逐步测试定时器功能
 **************************************************/

#include "config.h"

/*-----------------------全局变量-------------------------*/
volatile unsigned char timerFlag = 0;
volatile unsigned int timerCounter = 0;

/*-----------------------Timer0中断服务函数----------------*/
void TimerDebug_ISR(void) interrupt 1
{
    // 重新装载定时器初值
    TH0 = 0xFC;
    TL0 = 0x66;
    
    // 简单计数，避免复杂逻辑
    timerCounter++;
    if (timerCounter >= 500) {  // 大约0.5秒
        timerCounter = 0;
        timerFlag = 1;
    }
}

/*-----------------------简单延时-------------------------*/
void TimerDebug_Delay(unsigned int count)
{
    unsigned int i, j;
    for(i = 0; i < count; i++) {
        for(j = 0; j < 200; j++) {
            // 空循环
        }
    }
}

/*-----------------------主函数-------------------------*/
void main(void)
{
    unsigned int loopCounter = 0;
    
    // 关闭中断
    EA = 0;
    
    // 初始化引脚
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    NS_GREEN_PIN = 0;
    EW_RED_PIN = 0;
    
    // 硬件自检（使用简单延时）
    DEBUG_1S_PIN = 1; DEBUG_STATE_PIN = 1;
    NS_GREEN_PIN = 1; EW_RED_PIN = 1;
    TimerDebug_Delay(1000);
    
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    NS_GREEN_PIN = 0; EW_RED_PIN = 0;
    TimerDebug_Delay(1000);
    
    // 设置初始状态
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    
    // 配置定时器（但先不启动）
    TMOD &= 0xF0;      // 清除Timer0控制位
    TMOD |= 0x01;      // 设置Timer0为模式1
    TH0 = 0xFC;        // 定时器初值
    TL0 = 0x66;
    TF0 = 0;           // 清除Timer0溢出标志
    
    // 启用Timer0中断
    ET0 = 1;           // 启用Timer0中断
    PT0 = 1;           // 设置Timer0为高优先级中断
    
    // 启动定时器
    TR0 = 1;
    
    // 最后开启全局中断
    EA = 1;
    
    // 主循环
    while(1) {
        loopCounter++;
        
        // 程序运行指示（快速闪烁）
        if ((loopCounter % 30000) == 0) {
            DEBUG_1S_PIN = !DEBUG_1S_PIN;
        }
        
        // 检查定时器标志
        if (timerFlag) {
            timerFlag = 0;  // 清除标志
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;  // 定时器工作指示
            
            // 简单切换交通灯
            if (NS_GREEN_PIN) {
                NS_GREEN_PIN = 0; NS_RED_PIN = 1;
                EW_RED_PIN = 0; EW_GREEN_PIN = 1;
            } else {
                NS_RED_PIN = 0; NS_GREEN_PIN = 1;
                EW_GREEN_PIN = 0; EW_RED_PIN = 1;
            }
        }
    }
}