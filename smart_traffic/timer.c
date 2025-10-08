/**************************************************
 * 文件名:    timer.c
 * 作者:      GitHub Copilot
 * 日期:      2025-10-08
 * 描述:      定时器和延时函数模块实现
 *           提供标准的精确延时函数
 **************************************************/

#include "timer.h"
#include <intrins.h>  // 包含 _nop_() 函数

/*==============================================
 *                全局变量
 *==============================================*/
volatile unsigned int systemTime_s = 0;  // 系统运行时间（秒）

/*==============================================
 *                延时函数实现
 *==============================================*/

/**
 * @brief  延时1毫秒（使用Timer1模式1，精确延时）
 * @param  无
 * @retval 无
 * @note   11.0592MHz晶振，1ms定时
 *         计算: 1ms = 1000us
 *         机器周期 = 12 / 11.0592MHz ≈ 1.085us
 *         需要计数: 1000us / 1.085us ≈ 922次
 *         初值 = 65536 - 922 = 64614 = 0xFC66
 */
static void Delay_1ms(void)
{
    // 设置Timer1为模式1（16位定时器）
    TMOD &= 0x0F;        // 清除Timer1控制位
    TMOD |= 0x10;        // 设置Timer1为模式1
    
    TH1 = 0xFC;          // 设置初值高8位
    TL1 = 0x66;          // 设置初值低8位
    
    TF1 = 0;             // 清除溢出标志
    TR1 = 1;             // 启动Timer1
    
    while(!TF1);         // 等待溢出
    
    TR1 = 0;             // 停止Timer1
    TF1 = 0;             // 清除溢出标志
}

/**
 * @brief  延时指定毫秒数
 * @param  ms: 延时时间（毫秒）
 * @retval 无
 */
void Delay_ms(unsigned int ms)
{
    unsigned int i;
    for(i = 0; i < ms; i++) {
        Delay_1ms();
    }
}

/**
 * @brief  延时指定微秒数（软件延时）
 * @param  us: 延时时间（微秒）
 * @retval 无
 * @note   基于11.0592MHz晶振，软件延时
 *         机器周期约1.085us，每次循环约2个机器周期
 */
void Delay_us(unsigned int us)
{
    while(us--) {
        _nop_();  // 内部函数，1个机器周期
        _nop_();  // 内部函数，1个机器周期
    }
}

/**
 * @brief  延时指定秒数
 * @param  sec: 延时时间（秒）
 * @retval 无
 */
void Delay_s(unsigned char sec)
{
    unsigned char i;
    for(i = 0; i < sec; i++) {
        Delay_ms(1000);
    }
}

/*==============================================
 *                时间管理函数
 *==============================================*/

/**
 * @brief  获取系统运行时间（秒）
 * @param  无
 * @retval 系统运行秒数
 */
unsigned int Get_SystemTime_s(void)
{
    return systemTime_s;
}

/**
 * @brief  重置系统运行时间计数器
 * @param  无
 * @retval 无
 */
void Reset_SystemTime(void)
{
    systemTime_s = 0;
}

/*==============================================
 *                延时测试函数
 *==============================================*/

/**
 * @brief  测试延时精度（在数码管上显示倒计时）
 * @param  testSeconds: 测试秒数（0-9）
 * @retval 无
 * @note   通过数码管显示倒计时来验证延时是否准确
 *         每秒数字递减，用于观察1秒延时是否准确
 */
void Test_DelayAccuracy(unsigned char testSeconds)
{
    unsigned char count;
    extern void Display_ShowTime(unsigned char nsTime, unsigned char ewTime);
    
    // 限制测试范围
    if (testSeconds > 9) testSeconds = 9;
    
    // 倒计时显示
    for(count = testSeconds; count > 0; count--) {
        // 显示当前计数
        Display_ShowTime(count, count);
        
        // 延时1秒（通过多次刷新显示保持稳定）
        {
            unsigned char i;
            for(i = 0; i < 100; i++) {
                Display_ShowTime(count, count);
                Delay_ms(10);  // 每10ms刷新一次显示
            }
        }
    }
    
    // 显示 "00" 表示测试结束
    Display_ShowTime(0, 0);
    Delay_ms(2000);
}
