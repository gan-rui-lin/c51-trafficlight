/**************************************************
 * 文件名:    test_c51_syntax.c
 * 作者:        
 * 描述:      测试C51编译器语法兼容性
 **************************************************/

#include <reg52.h>

// 测试端口定义
#define LED_PIN P1^0

// 测试结构体定义
typedef struct {
    unsigned char a;
    unsigned char b;
    unsigned char c;
} TestStruct_t;

// 测试枚举
typedef enum {
    MODE_OFF = 0,
    MODE_ON = 1
} TestMode_t;

// 全局变量
TestStruct_t testStruct;
TestMode_t testMode = MODE_OFF;

/**
 * @brief  测试函数
 */
void Test_Function(void)
{
    // 测试变量声明在函数开头
    unsigned char localVar;
    unsigned int counter;
    
    localVar = 0;
    counter = 0;
    
    // 测试赋值
    LED_PIN = 1;
    testMode = MODE_ON;
    
    // 测试结构体访问
    testStruct.a = 1;
    testStruct.b = 2;
    testStruct.c = 3;
}

/**
 * @brief  主函数
 */
void main(void)
{
    Test_Function();
    
    while(1) {
        // 空循环
    }
}