/**************************************************
 * 文件名:    main_working_version.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      可工作的完整版本 - 基于main_minimal.c扩展
 **************************************************/

#include "config.h"

/*-----------------------全局变量-------------------------*/
volatile unsigned char debug_timer1sFlag = 0;
volatile unsigned int debug_counter1s = 0;
unsigned char globalState = 0;  // 0=NS绿EW红, 1=NS红EW绿
unsigned char globalTimeLeft = 5;  // 5秒切换

/*-----------------------Timer0中断服务函数----------------*/
void Working_Timer0_ISR(void) interrupt 1
{
    // 重新装载定时器初值（11.0592MHz配置）
    TH0 = 0xFC;
    TL0 = 0x66;
    
    // 处理1s计数
    debug_counter1s++;
    if (debug_counter1s >= 1000) {
        debug_counter1s = 0;
        debug_timer1sFlag = 1;
    }
}

/*-----------------------简单延时函数-------------------------*/
void Working_Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 113; j++) {
            // 简单延时循环
        }
    }
}

/*-----------------------系统初始化-------------------------*/
void Working_System_Init(void)
{
    // 关闭中断
    EA = 0;
    
    // 初始化引脚
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    NS_RED_PIN = 0; NS_YELLOW_PIN = 0; NS_GREEN_PIN = 0;
    EW_RED_PIN = 0; EW_YELLOW_PIN = 0; EW_GREEN_PIN = 0;
    
    // 硬件自检
    DEBUG_1S_PIN = 1; DEBUG_STATE_PIN = 1;
    NS_RED_PIN = 1; NS_YELLOW_PIN = 1; NS_GREEN_PIN = 1;
    EW_RED_PIN = 1; EW_YELLOW_PIN = 1; EW_GREEN_PIN = 1;
    Working_Delay_ms(500);
    
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    NS_RED_PIN = 0; NS_YELLOW_PIN = 0; NS_GREEN_PIN = 0;
    EW_RED_PIN = 0; EW_YELLOW_PIN = 0; EW_GREEN_PIN = 0;
    Working_Delay_ms(500);
    
    // 初始化定时器
    TMOD &= 0xF0;  // 清除Timer0控制位
    TMOD |= 0x01;  // 设置Timer0为模式1
    TH0 = 0xFC;    // 高8位
    TL0 = 0x66;    // 低8位
    TF0 = 0;       // 清除Timer0溢出标志
    ET0 = 1;       // 启用Timer0中断
    PT0 = 1;       // 设置Timer0为高优先级中断
    TR0 = 1;       // 启动Timer0
    IE0 = 1;       // 启用外部中断0

    // 设置初始状态
    globalState = 0;
    globalTimeLeft = 5;
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    
    // 开启中断
    EA = 1;
}

/*-----------------------主循环-------------------------*/
void Working_Main_Loop(void)
{
    unsigned int testCounter = 0;
    
    while(1) {
        testCounter++;
        if (testCounter >= 50000) {
            testCounter = 0;
            DEBUG_1S_PIN = !DEBUG_1S_PIN;  // 程序运行指示
        }
        
        // 检查1秒标志
        if (debug_timer1sFlag) {
            debug_timer1sFlag = 0;
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;  // 中断工作指示
            
            // 倒计时
            if (globalTimeLeft > 0) {
                globalTimeLeft--;
            }
            
            if (globalTimeLeft == 0) {
                // 切换状态
                if (globalState == 0) {
                    // 切换到NS红EW绿
                    globalState = 1;
                    globalTimeLeft = 5;
                    NS_GREEN_PIN = 0; NS_RED_PIN = 1;
                    EW_RED_PIN = 0; EW_GREEN_PIN = 1;
                } else {
                    // 切换到NS绿EW红
                    globalState = 0;
                    globalTimeLeft = 5;
                    NS_RED_PIN = 0; NS_GREEN_PIN = 1;
                    EW_GREEN_PIN = 0; EW_RED_PIN = 1;
                }
            }
        }
    }
}

/*-----------------------主函数-------------------------*/
void main(void)
{
    Working_System_Init();
    Working_Main_Loop();
}