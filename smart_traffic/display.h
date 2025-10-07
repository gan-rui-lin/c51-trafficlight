/**************************************************
 * 文件名:    display.h
 * 作者:
 * 日期:      2025-10-02
 * 描述:      显示模块头文件
 *           实现倒计时显示功能，使用数码管或LED显示
 **************************************************/

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

// #include "buzzer.h"
#include "config.h"

/*-----------------------显示模式定义-------------------------*/
typedef enum {
  DISPLAY_MODE_TIME = 0,    // 时间显示模式
  DISPLAY_MODE_SETTING = 1, // 设置显示模式
  DISPLAY_MODE_ERROR = 2,   // 错误显示模式
  DISPLAY_MODE_OFF = 3      // 关闭显示
} DisplayMode_t;

/*-----------------------显示位置定义-------------------------*/
typedef enum {
  DISPLAY_POS_NS_LEFT = 0,  // 南北方向左侧（十位）
  DISPLAY_POS_NS_RIGHT = 1, // 南北方向右侧（个位）
  DISPLAY_POS_EW_LEFT = 2,  // 东西方向左侧（十位）
  DISPLAY_POS_EW_RIGHT = 3, // 东西方向右侧（个位）
  DISPLAY_POS_SET_LEFT = 4, // 设置显示左侧
  DISPLAY_POS_SET_RIGHT = 5 // 设置显示右侧
} DisplayPos_t;

/*-----------------------全局变量声明-------------------------*/
extern DisplayMode_t displayMode;
extern unsigned char displayBrightness;

/*-----------------------函数声明-----------------------------*/

/**
 * @brief  显示模块初始化
 * @param  无
 * @retval 无
 */
void Display_Init(void);

/**
 * @brief  显示倒计时
 * @param  nsTime: 南北方向剩余时间
 * @param  ewTime: 东西方向剩余时间
 * @retval 无
 */
void Display_ShowTime(unsigned char nsTime, unsigned char ewTime);

/**
 * @brief  显示设置信息
 * @param  direction: 设置方向
 * @param  settingState: 设置状态
 * @param  greenTime: 绿灯时间
 * @param  yellowTime: 黄灯时间
 * @retval 无
 */
void Display_ShowSetting(unsigned char direction, unsigned char settingState,
                         unsigned char greenTime, unsigned char yellowTime);

/**
 * @brief  显示错误信息
 * @param  errorCode: 错误代码
 * @retval 无
 */
void Display_ShowError(unsigned char errorCode);

/**
 * @brief  设置显示模式
 * @param  mode: 显示模式
 * @retval 无
 */
void Display_SetMode(DisplayMode_t mode);

/**
 * @brief  设置显示亮度
 * @param  brightness: 亮度值（0-7）
 * @retval 无
 */
void Display_SetBrightness(unsigned char brightness);

/**
 * @brief  数码管显示刷新（定时调用）
 * @param  无
 * @retval 无
 */
void Display_Refresh(void);

/**
 * @brief  清除显示
 * @param  无
 * @retval 无
 */
void Display_Clear(void);

/**
 * @brief  显示单个数字
 * @param  pos: 显示位置
 * @param  num: 要显示的数字（0-9，10为关闭）
 * @retval 无
 */
void Display_ShowDigit(DisplayPos_t pos, unsigned char num);

/**
 * @brief  显示闪烁效果
 * @param  pos: 显示位置
 * @param  enable: 是否启用闪烁
 * @retval 无
 */
void Display_Blink(DisplayPos_t pos, unsigned char enable);

/**
 * @brief  显示状态指示
 * @param  direction: 方向
 * @param  state: 交通灯状态
 * @retval 无
 */
void Display_ShowState(Direction_t direction, LightState_t state);

#endif /* __DISPLAY_H__ */