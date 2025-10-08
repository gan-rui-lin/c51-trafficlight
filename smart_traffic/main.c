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

extern unsigned char currentState;
extern unsigned char timeLeft;
extern unsigned char isFlashing;
extern unsigned char stateTimeTable[4];


/*==============================================
 *                全局变量定义
 *==============================================*/

// 显示刷新变量
static unsigned char nsTime = 0;  // 南北方向显示时间
static unsigned char ewTime = 0;  // 东西方向显示时间

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
    
    // 硬件自检：所有灯亮1秒
    NS_RED_PIN = 1;      NS_YELLOW_PIN = 1;    NS_GREEN_PIN = 1;
    EW_RED_PIN = 1;      EW_YELLOW_PIN = 1;    EW_GREEN_PIN = 1;
    DEBUG_1S_PIN = 1;    DEBUG_STATE_PIN = 1;
    
    // 简单延时（自检延时）
    {
        unsigned int i, j;
        for(i = 0; i < 1000; i++) {
            for(j = 0; j < 100; j++) {
                // 延时约1秒
            }
        }
    }
    
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
    
    // 初始化显示模块
    Display_Init();
    
    // 初始化定时器（这将启动整个系统）
    Timer0_Init();
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
    
    // 主循环：定时器中断处理交通灯逻辑，主循环处理显示刷新
    while(1) {
        // ==========================================
        // 显示刷新处理（动态扫描）
        // ==========================================
        // 根据当前交通灯状态计算两个方向的剩余时间
        
        switch(currentState) {
            case STATE_NS_GREEN_EW_RED:     // 南北绿灯，东西红灯
                nsTime = timeLeft;          // 南北：显示绿灯剩余时间
                ewTime = timeLeft + stateTimeTable[STATE_NS_YELLOW_EW_RED];  // 东西：需要等待黄灯时间
                break;
                
            case STATE_NS_YELLOW_EW_RED:    // 南北黄灯，东西红灯
                nsTime = timeLeft;          // 南北：显示黄灯剩余时间
                ewTime = timeLeft;          // 东西：红灯剩余时间等于黄灯时间
                break;
                
            case STATE_NS_RED_EW_GREEN:     // 南北红灯，东西绿灯
                nsTime = timeLeft + stateTimeTable[STATE_NS_RED_EW_YELLOW];  // 南北：需要等待黄灯时间
                ewTime = timeLeft;          // 东西：显示绿灯剩余时间
                break;
                
            case STATE_NS_RED_EW_YELLOW:    // 南北红灯，东西黄灯
                nsTime = timeLeft;          // 南北：红灯剩余时间等于黄灯时间
                ewTime = timeLeft;          // 东西：显示黄灯剩余时间
                break;
                
            default:
                nsTime = 0;
                ewTime = 0;
                break;
        }
        
        // 限制显示范围 (0-9)，因为只使用2个数码管
        if (nsTime > 9) nsTime = 9;
        if (ewTime > 9) ewTime = 9;
        
        // 调用显示函数更新显示（动态扫描）
        Display_ShowTime(nsTime, ewTime);
        
        // ==========================================
        // 未来扩展功能
        // ==========================================
        // - 按键扫描处理
        // - 蓝牙通信处理
        // - 故障检测与报警
        // - 温度监控（DS18B20）
        // - 红外遥控接收
    }
}
