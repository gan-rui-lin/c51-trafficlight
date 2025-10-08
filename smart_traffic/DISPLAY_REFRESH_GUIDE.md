# 数码管高速刷新方案

## 问题描述
需要实现：
1. **动态扫描快速**：肉眼看不出两个数码管的切换过程
2. **整体显示持久**：持续显示1秒钟，与定时器时钟同步
3. **无闪烁感**：在1秒内不会出现熄灭或闪烁

## 解决方案

### 方案1：阻塞式刷新（推荐用于简单系统）

在主循环中每秒调用一次 `Display_ShowTime_1s()`：

```c
void main(void)
{
    unsigned char nsTime, ewTime;
    
    System_Init();
    
    while(1)
    {
        // 读取当前时间（需要中断保护）
        EA = 0;
        nsTime = currentNsTime;
        ewTime = currentEwTime;
        EA = 1;
        
        // 持续显示1秒钟（阻塞式）
        Display_ShowTime_1s(nsTime, ewTime);
    }
}
```

**优点**：
- 代码简单，容易理解
- 1秒内刷新200次，显示稳定无闪烁
- 每次刷新约5ms，看不出切换过程

**缺点**：
- 阻塞执行，1秒内无法处理其他任务
- 如果需要按键检测等功能，会有延迟

---

### 方案2：定时器中断刷新（推荐用于复杂系统）

在定时器中断中周期性调用 `Display_ShowTime()`：

```c
// 全局变量
unsigned char currentNsTime = 0;
unsigned char currentEwTime = 0;

// 定时器2中断：10ms刷新一次显示（100Hz）
void Timer2_ISR(void) interrupt 5
{
    TF2 = 0;  // 清除中断标志
    
    // 快速刷新显示（约5ms）
    Display_ShowTime(currentNsTime, currentEwTime);
}

void main(void)
{
    System_Init();
    Timer2_Init();  // 配置为10ms中断
    
    while(1)
    {
        // 主循环可以处理其他任务
        // 显示由定时器中断自动刷新
        
        if (needUpdateTime)
        {
            // 更新时间时禁用中断，避免冲突
            EA = 0;
            currentNsTime = newNsTime;
            currentEwTime = newEwTime;
            EA = 1;
        }
    }
}
```

**优点**：
- 非阻塞，主循环可以处理其他任务
- 刷新频率稳定（100Hz）
- 适合复杂系统（带按键、传感器等）

**缺点**：
- 代码稍复杂，需要配置额外的定时器
- 中断嵌套可能导致问题（需要注意优先级）

---

### 方案3：主循环高速刷新（折中方案）

主循环中连续调用 `Display_ShowTime()`，在需要时更新时间：

```c
void main(void)
{
    unsigned char nsTime = 0;
    unsigned char ewTime = 0;
    unsigned char lastSecond = 0;
    
    System_Init();
    
    while(1)
    {
        // 高速刷新显示（每次约5ms）
        Display_ShowTime(nsTime, ewTime);
        
        // 检查是否到了新的一秒
        if (currentSecond != lastSecond)
        {
            lastSecond = currentSecond;
            
            // 更新时间
            EA = 0;
            nsTime = currentNsTime;
            ewTime = currentEwTime;
            EA = 1;
        }
    }
}
```

**优点**：
- 无需额外定时器
- 刷新频率非常高（约200Hz）
- 主循环可以插入简单任务

**缺点**：
- CPU占用率高
- 不适合有复杂任务的系统

---

## 技术参数

### 刷新时间分析
- **单次刷新时间**：约5ms（包括两个数码管各2ms + 切换时间）
- **最小刷新频率**：50Hz（肉眼不闪烁）
- **推荐刷新频率**：100-200Hz（显示稳定）
- **1秒内刷新次数**：200次（方案1）或100次（方案2）

### 延时参数（11.0592MHz晶振）
```c
// 单个数码管保持时间：1-2ms
for(i=0; i<30; i++)
    for(j=0; j<10; j++);
```

