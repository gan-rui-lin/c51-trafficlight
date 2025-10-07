/**************************************************
 * 文件名:    main_modular.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-07
 * 描述:      智能交通灯系统 - 模块化版本
 *           基于可工作的单文件版本改造
 *           使用config.h和traffic_light模块
 **************************************************/

#include "config.h"
#include "traffic_light.h"
#include "display.h"

extern unsigned char currentState;
extern unsigned char timeLeft;
extern unsigned char isFlashing;

extern unsigned char stateTimeTable[4];

	            unsigned char nsTime = 0;
            unsigned char ewTime = 0;


/*==============================================
 *                全局变量定义
 *==============================================*/

// 显示刷新计数器
static unsigned int displayRefreshCounter = 0;

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
    
    // 主循环：所有工作都在Timer0中断中完成
    // 主循环只需要保持系统运行

    while(1) {
        // 显示刷新处理（动态扫描显示）
        displayRefreshCounter++;
        if (displayRefreshCounter >= 100) {  // 每100次循环刷新一次
            displayRefreshCounter = 0;
            
            // 计算并显示南北和东西方向的剩余时间
            // 南北方向的剩余时间就是当前状态的剩余时间
            // 东西方向的剩余时间需要根据状态计算

            
            switch(currentState) {
                case STATE_NS_GREEN_EW_RED:     // 南北绿灯，东西红灯
                    nsTime = timeLeft;          // 南北显示剩余绿灯时间
                    ewTime = timeLeft + stateTimeTable[STATE_NS_YELLOW_EW_RED];  // 东西需要等待南北黄灯
                    break;
                    
                case STATE_NS_YELLOW_EW_RED:    // 南北黄灯，东西红灯
                    nsTime = timeLeft;          // 南北显示剩余黄灯时间
                    ewTime = timeLeft;          // 东西红灯剩余时间等于南北黄灯时间
                    break;
                    
                case STATE_NS_RED_EW_GREEN:     // 南北红灯，东西绿灯
                    nsTime = timeLeft + stateTimeTable[STATE_NS_RED_EW_YELLOW];  // 南北需要等待东西黄灯
                    ewTime = timeLeft;          // 东西显示剩余绿灯时间
                    break;
                    
                case STATE_NS_RED_EW_YELLOW:    // 南北红灯，东西黄灯
                    nsTime = timeLeft;          // 南北红灯剩余时间等于东西黄灯时间
                    ewTime = timeLeft;          // 东西显示剩余黄灯时间
                    break;
                    
                default:
                    nsTime = 0;
                    ewTime = 0;
                    break;
            }
            
            // 调用显示函数更新显示
            Display_ShowTime(nsTime, ewTime);
        }
        
        // 未来可以在这里添加：
        // - 按键扫描
        // - 通信处理
        // - 故障检测
    }
}
