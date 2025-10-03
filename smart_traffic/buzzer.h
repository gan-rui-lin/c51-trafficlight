/**************************************************
 * 文件名:    buzzer.h
 * 作者:      
 * 日期:      2025-10-02
 * 描述:      蜂鸣器模块头文件
 *           提供蜂鸣器控制功能，用于交通灯提示音
 **************************************************/

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "config.h"

/*-----------------------蜂鸣器模式定义-----------------------*/
typedef enum {
  BUZZER_MODE_OFF = 0,        // 关闭
  BUZZER_MODE_CONTINUOUS = 1, // 连续响
  BUZZER_MODE_BEEP = 2,       // 短促响（滴声）
  BUZZER_MODE_WARNING = 3,    // 警告声（长响）
  BUZZER_MODE_PULSE = 4       // 脉冲响（间歇响）
} BuzzerMode_t;

/*-----------------------全局变量声明-------------------------*/
extern BuzzerMode_t buzzerMode;
extern unsigned char buzzerEnable;

/*-----------------------函数声明-----------------------------*/

/**
 * @brief  蜂鸣器模块初始化
 * @param  无
 * @retval 无
 */
void Buzzer_Init(void);

/**
 * @brief  设置蜂鸣器模式
 * @param  mode: 蜂鸣器工作模式
 * @retval 无
 */
void Buzzer_SetMode(BuzzerMode_t mode);

/**
 * @brief  启用/禁用蜂鸣器
 * @param  enable: 1启用，0禁用
 * @retval 无
 */
void Buzzer_Enable(unsigned char enable);

/**
 * @brief  蜂鸣器开启
 * @param  无
 * @retval 无
 */
void Buzzer_On(void);

/**
 * @brief  蜂鸣器关闭
 * @param  无
 * @retval 无
 */
void Buzzer_Off(void);

/**
 * @brief  发出短促提示音
 * @param  无
 * @retval 无
 */
void Buzzer_Beep(void);

/**
 * @brief  发出警告音
 * @param  duration: 警告音持续时间（毫秒）
 * @retval 无
 */
void Buzzer_Warning(unsigned int duration);

/**
 * @brief  蜂鸣器定时处理（在定时器中断中调用）
 * @param  无
 * @retval 无
 */
void Buzzer_TimerHandler(void);

/**
 * @brief  检查是否需要蜂鸣器提示
 * @param  timeLeft: 剩余时间
 * @param  lightState: 交通灯状态
 * @retval 是否需要提示（1需要，0不需要）
 */
unsigned char Buzzer_ShouldBeep(unsigned char timeLeft,
                                LightState_t lightState);

#endif /* __BUZZER_H__ */