/**************************************************
 * 文件名:    main_modular.c
 * 作者:      GitHub Copilot  
 * 日期:      2025-10-07
 * 描述:      智能交通灯系统 - 模块化版本
 *           基于可工作的单文件版本改造
 *           使用config.h和traffic_light模块
 **************************************************/

#include "config.h"
#include "traffic_light_simple.h"

/*==============================================
 *                全局变量定义
 *==============================================*/
// 系统状态变量
volatile unsigned char currentState = STATE_NS_GREEN_EW_RED;  // 当前交通灯状态
volatile unsigned char timeLeft = GREEN_LIGHT_TIME;           // 当前状态剩余时间
volatile unsigned char isFlashing = 0;                       // 闪烁标志

// 定时器相关变量
volatile unsigned int timer0Count = 0;         // Timer0中断计数器（用于产生1秒定时）
volatile unsigned int flashCount = 0;          // 闪烁计数器（用于控制闪烁频率）

// 状态时间配置表
unsigned char stateTimeTable[4] = {
    GREEN_LIGHT_TIME,   // 状态0：南北绿灯时间
    YELLOW_LIGHT_TIME,  // 状态1：南北黄灯时间
    GREEN_LIGHT_TIME,   // 状态2：东西绿灯时间  
    YELLOW_LIGHT_TIME   // 状态3：东西黄灯时间
};

/*==============================================
 *                硬件控制函数
 *==============================================*/
/**
 * @brief  设置交通灯状态
 * @param  state: 交通灯状态(0-3)
 * @retval 无
 */
void SetTrafficLights(unsigned char state)
{
    // 先关闭所有交通灯
    NS_RED_PIN = 0;    NS_YELLOW_PIN = 0;    NS_GREEN_PIN = 0;
    EW_RED_PIN = 0;    EW_YELLOW_PIN = 0;    EW_GREEN_PIN = 0;
    
    // 根据状态设置对应的交通灯
    switch(state) {
        case STATE_NS_GREEN_EW_RED:     // 南北绿灯，东西红灯
            NS_GREEN_PIN = 1;
            EW_RED_PIN = 1;
            break;
            
        case STATE_NS_YELLOW_EW_RED:    // 南北黄灯，东西红灯
            NS_YELLOW_PIN = 1;
            EW_RED_PIN = 1;
            break;
            
        case STATE_NS_RED_EW_GREEN:     // 南北红灯，东西绿灯
            NS_RED_PIN = 1;
            EW_GREEN_PIN = 1;
            break;
            
        case STATE_NS_RED_EW_YELLOW:    // 南北红灯，东西黄灯
            NS_RED_PIN = 1;
            EW_YELLOW_PIN = 1;
            break;
            
        default:
            // 异常状态：所有红灯亮起（安全状态）
            NS_RED_PIN = 1;
            EW_RED_PIN = 1;
            break;
    }
}

/**
 * @brief  处理交通灯闪烁逻辑
 * @param  无
 * @retval 无
 */
void HandleTrafficLightFlash(void)
{
    // 只在最后几秒进行闪烁
    if (timeLeft <= FLASH_START_TIME && timeLeft > 0) {
        isFlashing = 1;
        
        // 控制闪烁频率（约每500ms切换一次）
        if ((flashCount % 250) == 0) {
            switch(currentState) {
                case STATE_NS_GREEN_EW_RED:     // 南北绿灯闪烁
                    NS_GREEN_PIN = !NS_GREEN_PIN;
                    break;
                    
                case STATE_NS_YELLOW_EW_RED:    // 南北黄灯闪烁
                    NS_YELLOW_PIN = !NS_YELLOW_PIN;
                    break;
                    
                case STATE_NS_RED_EW_GREEN:     // 东西绿灯闪烁
                    EW_GREEN_PIN = !EW_GREEN_PIN;
                    break;
                    
                case STATE_NS_RED_EW_YELLOW:    // 东西黄灯闪烁
                    EW_YELLOW_PIN = !EW_YELLOW_PIN;
                    break;
            }
        }
    } else {
        isFlashing = 0;
    }
}

/**
 * @brief  切换到下一个交通灯状态
 * @param  无
 * @retval 无
 */
void SwitchToNextState(void)
{
    // 循环切换到下一个状态
    currentState = (currentState + 1) % 4;
    
    // 设置新状态的时间
    timeLeft = stateTimeTable[currentState];
    
    // 设置交通灯硬件状态
    SetTrafficLights(currentState);
    
    // 重置闪烁标志
    isFlashing = 0;
    
    // 状态切换指示：DEBUG_STATE_PIN闪烁一次
    DEBUG_STATE_PIN = 1;
    DEBUG_STATE_PIN = 0;
}

/*==============================================
 *                定时器初始化
 *==============================================*/
/**
 * @brief  Timer0初始化 - 配置为2ms中断
 * @param  无  
 * @retval 无
 * @note   11.0592MHz晶振，2ms定时
 */
void Timer0_Init(void)
{
    // 设置Timer0为模式1（16位定时器）
    TMOD &= 0xF0;        // 清除Timer0控制位
    TMOD |= 0x01;        // 设置Timer0为模式1
    
    // 设置定时器初值（2ms定时）
    TH0 = TIMER0_RELOAD_H;
    TL0 = TIMER0_RELOAD_L;
    
    // 清除相关标志位
    TF0 = 0;             // 清除Timer0溢出标志
    
    // 配置中断
    ET0 = 1;             // 使能Timer0中断
    PT0 = 1;             // 设置Timer0为高优先级中断
    EA = 1;              // 使能全局中断
    
    // 启动定时器
    TR0 = 1;             // 启动Timer0
    
    // 初始化计数器
    timer0Count = 0;
    flashCount = 0;
}

/*==============================================
 *                中断服务函数
 *==============================================*/
/**
 * @brief  Timer0中断服务函数
 * @param  无
 * @retval 无
 * @note   每2ms执行一次，500次中断产生1秒定时
 */
void Timer0_ISR(void) interrupt 1
{
    // 重新装载定时器初值
    TH0 = TIMER0_RELOAD_H;
    TL0 = TIMER0_RELOAD_L;
    
    // 关中断保护（重要！）
    TR0 = 0;  // 暂停计时器
    EA = 0;   // 全局关中断
    
    // 2ms定时计数
    timer0Count++;
    flashCount++;
    
    // 心跳指示：每100次中断（200ms）切换一次DEBUG_1S_PIN
    if ((timer0Count % 100) == 0) {
        DEBUG_1S_PIN = !DEBUG_1S_PIN;
    }
    
    // 处理闪烁逻辑（每2ms检查一次）
    HandleTrafficLightFlash();
    
    // 1秒定时处理：500次中断 = 1000ms = 1秒
    if (timer0Count >= 50) {
        timer0Count = 0;  // 重置计数器
        
        // 时间递减
        if (timeLeft > 0) {
            timeLeft--;
        }
        
        // 检查是否需要切换状态
        if (timeLeft == 0) {
            SwitchToNextState();
        }
    }
    
    // 恢复中断（重要！）
    EA = 1;   // 再开中断
    TR0 = 1;  // 恢复定时器
}

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
        // 主循环可以处理其他任务，如按键检测、通信等
        // 目前为空，所有交通灯逻辑都在中断中处理
        
        // 未来可以在这里添加：
        // - 按键扫描
        // - 显示更新
        // - 通信处理
        // - 故障检测
    }
}
