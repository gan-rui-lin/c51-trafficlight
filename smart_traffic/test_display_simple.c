/**************************************************
 * 文件名:    test_display_simple.c
 * 作者:      
 * 日期:      2025-10-08
 * 描述:      简单的数码管测试程序
 *           用于诊断两个数码管显示相同问题
 **************************************************/

#include <reg52.h>
#include <intrins.h>

// 引脚定义
#define DISPLAY_DATA_PORT P1
sbit DISPLAY_SEL_A = P2^6;
sbit DISPLAY_SEL_B = P2^7;

// 共阳极数码管段码表
unsigned char code SegmentCode[] = {
    ~0x3F, ~0x06, ~0x5B, ~0x4F, ~0x66,
    ~0x6D, ~0x7D, ~0x07, ~0x7F, ~0x6F
};

// 延时函数
void Delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        _nop_();
        j = 2;
        while(j--) {
            for(i = 0; i < 199; i++);
        }
    }
}

/**
 * @brief  测试函数：显示两个不同的数字
 * @param  left: 左边数码管显示的数字
 * @param  right: 右边数码管显示的数字
 */
void Test_TwoDigits(unsigned char left, unsigned char right)
{
    unsigned char i;
    
    // 连续刷新200次（约2秒）
    for(i = 0; i < 200; i++) {
        // 显示左边数码管
        DISPLAY_DATA_PORT = 0xFF;   // 先消隐
        DISPLAY_SEL_A = 0;          // BA = 00
        DISPLAY_SEL_B = 0;
        DISPLAY_DATA_PORT = SegmentCode[left];
        Delay_ms(5);
        
        // 显示右边数码管
        DISPLAY_DATA_PORT = 0xFF;   // 先消隐
        DISPLAY_SEL_A = 1;          // BA = 01
        DISPLAY_SEL_B = 0;
        DISPLAY_DATA_PORT = SegmentCode[right];
        Delay_ms(5);
    }
}

void main(void)
{
    // 初始化
    DISPLAY_DATA_PORT = 0xFF;
    DISPLAY_SEL_A = 0;
    DISPLAY_SEL_B = 0;
    
    Delay_ms(500);
    
    while(1) {
        // 测试1：显示 "12"
        Test_TwoDigits(1, 2);
        
        // 测试2：显示 "34"
        Test_TwoDigits(3, 4);
        
        // 测试3：显示 "56"
        Test_TwoDigits(5, 6);
        
        // 测试4：显示 "78"
        Test_TwoDigits(7, 8);
    }
}

/*
 * ============================================================
 *                    测试说明
 * ============================================================
 * 
 * 预期结果：
 * - 应该看到数码管循环显示：12 → 34 → 56 → 78
 * - 左边和右边数码管显示不同的数字
 * 
 * 如果看到相同数字（11、33、55、77）：
 * 
 * 【问题1】硬件接线错误
 * - 74HC139 的 Y0 和 Y1 可能接在一起
 * - 或者只有一个数码管连接正确
 * 
 * 解决方法：
 * 1. 检查 74HC139 pin4 (Y0) 是否连接到左边数码管
 * 2. 检查 74HC139 pin5 (Y1) 是否连接到右边数码管
 * 3. 确认 P2.6 连接到 74HC139 pin2 (A1)
 * 
 * 【问题2】P2.6 地址线不工作
 * - P2.6 引脚可能被其他功能占用
 * - P2.6 没有正确连接到 74HC139
 * 
 * 解决方法：
 * 1. 用万用表测量 P2.6 是否有电平变化
 * 2. 测量 74HC139 pin2 是否收到信号
 * 3. 尝试更换 P2.6 为其他引脚（如 P2.4）
 * 
 * 【调试技巧】
 * 可以在代码中添加 LED 指示：
 * 
 * sbit LED = P3^6;  // 添加LED
 * 
 * // 在 Test_TwoDigits 中：
 * LED = DISPLAY_SEL_A;  // LED应该闪烁
 * 
 * ============================================================
 */
