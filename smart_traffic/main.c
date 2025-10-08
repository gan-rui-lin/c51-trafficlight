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
    
    // 初始化显示模块（先初始化，用于延时测试）
    Display_Init();
    
    // ==========================================
    // 延时测试：通过数码管倒计时验证延时准确性
    // ==========================================
    // 显示 "99" 提示即将开始测试
    Display_ShowTime(9, 9);
    Delay_ms(500);
    
    // 测试3秒延时（数码管显示 3→2→1→0）
    Test_DelayAccuracy(3);
    
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
    Delay_ms(5000);
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
        // 【解决竞争问题】：读取数据前临时关中断，防止数据不一致
        {
            unsigned char tempState;
            unsigned char tempTimeLeft;
            
            // 快速读取当前状态（关中断保护）
            EA = 0;  // 关中断
            tempState = currentState;
            tempTimeLeft = timeLeft;
            EA = 1;  // 开中断
            
            // 根据当前交通灯状态计算两个方向的剩余时间
            switch(tempState) {
                case STATE_NS_GREEN_EW_RED:     // 南北绿灯，东西红灯
                    nsTime = tempTimeLeft;      // 南北：显示绿灯剩余时间
                    ewTime = tempTimeLeft + stateTimeTable[STATE_NS_YELLOW_EW_RED];  // 东西：需要等待黄灯时间
                    break;
                    
                case STATE_NS_YELLOW_EW_RED:    // 南北黄灯，东西红灯
                    nsTime = tempTimeLeft;      // 南北：显示黄灯剩余时间
                    ewTime = tempTimeLeft;      // 东西：红灯剩余时间等于黄灯时间
                    break;
                    
                case STATE_NS_RED_EW_GREEN:     // 南北红灯，东西绿灯
                    nsTime = tempTimeLeft + stateTimeTable[STATE_NS_RED_EW_YELLOW];  // 南北：需要等待黄灯时间
                    ewTime = tempTimeLeft;      // 东西：显示绿灯剩余时间
                    break;
                    
                case STATE_NS_RED_EW_YELLOW:    // 南北红灯，东西黄灯
                    nsTime = tempTimeLeft;      // 南北：红灯剩余时间等于黄灯时间
                    ewTime = tempTimeLeft;      // 东西：显示黄灯剩余时间
                    break;
                    
                default:
                    nsTime = 0;
                    ewTime = 0;
                    break;
            }
            
            // 限制显示范围 (0-9)，因为只使用2个数码管
            if (nsTime > 9) nsTime = 9;
            if (ewTime > 9) ewTime = 9;
        }
        
        // 【解决闪烁问题】：连续刷新显示多次，增加稳定性
        {
            unsigned char refresh;
            for(refresh = 0; refresh < 5; refresh++) {
                Display_ShowTime(nsTime, ewTime);
            }
        }
        
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
