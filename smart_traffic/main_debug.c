/**************************************************
 * 文件名:    main_debug.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-04
 * 描述:      简化调试版本 - 只测试定时器中断
 *           独立版本，不依赖其他模块
 **************************************************/

#include "config.h"
#include <intrins.h>  // 添加这个头文件以支持_nop_()函数

/*-----------------------全局变量-------------------------*/
volatile unsigned char debug_timer1sFlag = 0;  // 重命名避免冲突
volatile unsigned int debug_counter1s = 0;     // 重命名避免冲突

/*-----------------------函数声明-------------------------*/
void Debug_Timer_Init(void);
void Debug_System_Init(void);
void Debug_Main_Loop(void);

/*-----------------------Timer0中断服务函数----------------*/
void Debug_Timer0_ISR(void) interrupt 1  // 重命名避免冲突
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

/*-----------------------函数实现-------------------------*/

/**
 * @brief  简单定时器初始化
 */
void Debug_Timer_Init(void)
{
    // 设置Timer0为模式1（16位定时器）
    TMOD &= 0xF0;  // 清除Timer0控制位
    TMOD |= 0x01;  // 设置Timer0为模式1
    
    // 11.0592MHz 1ms定时初值
    TH0 = 0xFC;
    TL0 = 0x66;
    
    // 启用Timer0中断
    ET0 = 1;
    
    // 启动Timer0
    TR0 = 1;
}

/**
 * @brief  简单系统初始化
 */
void Debug_System_Init(void)
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
    
    // 简单延时（不使用复杂的Delay_ms函数）
    {
        unsigned int i, j;
        for(i = 0; i < 500; i++) {
            for(j = 0; j < 113; j++) {
                _nop_();
            }
        }
    }
    
    // 关闭所有灯
    NS_RED_PIN = 0; NS_YELLOW_PIN = 0; NS_GREEN_PIN = 0;
    EW_RED_PIN = 0; EW_YELLOW_PIN = 0; EW_GREEN_PIN = 0;
    DEBUG_1S_PIN = 0; DEBUG_STATE_PIN = 0;
    
    // 设置初始状态
    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;
    
    // 初始化定时器
    Debug_Timer_Init();
    
    // 开启全局中断
    EA = 1;
}

/**
 * @brief  简单主循环
 */
void Debug_Main_Loop(void)
{
    unsigned int testCounter = 0;
    
    while(1) {
        // 轮询计数器，验证程序运行
        testCounter++;
        if (testCounter >= 50000) {
            testCounter = 0;
            DEBUG_1S_PIN = !DEBUG_1S_PIN;  // P3.6闪烁证明程序运行
        }
        
        // 检查1秒定时器标志
        if (debug_timer1sFlag) {
            debug_timer1sFlag = 0;  // 清除标志
            DEBUG_STATE_PIN = !DEBUG_STATE_PIN;  // P3.7闪烁证明中断工作
            
            // 简单的交通灯切换
            if (NS_GREEN_PIN) {
                // 当前NS绿灯，切换到NS红灯，EW绿灯
                NS_GREEN_PIN = 0;
                NS_RED_PIN = 1;
                EW_RED_PIN = 0;
                EW_GREEN_PIN = 1;
            } else {
                // 当前EW绿灯，切换到NS绿灯，EW红灯
                EW_GREEN_PIN = 0;
                EW_RED_PIN = 1;
                NS_RED_PIN = 0;
                NS_GREEN_PIN = 1;
            }
        }
    }
}

/**
 * @brief  主函数
 */
void main(void)
{
    Debug_System_Init();
    Debug_Main_Loop();
}