/**************************************************
 * 文件名:    simple_traffic.c
 * 作者:      
 * 描述:      简化的交通灯控制系统 - C51兼容版本
 **************************************************/

#include <reg52.h>

// LED端口定义  
sbit LED_RED_NS = P1^0;     // 南北向红灯
sbit LED_YELLOW_NS = P1^1;  // 南北向黄灯
sbit LED_GREEN_NS = P1^2;   // 南北向绿灯
sbit LED_RED_EW = P1^3;     // 东西向红灯
sbit LED_YELLOW_EW = P1^4;  // 东西向黄灯
sbit LED_GREEN_EW = P1^5;   // 东西向绿灯

// 按键端口定义
sbit KEY_MODE = P3^0;       // 模式切换按键
sbit KEY_EMERGENCY = P3^1;  // 紧急按键

// 蜂鸣器端口
sbit BUZZER_PIN = P3^4;     // 蜂鸣器控制

// 数码管控制
#define DISPLAY_DATA_PORT P2    // 数码管数据端口
sbit DISPLAY_SEL_A = P2^6;      // 数码管位选A
sbit DISPLAY_SEL_B = P2^7;      // 数码管位选B
sbit DISPLAY_SEL_C = P3^5;      // 数码管位选C

// 状态定义
#define STATE_NS_RED_EW_GREEN   0
#define STATE_NS_RED_EW_YELLOW  1
#define STATE_NS_GREEN_EW_RED   2
#define STATE_NS_YELLOW_EW_RED  3

// 时间参数
#define TIME_RED    30
#define TIME_GREEN  25  
#define TIME_YELLOW 5

// 全局变量
unsigned char currentState = STATE_NS_RED_EW_GREEN;
unsigned char timeLeft = TIME_GREEN;
unsigned char systemMode = 0;  // 0=自动, 1=手动
unsigned char emergencyMode = 0;

// 数码管段码表
unsigned char code digitTable[10] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99,  // 0-4
    0x92, 0x82, 0xF8, 0x80, 0x90   // 5-9
};

/**
 * @brief  延时函数
 */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 123; j++);
    }
}

/**
 * @brief  初始化系统
 */
void System_Init(void)
{
    // 初始化LED端口
    LED_RED_NS = 1;
    LED_YELLOW_NS = 0;
    LED_GREEN_NS = 0;
    LED_RED_EW = 0;
    LED_YELLOW_EW = 0;
    LED_GREEN_EW = 1;
    
    // 初始化数码管
    DISPLAY_DATA_PORT = 0xFF;
    DISPLAY_SEL_A = 0;
    DISPLAY_SEL_B = 0;
    DISPLAY_SEL_C = 0;
    
    // 初始化蜂鸣器
    BUZZER_PIN = 1;
    
    // 设置初始状态
    currentState = STATE_NS_RED_EW_GREEN;
    timeLeft = TIME_GREEN;
}

/**
 * @brief  显示数字
 */
void Display_ShowNumber(unsigned char num)
{
    unsigned char tens, units;
    
    if(num > 99) num = 99;
    
    tens = num / 10;
    units = num % 10;
    
    // 显示十位
    DISPLAY_SEL_A = 0;
    DISPLAY_SEL_B = 0;
    DISPLAY_DATA_PORT = digitTable[tens];
    delay_ms(2);
    
    // 显示个位
    DISPLAY_SEL_A = 1;
    DISPLAY_SEL_B = 0;
    DISPLAY_DATA_PORT = digitTable[units];
    delay_ms(2);
}

/**
 * @brief  设置LED状态
 */
void Traffic_SetLEDs(void)
{
    switch(currentState) {
        case STATE_NS_RED_EW_GREEN:
            LED_RED_NS = 1;
            LED_YELLOW_NS = 0;
            LED_GREEN_NS = 0;
            LED_RED_EW = 0;
            LED_YELLOW_EW = 0;
            LED_GREEN_EW = 1;
            break;
            
        case STATE_NS_RED_EW_YELLOW:
            LED_RED_NS = 1;
            LED_YELLOW_NS = 0;
            LED_GREEN_NS = 0;
            LED_RED_EW = 0;
            LED_YELLOW_EW = 1;
            LED_GREEN_EW = 0;
            break;
            
        case STATE_NS_GREEN_EW_RED:
            LED_RED_NS = 0;
            LED_YELLOW_NS = 0;
            LED_GREEN_NS = 1;
            LED_RED_EW = 1;
            LED_YELLOW_EW = 0;
            LED_GREEN_EW = 0;
            break;
            
        case STATE_NS_YELLOW_EW_RED:
            LED_RED_NS = 0;
            LED_YELLOW_NS = 1;
            LED_GREEN_NS = 0;
            LED_RED_EW = 1;
            LED_YELLOW_EW = 0;
            LED_GREEN_EW = 0;
            break;
    }
}

/**
 * @brief  状态转换
 */
void Traffic_StateTransition(void)
{
    switch(currentState) {
        case STATE_NS_RED_EW_GREEN:
            currentState = STATE_NS_RED_EW_YELLOW;
            timeLeft = TIME_YELLOW;
            break;
            
        case STATE_NS_RED_EW_YELLOW:
            currentState = STATE_NS_GREEN_EW_RED;
            timeLeft = TIME_GREEN;
            break;
            
        case STATE_NS_GREEN_EW_RED:
            currentState = STATE_NS_YELLOW_EW_RED;
            timeLeft = TIME_YELLOW;
            break;
            
        case STATE_NS_YELLOW_EW_RED:
            currentState = STATE_NS_RED_EW_GREEN;
            timeLeft = TIME_GREEN;
            break;
    }
}

/**
 * @brief  按键扫描
 */
void Key_Scan(void)
{
    static unsigned char keyState1 = 1;
    static unsigned char keyState2 = 1;
    
    // 模式切换按键
    if(KEY_MODE == 0 && keyState1 == 1) {
        keyState1 = 0;
        systemMode = !systemMode;
        BUZZER_PIN = 0;
        delay_ms(50);
        BUZZER_PIN = 1;
    } else if(KEY_MODE == 1) {
        keyState1 = 1;
    }
    
    // 紧急按键
    if(KEY_EMERGENCY == 0 && keyState2 == 1) {
        keyState2 = 0;
        emergencyMode = 1;
        timeLeft = timeLeft + 30;  // 延长30秒
        BUZZER_PIN = 0;
        delay_ms(100);
        BUZZER_PIN = 1;
    } else if(KEY_EMERGENCY == 1) {
        keyState2 = 1;
    }
}

/**
 * @brief  主函数
 */
void main(void)
{
    unsigned int counter = 0;
    
    System_Init();
    
    while(1) {
        // 按键扫描
        Key_Scan();
        
        // 显示时间
        Display_ShowNumber(timeLeft);
        
        // 设置LED状态
        Traffic_SetLEDs();
        
        // 时间计数（大约1秒）
        counter++;
        if(counter >= 500) {
            counter = 0;
            
            if(systemMode == 0) {  // 自动模式
                if(timeLeft > 0) {
                    timeLeft--;
                    
                    // 倒计时警告
                    if(timeLeft <= 5 && timeLeft > 0) {
                        BUZZER_PIN = 0;
                        delay_ms(100);
                        BUZZER_PIN = 1;
                    }
                } else {
                    Traffic_StateTransition();
                    emergencyMode = 0;  // 清除紧急模式
                }
            }
        }
        
        delay_ms(1);
    }
}