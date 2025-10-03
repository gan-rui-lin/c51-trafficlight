/**************************************************
 * 文件名:    extension.c
 * 作者:      
 * 日期:      2025-10-03
 * 描述:      扩展功能模块实现文件（精简版本）
 *           只保留基本的温度监控功能
 **************************************************/

#include "extension.h"
#include "timer.h"

/*-----------------------DS18B20温度传感器命令---------------*/
#define DS18B20_SKIP_ROM      0xCC
#define DS18B20_CONVERT_T     0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

/*-----------------------全局变量定义-------------------------*/
static unsigned int currentTemperature = 250;  // 当前温度(25.0度)

/*-----------------------函数实现-----------------------------*/

/*
**
 * @brief  扩展模块初始化
 */
/*
void Extension_Init(void)
{
    // 初始化扩展端口
    DS18B20_DQ = 1;         // DS18B20数据线
    currentTemperature = 250;  // 默认25.0度
    
    // 初始化DS18B20
    DS18B20_Init();
}
*/

/*
**
 * @brief  DS18B20初始化
 */
/*
void DS18B20_Init(void)
{
    // DS18B20初始化序列
    DS18B20_DQ = 0;
    Delay_us(500);      // 复位脉冲
    DS18B20_DQ = 1;
    Delay_us(70);       // 等待存在脉冲
}
*/

/*
**
 * @brief  DS18B20写字节
 */
/*
void DS18B20_WriteByte(unsigned char byte)
{
    unsigned char i;
    
    for(i = 0; i < 8; i++) {
        DS18B20_DQ = 0;
        Delay_us(2);
        DS18B20_DQ = (byte & 0x01);
        Delay_us(60);
        DS18B20_DQ = 1;
        Delay_us(2);
        byte >>= 1;
    }
}
*/

/*
**
 * @brief  DS18B20读字节
 */
/*
unsigned char DS18B20_ReadByte(void)
{
    unsigned char i;
    unsigned char byte;
    
    byte = 0;
    for(i = 0; i < 8; i++) {
        DS18B20_DQ = 0;
        Delay_us(2);
        DS18B20_DQ = 1;
        Delay_us(14);
        if(DS18B20_DQ) {
            byte |= (0x01 << i);
        }
        Delay_us(45);
    }
    return byte;
}
*/

/*
**
 * @brief  读取温度
 */
/*
unsigned int DS18B20_ReadTemperature(void)
{
    unsigned char tempLow, tempHigh;
    int temp;
    unsigned int temperature;
    
    // 启动温度转换
    DS18B20_Init();
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_CONVERT_T);
    
    Delay_ms(750);  // 等待转换完成
    
    // 读取温度数据
    DS18B20_Init();
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_READ_SCRATCHPAD);
    
    tempLow = DS18B20_ReadByte();
    tempHigh = DS18B20_ReadByte();
    
    // 合成温度值（乘以10后的整数）
    temp = (tempHigh << 8) | tempLow;
    temperature = (unsigned int)(temp * 625 / 100);  // DS18B20分辨率为0.0625度，乘以10
    
    currentTemperature = temperature;
    return temperature;
}
*/

/*
**
 * @brief  获取当前温度
 */
/*
unsigned int Extension_GetTemperature(void)
{
    return currentTemperature;
}
*/