### 动态扫描原理
```
时间轴：|------5ms------|------5ms------|------5ms------|
显示：  NS亮(2ms)→EW亮(2ms)→NS亮(2ms)→EW亮(2ms)→...
人眼感觉：NS和EW同时显示（因为刷新频率>50Hz）
```

---

## 当前实现

### 函数1：Display_ShowTime() - 单次快速刷新
```c
void Display_ShowTime(unsigned char nsTime, unsigned char ewTime);
```
- **功能**：执行一次完整的动态扫描（两个数码管各显示一次）
- **耗时**：约5ms
- **用途**：需要外部循环调用，用于方案2和方案3

### 函数2：Display_ShowTime_1s() - 1秒连续刷新
```c
void Display_ShowTime_1s(unsigned char nsTime, unsigned char ewTime);
```
- **功能**：连续刷新显示1秒钟（内部循环200次）
- **耗时**：1秒（阻塞）
- **用途**：方案1，简单系统每秒调用一次

---

## 使用建议

### 推荐配置
| 系统复杂度 | 推荐方案 | 刷新方式 | 刷新频率 |
|-----------|---------|---------|---------|
| 简单（仅显示） | 方案1 | Display_ShowTime_1s() | 200Hz |
| 中等（带按键） | 方案3 | Display_ShowTime() | 200Hz |
| 复杂（多任务） | 方案2 | Timer2中断 | 100Hz |

### 代码模板（方案1 - 最简单）

```c
#include "config.h"
#include "display.h"
#include "traffic_light.h"

void main(void)
{
    unsigned char nsTime, ewTime;
    
    // 初始化
    Display_Init();
    TrafficLight_Init();
    Timer0_Init();  // 1秒定时器，更新时间
    
    EA = 1;  // 开启总中断
    
    while(1)
    {
        // 读取当前倒计时（带中断保护）
        EA = 0;
        nsTime = currentNsTime;
        ewTime = currentEwTime;
        EA = 1;
        
        // 持续显示1秒（期间连续刷新200次）
        Display_ShowTime_1s(nsTime, ewTime);
    }
}
```

---

## 常见问题

### Q1: 为什么要刷新这么多次？
**A**: 人眼的视觉暂留约16ms（60Hz），刷新频率需要>50Hz才不会感觉闪烁。100-200Hz可以保证显示非常稳定。

### Q2: 单次刷新为什么是5ms？
**A**: 每个数码管显示2ms（30×10循环），两个数码管共4ms，加上切换时间约5ms。

### Q3: 会不会影响Timer0的1秒计时？
**A**: 不会。Timer0在中断中独立运行，不受主循环刷新影响。

### Q4: 如何调整刷新频率？
**A**: 
- **提高频率**：增加 `Display_ShowTime_1s()` 中的循环次数（如300次）
- **降低频率**：减少循环次数（如100次）
- **注意**：频率<50Hz会闪烁，频率>300Hz浪费CPU

### Q5: 能否用Timer1中断刷新？
**A**: 可以，但要注意：
- Timer0已用于1秒计时
- Timer1可配置为10ms中断
- 需要注意中断优先级，避免冲突

---

## 测试方法

### 测试1：验证刷新频率
```c
// 在 Display_ShowTime_1s() 中临时添加LED闪烁
sbit LED = P3^6;
LED = !LED;  // 应该看到LED快速闪烁（肉眼看不清）
```

### 测试2：验证显示稳定性
```c
// 显示固定值，观察是否闪烁
Display_ShowTime_1s(8, 8);  // 应该看到稳定的"88"
```

### 测试3：验证时间同步
```c
// 配合Timer0，验证1秒显示
// 应该看到数字每秒变化一次，中途不闪烁
```

---

## 总结

- ✅ **方案1（阻塞式）**：最简单，适合纯显示系统
- ✅ **方案2（中断式）**：最灵活，适合多任务系统
- ✅ **方案3（循环式）**：折中方案，适合中等复杂度

**当前推荐**：先用方案1测试，确认显示效果后再考虑是否需要升级到方案2。
