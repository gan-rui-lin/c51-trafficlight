/**************************************************
 * 文件名:    main.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-08
 * 描述:      智能交通灯系统 - 正式版本
 *           使用2个数码管显示南北和东西方向剩余时间
 *           基于74HC139译码器和共阳极数码管
 **************************************************/

#include "config.h"
#include "traffic_light.h"
#include "display.h"
#include "timer.h"

extern unsigned char currentState;
extern unsigned char timeLeft;
extern unsigned char isFlashing;
extern unsigned char stateTimeTable[4];


/*==============================================
 *                全局变量定义
 *==============================================*/

// 显示刷新变量（供Timer0中断使用，必须是全局变量）
volatile unsigned char nsTime = 0;  // 南北方向显示时间
volatile unsigned char ewTime = 0;  // 东西方向显示时间

/*==============================================
 *                系统初始化
 *==============================================*/
/**
 * @brief  系统初始化
 * @param  无
 * @retval 无
 */
void System_Init(void)
{
    // 初始化所有引脚为低电平
    DEBUG_1S_PIN = 0;    DEBUG_STATE_PIN = 0;
    NS_RED_PIN = 0;      NS_YELLOW_PIN = 0;    NS_GREEN_PIN = 0;
    EW_RED_PIN = 0;      EW_YELLOW_PIN = 0;    EW_GREEN_PIN = 0;
    
    // 初始化显示模块（先初始化，用于延时测试）
    Display_Init();
    
    // // 测试：左边显示1，右边显示2
    // Display_ShowTime(1, 2);
    // Delay_ms(3000);
    
    // ==========================================
    // 延时测试：通过数码管倒计时验证延时准确性
    // ==========================================
    // 显示 "99" 提示即将开始测试
    // Display_ShowTime(9, 9);
    Delay_ms(500);
    
    // // 测试3秒延时（数码管显示 3→2→1→0）
    // Test_DelayAccuracy(3);
    
    // ==========================================
    // 硬件自检：所有灯亮1秒
    // ==========================================
    NS_RED_PIN = 1;      NS_YELLOW_PIN = 1;    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;      EW_YELLOW_PIN = 1;    EW_GREEN_PIN = 1;
    DEBUG_1S_PIN = 1;    DEBUG_STATE_PIN = 1;
    
    // 使用标准延时函数
    Delay_ms(1000);  // 精确延时1秒
    
    // 关闭所有灯，准备正常工作
    NS_RED_PIN = 0;      NS_YELLOW_PIN = 0;    NS_GREEN_PIN = 0;
    EW_RED_PIN = 0;      EW_YELLOW_PIN = 0;    EW_GREEN_PIN = 0;
    DEBUG_1S_PIN = 0;    DEBUG_STATE_PIN = 0;
    
    // 初始化系统状态
    currentState = STATE_NS_GREEN_EW_RED;
    timeLeft = stateTimeTable[currentState];
    isFlashing = 0;
    
    // 设置初始交通灯状态
    SetTrafficLights(currentState);
    
    // 初始化定时器（这将启动整个系统）
    Timer0_Init();
    
    // 延时测试完成，显示 "88" 表示系统准备就绪
    Display_ShowTime(8, 8);
    Delay_ms(1000);
}

/*==============================================
 *                主函数
 *==============================================*/
/**
 * @brief  主函数
 * @param  无
 * @retval 无
 */
void main(void)
{
    // 系统初始化
    System_Init();
    
    // 主循环：定时器中断处理交通灯逻辑和显示刷新
    // 主循环只负责计算显示数值，实际显示由Timer0中断处理
    while(1) {
        // ==========================================
        // 计算显示数值（由主循环计算，Timer0中断显示）
        // ==========================================
        {
            unsigned char tempState;
            unsigned char tempTimeLeft;
            unsigned char newNsTime, newEwTime;
            
            // 快速读取当前状态（关中断保护）
            EA = 0;  // 关中断
            tempState = currentState;
            tempTimeLeft = timeLeft;
            EA = 1;  // 开中断
            
            // 根据当前交通灯状态计算两个方向的剩余时间
            // 【关键】：红灯方向显示需要等待的总时间
            //          绿灯/黄灯方向显示当前剩余时间
            switch(tempState) {
                case STATE_NS_GREEN_EW_RED:     // 状态0: 南北绿灯，东西红灯
                    newNsTime = tempTimeLeft;      // 南北：绿灯剩余时间（3→2→1）
                    // 东西红灯需要等：当前绿灯剩余 + 后续黄灯时间
                    newEwTime = tempTimeLeft + stateTimeTable[STATE_NS_YELLOW_EW_RED];
                    break;
                    
                case STATE_NS_YELLOW_EW_RED:    // 状态1: 南北黄灯，东西红灯
                    newNsTime = tempTimeLeft;      // 南北：黄灯剩余时间（3→2→1）
                    newEwTime = tempTimeLeft;      // 东西：红灯即将结束（3→2→1）
                    break;
                    
                case STATE_NS_RED_EW_GREEN:     // 状态2: 南北红灯，东西绿灯
                    // 南北红灯需要等：当前东西绿灯剩余 + 后续东西黄灯时间
                    newNsTime = tempTimeLeft + stateTimeTable[STATE_NS_RED_EW_YELLOW];
                    newEwTime = tempTimeLeft;      // 东西：绿灯剩余时间（3→2→1）
                    break;
                    
                case STATE_NS_RED_EW_YELLOW:    // 状态3: 南北红灯，东西黄灯
                    newNsTime = tempTimeLeft;      // 南北：红灯即将结束（3→2→1）
                    newEwTime = tempTimeLeft;      // 东西：黄灯剩余时间（3→2→1）
                    break;
                    
                default:
                    newNsTime = 0;
                    newEwTime = 0;
                    break;
            }
            
            // 限制显示范围 (0-9)，因为只使用2个数码管
            if (newNsTime > 9) newNsTime = 9;
            if (newEwTime > 9) newEwTime = 9;
            
            // 更新全局显示变量（供Timer0中断使用）
            EA = 0;  // 关中断
            nsTime = newNsTime;
            ewTime = newEwTime;
            EA = 1;  // 开中断
        }
        
        // ==========================================
        // 未来扩展功能
        // ==========================================
        // - 按键扫描处理
        // - 蓝牙通信处理
        // - 故障检测与报警
        // - 温度监控（DS18B20）
        // - 红外遥控接收
        
        // 主循环延时，降低CPU占用
        // 注意：不能延时太长，否则显示数值更新不及时
        // Delay_ms(10);  // 可选：如果CPU占用过高可以加
    }
}
