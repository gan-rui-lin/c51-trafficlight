/**************************************************
 * 文件名:    extension.h
 * 作者:
 * 日期:      2025-10-03
 * 描述:      扩展功能模块头文件（精简版本）
 *           只保留基本的温度监控功能
 **************************************************/

#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include "config.h"

/*-----------------------函数声明-----------------------------*/
void Extension_Init(void);                   // 扩展模块初始化
void DS18B20_Init(void);                     // DS18B20初始化
void DS18B20_WriteByte(unsigned char);       // DS18B20写字节
unsigned char DS18B20_ReadByte(void);        // DS18B20读字节
unsigned int DS18B20_ReadTemperature(void);  // 读取温度
unsigned int Extension_GetTemperature(void); // 获取当前温度

#endif /* __EXTENSION_H__ */