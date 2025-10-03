/**************************************************
 * 文件名:    traffic_light.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      交通灯控制模块实现文件
 *           实现交通灯的基本控制逻辑，使用宏定义保证可读性
 **************************************************/

#include "traffic_light.h"

/*-----------------------全局变量定义-------------------------*/
unsigned char nsCurrentState = LIGHT_GREEN;     // 南北方向当前状态
unsigned char ewCurrentState = LIGHT_RED;       // 东西方向当前状态
unsigned char nsTimeLeft = DEFAULT_GREEN_TIME;  // 南北方向剩余时间
unsigned char ewTimeLeft = DEFAULT_RED_TIME;    // 东西方向剩余时间

// 时间设置变量
static unsigned char nsGreenTime = DEFAULT_GREEN_TIME;
static unsigned char nsYellowTime = DEFAULT_YELLOW_TIME;
static unsigned char nsRedTime = DEFAULT_RED_TIME;
static unsigned char ewGreenTime = DEFAULT_GREEN_TIME;
static unsigned char ewYellowTime = DEFAULT_YELLOW_TIME;
static unsigned char ewRedTime = DEFAULT_RED_TIME;

// 内部状态变量
static unsigned char manualMode = 0;       // 手动控制模式标志
static unsigned int blinkCounter = 0;      // 闪烁计数器

/*-----------------------内部函数声明-------------------------*/
static void updateLightOutput(unsigned char direction, unsigned char state, unsigned char isBlinking);
static unsigned char getNextState(unsigned char currentState);
static unsigned char getTimeForState(unsigned char direction, unsigned char state);

/**
 * @brief  交通灯系统初始化
 */
void TrafficLight_Init(void)
{
    // 初始化南北方向交通灯（开始为绿灯）
    nsCurrentState = LIGHT_GREEN;
    nsTimeLeft = DEFAULT_GREEN_TIME;
    
    // 初始化东西方向交通灯（开始为红灯）
    ewCurrentState = LIGHT_RED;
    ewTimeLeft = DEFAULT_RED_TIME;
    
    // 重置内部状态
    manualMode = 0;
    blinkCounter = 0;
    
    // 设置初始输出状态
    updateLightOutput(DIRECTION_NS, LIGHT_GREEN, 0);
    updateLightOutput(DIRECTION_EW, LIGHT_RED, 0);
}

/**
 * @brief  设置交通灯状态
 */
void TrafficLight_SetState(unsigned char direction, unsigned char state)
{
    if (direction == DIRECTION_NS) {
        nsCurrentState = state;
        nsTimeLeft = getTimeForState(DIRECTION_NS, state);
    } else if (direction == DIRECTION_EW) {
        ewCurrentState = state;
        ewTimeLeft = getTimeForState(DIRECTION_EW, state);
    }
    
    // 更新物理输出
    updateLightOutput(direction, state, 0);
}

/*
**
 * @brief  更新交通灯物理输出
 */
/*
void TrafficLight_UpdateOutput(unsigned char direction, unsigned char state, unsigned char isBlinking)
{
    updateLightOutput(direction, state, isBlinking);
}
*/

/**
 * @brief  交通灯定时处理（每秒调用一次）
 */
void TrafficLight_TimerHandler(void)
{
    // 如果是手动模式，不进行自动处理
    if (manualMode) return;
    
    // 处理南北方向
    if (nsTimeLeft > 0) {
        nsTimeLeft--;
        if (nsTimeLeft == 0) {
            // 时间到，切换到下一个状态
            nsCurrentState = getNextState(nsCurrentState);
            nsTimeLeft = getTimeForState(DIRECTION_NS, nsCurrentState);
            updateLightOutput(DIRECTION_NS, nsCurrentState, 0);
        }
    }
    
    // 处理东西方向
    if (ewTimeLeft > 0) {
        ewTimeLeft--;
        if (ewTimeLeft == 0) {
            // 时间到，切换到下一个状态
            ewCurrentState = getNextState(ewCurrentState);
            ewTimeLeft = getTimeForState(DIRECTION_EW, ewCurrentState);
            updateLightOutput(DIRECTION_EW, ewCurrentState, 0);
        }
    }
}

/**
 * @brief  交通灯闪烁处理（定时调用）
 */
void TrafficLight_BlinkHandler(void)
{
    blinkCounter++;
    if (blinkCounter >= 50) {  // 约500ms闪烁一次
        blinkCounter = 0;
        // 这里可以添加闪烁逻辑
    }
}

/*
**
 * @brief  设置交通灯时间
 */
/*
unsigned char TrafficLight_SetTime(unsigned char direction, unsigned char greenTime, unsigned char yellowTime)
{
    // 参数有效性检查
    if (greenTime < 5 || greenTime > 99) return 0;
    if (yellowTime < 2 || yellowTime > 10) return 0;
    
    if (direction == DIRECTION_NS) {
        nsGreenTime = greenTime;
        nsYellowTime = yellowTime;
        nsRedTime = greenTime + yellowTime;  // 红灯时间根据对方绿灯+黄灯时间计算
    } else if (direction == DIRECTION_EW) {
        ewGreenTime = greenTime;
        ewYellowTime = yellowTime;
        ewRedTime = greenTime + yellowTime;
    } else {
        return 0;  // 无效方向
    }
    
    return 1;  // 设置成功
}
*/

