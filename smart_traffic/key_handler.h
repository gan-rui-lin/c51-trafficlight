/**************************************************
 * 文件名:    key_handler.h
 * 作者:
 * 日期:      2025-10-02
 * 描述:      按键处理模块头文件
 *           实现操作员按键功能，允许设定不同方向的红绿灯时长
 **************************************************/

#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include "config.h"

/*-----------------------按键状态定义-------------------------*/
typedef enum {
  KEY_STATE_IDLE = 0,    // 空闲状态
  KEY_STATE_PRESSED = 1, // 按下状态
  KEY_STATE_RELEASED = 2 // 释放状态
} KeyState_t;

/*-----------------------设置模式状态-------------------------*/
typedef enum {
  SETTING_IDLE = 0,        // 设置空闲
  SETTING_SELECT_DIR = 1,  // 选择方向
  SETTING_GREEN_TIME = 2,  // 设置绿灯时间
  SETTING_YELLOW_TIME = 3, // 设置黄灯时间
  SETTING_CONFIRM = 4      // 确认设置
} SettingState_t;

/*-----------------------全局变量声明-------------------------*/
extern SettingState_t settingState;
extern Direction_t settingDirection;
extern unsigned char tempGreenTime;
extern unsigned char tempYellowTime;

/*-----------------------函数声明-----------------------------*/

/**
 * @brief  按键模块初始化
 * @param  无
 * @retval 无
 */
void Key_Init(void);

/**
 * @brief  按键扫描处理
 * @param  无
 * @retval 无
 */
void Key_Scan(void);

/**
 * @brief  设置模式按键处理
 * @param  无
 * @retval 无
 */
void Key_SetModeHandler(void);

/**
 * @brief  确认键处理
 * @param  无
 * @retval 无
 */
void Key_ConfirmHandler(void);

/**
 * @brief  增加键处理
 * @param  无
 * @retval 无
 */
void Key_UpHandler(void);

/**
 * @brief  减少键处理
 * @param  无
 * @retval 无
 */
void Key_DownHandler(void);

/**
 * @brief  紧急键处理
 * @param  无
 * @retval 无
 */
void Key_EmergencyHandler(void);

/**
 * @brief  获取当前设置状态
 * @param  无
 * @retval 设置状态
 */
SettingState_t Key_GetSettingState(void);

/**
 * @brief  获取设置方向
 * @param  无
 * @retval 设置的方向
 */
Direction_t Key_GetSettingDirection(void);

/**
 * @brief  获取临时绿灯时间
 * @param  无
 * @retval 绿灯时间
 */
unsigned char Key_GetTempGreenTime(void);

/**
 * @brief  获取临时黄灯时间
 * @param  无
 * @retval 黄灯时间
 */
unsigned char Key_GetTempYellowTime(void);

/**
 * @brief  退出设置模式
 * @param  无
 * @retval 无
 */
void Key_ExitSettingMode(void);

/**
 * @brief  按键消抖延时
 * @param  无
 * @retval 无
 */
void Key_Delay(void);

#endif /* __KEY_HANDLER_H__ */