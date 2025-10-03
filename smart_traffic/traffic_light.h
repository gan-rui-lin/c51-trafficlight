/**************************************************
 * 文件名:    traffic_light.h
 * 作者:      
 * 日期:      2025-10-02
 * 描述:      交通灯控制模块头文件
 *           定义交通灯控制相关函数和接口
 **************************************************/

#ifndef __TRAFFIC_LIGHT_H__
#define __TRAFFIC_LIGHT_H__

#include "config.h"

/*-----------------------常量定义-----------------------------*/
// 交通灯状态定义
#define LIGHT_RED    0
#define LIGHT_YELLOW 1  
#define LIGHT_GREEN  2

// 方向定义
#define DIRECTION_NS 0  // 南北方向
#define DIRECTION_EW 1  // 东西方向

/*-----------------------全局变量声明-------------------------*/
extern unsigned char nsCurrentState;     // 南北方向当前状态
extern unsigned char ewCurrentState;     // 东西方向当前状态
extern unsigned char nsTimeLeft;         // 南北方向剩余时间
extern unsigned char ewTimeLeft;         // 东西方向剩余时间

/*-----------------------函数声明-----------------------------*/

/**
 * @brief  交通灯系统初始化
 * @param  无
 * @retval 无
 */
void TrafficLight_Init(void);

/**
 * @brief  设置交通灯状态
 * @param  direction: 方向 (DIRECTION_NS或DIRECTION_EW)
 * @param  state: 要设置的灯状态 (LIGHT_RED, LIGHT_YELLOW, LIGHT_GREEN)
 * @retval 无
 */
void TrafficLight_SetState(unsigned char direction, unsigned char state);

/**
 * @brief  更新交通灯物理输出
 * @param  direction: 交通方向 (DIRECTION_NS或DIRECTION_EW)
 * @param  state: 灯状态 (LIGHT_RED, LIGHT_YELLOW, LIGHT_GREEN)
 * @param  isBlinking: 是否闪烁
 * @retval 无
 */
void TrafficLight_UpdateOutput(unsigned char direction, unsigned char state, unsigned char isBlinking);

/**
 * @brief  交通灯定时处理（每秒调用一次）
 * @param  无
 * @retval 无
 */
void TrafficLight_TimerHandler(void);

/**
 * @brief  交通灯闪烁处理（定时调用）
 * @param  无
 * @retval 无
 */
void TrafficLight_BlinkHandler(void);

/**
 * @brief  设置交通灯时间
 * @param  direction: 交通方向 (DIRECTION_NS或DIRECTION_EW)
 * @param  greenTime: 绿灯时间
 * @param  yellowTime: 黄灯时间
 * @retval 设置是否成功（1成功，0失败）
 */
unsigned char TrafficLight_SetTime(unsigned char direction, unsigned char greenTime, unsigned char yellowTime);

/**
 * @brief  获取交通灯剩余时间
 * @param  direction: 交通方向 (DIRECTION_NS或DIRECTION_EW)
 * @retval 剩余时间
 */
unsigned char TrafficLight_GetTimeLeft(unsigned char direction);

/**
 * @brief  获取交通灯当前状态
 * @param  direction: 交通方向 (DIRECTION_NS或DIRECTION_EW)
 * @retval 当前灯状态 (LIGHT_RED, LIGHT_YELLOW, LIGHT_GREEN)
 */
unsigned char TrafficLight_GetState(unsigned char direction);

/**
 * @brief  紧急延时处理
 * @param  无
 * @retval 无
 */
void TrafficLight_EmergencyExtend(void);

/**
 * @brief  切换到手动控制模式
 * @param  enable: 1启用，0禁用
 * @retval 无
 */
void TrafficLight_ManualMode(unsigned char enable);

/**
 * @brief  手动控制交通灯
 * @param  direction: 交通方向 (DIRECTION_NS或DIRECTION_EW)
 * @param  state: 要设置的状态 (LIGHT_RED, LIGHT_YELLOW, LIGHT_GREEN)
 * @retval 无
 */
void TrafficLight_ManualControl(unsigned char direction, unsigned char state);

/**
 * @brief  重置交通灯为默认状态
 * @param  无
 * @retval 无
 */
void TrafficLight_Reset(void);

#endif /* __TRAFFIC_LIGHT_H__ */