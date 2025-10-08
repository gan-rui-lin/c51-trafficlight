/**************************************************
 * 文件名:    display.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      显示模块实现文件（精简版本）
 *           只保留基本的显示功能
 **************************************************/

#include "display.h"
#include "config.h"
// #include "timer.h"

/*-----------------------数码管显示表-------------------------*/
// 共阳极数码管段码表（原共阴极段码取反）
static const unsigned char segmentTable[] = {
    ~0x3F,   // 0 → 0xC0
    ~0x06,   // 1 → 0xF9
    ~0x5B,   // 2 → 0xA4
    ~0x4F,   // 3 → 0xB0
    ~0x66,   // 4 → 0x99
    ~0x6D,   // 5 → 0x92
    ~0x7D,   // 6 → 0x82
    ~0x07,   // 7 → 0xF8
    ~0x7F,   // 8 → 0x80
    ~0x6F    // 9 → 0x90
};

/* 如果是共阴极数码管，请恢复原来的段码：
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
*/

/*-----------------------函数实现-----------------------------*/

/**
 * @brief  显示模块初始化
 */
void Display_Init(void)
{
    // 初始化显示端口（共阳极数码管用0xFF消隐）
    DISPLAY_DATA_PORT = 0xFF;    // 消隐显示（共阳极）
    DISPLAY_SEL_A = 0;           // 关闭所有位选
    DISPLAY_SEL_B = 0;           // 2-4译码器只需要2个地址位
}

/**
 * @brief  显示两位数字（使用2-4译码器动态扫描） - 单次刷新版本
 * @param  nsTime: 南北方向剩余时间 (0-9)
 * @param  ewTime: 东西方向剩余时间 (0-9)
 * @note   使用74HC139译码器，只使用Y0和Y1两个输出
 *         数码管0(Y0): 显示 nsTime（南北方向）
 *         数码管1(Y1): 显示 ewTime（东西方向）
 *         已适配共阳极数码管（段码已取反）
 *         
 *         【重要】此函数仅执行一次快速扫描，需要外部循环调用
 *         推荐调用频率：100Hz以上（每秒100次）
 */
void Display_ShowTime(unsigned char nsTime, unsigned char ewTime)
{
    unsigned int i, j;
    
    // 限制输入范围 (0-9)
    if (nsTime > 9) nsTime = 9;
    if (ewTime > 9) ewTime = 9;
    
    // ========== 显示第0位：南北方向时间 ==========
    // 地址: BA = 00 → 选中Y0
    DISPLAY_DATA_PORT = 0xFF;       // 【关键】先消隐（共阳极用0xFF）
    DISPLAY_SEL_A = 0;              // A=0
    DISPLAY_SEL_B = 0;              // B=0
    
    DISPLAY_DATA_PORT = segmentTable[nsTime];  // 送入段码
    
    // 保持时间：约1-2ms（根据晶振频率调整）
    for(i=0; i<6; i++)
        for(j=0; j<10; j++);

    // ========== 显示第1位：东西方向时间 ==========
    // 地址: BA = 01 → 选中Y1
    DISPLAY_DATA_PORT = 0xFF;       // 【关键】先消隐（共阳极用0xFF）
    DISPLAY_SEL_A = 1;              // A=1
    DISPLAY_SEL_B = 0;              // B=0
    
    DISPLAY_DATA_PORT = segmentTable[ewTime];  // 送入段码
    
    // 保持时间：约1-2ms
    for(i=0; i<30; i++)
        for(j=0; j<10; j++);
}

/**
 * @brief  持续显示1秒钟（高速刷新版本）
 * @param  nsTime: 南北方向剩余时间 (0-9)
 * @param  ewTime: 东西方向剩余时间 (0-9)
 * @note   此函数会持续刷新显示1秒钟，期间不会闪烁
 *         刷新频率：约200次/秒（每次刷新耗时约5ms）
 *         
 *         【调用方式】
 *         方式1：在主循环中每秒调用一次（阻塞式）
 *         方式2：在定时器中断中周期性调用 Display_ShowTime()（非阻塞）
 */
void Display_ShowTime_1s(unsigned char nsTime, unsigned char ewTime)
{
    unsigned int refresh_count;
    
    // 1秒 = 200次刷新 × 5ms/次
    for(refresh_count = 0; refresh_count < 200; refresh_count++)
    {
        Display_ShowTime(nsTime, ewTime);
    }
}