/**
 * @brief  获取交通灯剩余时间
 */
unsigned char TrafficLight_GetTimeLeft(unsigned char direction)
{
    if (direction == DIRECTION_NS) {
        return nsTimeLeft;
    } else if (direction == DIRECTION_EW) {
        return ewTimeLeft;
    }
    return 0;  // 无效方向
}

/**
 * @brief  获取交通灯当前状态
 */
unsigned char TrafficLight_GetState(unsigned char direction)
{
    if (direction == DIRECTION_NS) {
        return nsCurrentState;
    } else if (direction == DIRECTION_EW) {
        return ewCurrentState;
    }
    return LIGHT_RED;  // 默认返回红灯
}

/**
 * @brief  紧急延时处理
 */
void TrafficLight_EmergencyExtend(void)
{
    // 延长当前绿灯状态时间
    if (nsCurrentState == LIGHT_GREEN) {
        nsTimeLeft += 10;  // 延长10秒
    }
    if (ewCurrentState == LIGHT_GREEN) {
        ewTimeLeft += 10;  // 延长10秒
    }
}

/*
**
 * @brief  切换到手动控制模式
 */
/*
void TrafficLight_ManualMode(unsigned char enable)
{
    manualMode = enable;
}
*/

/*
**
 * @brief  手动控制交通灯
 */
/*
void TrafficLight_ManualControl(unsigned char direction, unsigned char state)
{
    if (manualMode) {
        TrafficLight_SetState(direction, state);
    }
}
*/

/*
**
 * @brief  重置交通灯为默认状态
 */
/*
void TrafficLight_Reset(void)
{
    TrafficLight_Init();
}
*/

/*-----------------------内部函数实现-------------------------*/

/**
 * @brief  获取下一个状态
 * @param  currentState: 当前状态
 * @retval 下一个状态
 */
static unsigned char getNextState(unsigned char currentState)
{
    switch (currentState) {
        case LIGHT_GREEN:
            return LIGHT_YELLOW;
        case LIGHT_YELLOW:
            return LIGHT_RED;
        case LIGHT_RED:
            return LIGHT_GREEN;
        default:
            return LIGHT_RED;  // 异常情况默认返回红灯
    }
}

/**
 * @brief  获取指定方向和状态的时间
 * @param  direction: 方向
 * @param  state: 状态
 * @retval 对应的时间
 */
static unsigned char getTimeForState(unsigned char direction, unsigned char state)
{
    if (direction == DIRECTION_NS) {
        switch (state) {
            case LIGHT_GREEN:  return nsGreenTime;
            case LIGHT_YELLOW: return nsYellowTime;
            case LIGHT_RED:    return nsRedTime;
            default:           return nsRedTime;
        }
    } else if (direction == DIRECTION_EW) {
        switch (state) {
            case LIGHT_GREEN:  return ewGreenTime;
            case LIGHT_YELLOW: return ewYellowTime;
            case LIGHT_RED:    return ewRedTime;
            default:           return ewRedTime;
        }
    }
    return DEFAULT_RED_TIME;  // 默认时间
}

/**
 * @brief  更新交通灯物理输出
 * @param  direction: 方向
 * @param  state: 状态
 * @param  isBlinking: 是否闪烁
 */
static void updateLightOutput(unsigned char direction, unsigned char state, unsigned char isBlinking)
{
    if (direction == DIRECTION_NS) {
        // 先关闭所有南北方向的灯
        NS_RED_PIN = 0;
        NS_YELLOW_PIN = 0;
        NS_GREEN_PIN = 0;
        
        // 根据状态点亮对应的灯（非闪烁模式）
        if (!isBlinking) {
            switch (state) {
                case LIGHT_RED:
                    NS_RED_PIN = 1;
                    break;
                case LIGHT_YELLOW:
                    NS_YELLOW_PIN = 1;
                    break;
                case LIGHT_GREEN:
                    NS_GREEN_PIN = 1;
                    break;
            }
        }
    } else if (direction == DIRECTION_EW) {
        // 先关闭所有东西方向的灯
        EW_RED_PIN = 0;
        EW_YELLOW_PIN = 0;
        EW_GREEN_PIN = 0;
        
        // 根据状态点亮对应的灯（非闪烁模式）
        if (!isBlinking) {
            switch (state) {
                case LIGHT_RED:
                    EW_RED_PIN = 1;
                    break;
                case LIGHT_YELLOW:
                    EW_YELLOW_PIN = 1;
                    break;
                case LIGHT_GREEN:
                    EW_GREEN_PIN = 1;
                    break;
            }
        }
    }
}