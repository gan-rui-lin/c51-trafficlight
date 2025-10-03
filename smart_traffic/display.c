/**************************************************
 * 文件名:    display.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      显示模块实现文件（精简版本）
 *           只保留基本的显示功能
 **************************************************/

#include "display.h"
#include "config.h"
#include "timer.h"

/*-----------------------数码管显示表-------------------------*/
static const unsigned char segmentTable[] = {
    0x3F,   // 0
    0x06,   // 1
    0x5B,   // 2
    0x4F,   // 3
    0x66,   // 4
    0x6D,   // 5
    0x7D,   // 6
    0x07,   // 7
    0x7F,   // 8
    0x6F    // 9
};

/*-----------------------函数实现-----------------------------*/

/**
 * @brief  显示模块初始化
 */
void Display_Init(void)
{
    // 初始化显示端口
    DISPLAY_DATA_PORT = 0x00;    // 清除显示数据
    DISPLAY_SEL_A = 0;           // 关闭所有位选
    DISPLAY_SEL_B = 0;
    DISPLAY_SEL_C = 0;
}

/**
 * @brief  显示两位数字
 */
void Display_ShowTime(unsigned char nsTime, unsigned char ewTime)
{
    unsigned char digit;
    
    // 显示南北方向时间（左两位）
    DISPLAY_SEL_A = 1;
    digit = nsTime / 10;
    if (digit > 9) digit = 9;
    DISPLAY_DATA_PORT = segmentTable[digit];
    Delay_ms(1);
    DISPLAY_SEL_A = 0;
    
    DISPLAY_SEL_B = 1;
    digit = nsTime % 10;
    DISPLAY_DATA_PORT = segmentTable[digit];
    Delay_ms(1);
    DISPLAY_SEL_B = 0;
    
    // 显示东西方向时间（右两位）
    DISPLAY_SEL_C = 1;
    digit = ewTime / 10;
    if (digit > 9) digit = 9;
    DISPLAY_DATA_PORT = segmentTable[digit];
    Delay_ms(1);
    DISPLAY_SEL_C = 0;
}

