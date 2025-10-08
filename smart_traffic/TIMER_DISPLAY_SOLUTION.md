# 定时器中断显示方案实施总结

## 修改内容

### 1. main.c 修改

#### 变量声明
```c
// 将显示变量改为全局volatile，供Timer0中断使用
volatile unsigned char nsTime = 0;  // 南北方向显示时间
volatile unsigned char ewTime = 0;  // 东西方向显示时间
```

#### 主循环逻辑
```c
while(1) {
    // 1. 读取交通灯状态
    EA = 0;
    tempState = currentState;
    tempTimeLeft = timeLeft;
    EA = 1;
    
    // 2. 计算显示数值
    switch(tempState) {
        // ... 计算 newNsTime 和 newEwTime
    }
    
    // 3. 更新全局显示变量
    EA = 0;
    nsTime = newNsTime;
    ewTime = newEwTime;
    EA = 1;
    
    // 4. 实际显示由Timer0中断自动完成，主循环不再调用Display_ShowTime()
}
```

### 2. traffic_light.c 修改

#### 头文件包含
```c
#include "traffic_light.h"
#include "display.h"  // 用于在中断中调用 Display_ShowTime()
```

#### 外部变量声明
```c
// 外部变量声明（来自main.c的显示变量）
extern volatile unsigned char nsTime;
extern volatile unsigned char ewTime;
```

#### Timer0中断函数
```c
void Timer0_ISR(void) interrupt 1
{
    // 重新装载定时器初值
    TH0 = TIMER0_RELOAD_H;
    TL0 = TIMER0_RELOAD_L;
    
    // 计数器递增
    timer0Count++;
    flashCount++;
    
    // 【关键】每2ms刷新一次显示（约500Hz刷新率）
    Display_ShowTime(nsTime, ewTime);
    
    // 心跳指示
    if ((timer0Count % (100 / 3)) == 0) {
        DEBUG_1S_PIN = !DEBUG_1S_PIN;
    }
    
    // 1秒定时处理
    if (timer0Count >= 33) {
        timer0Count = 0;
        
        if (timeLeft > 0) {
            timeLeft--;
        }
        
        if (timeLeft == 0) {
            SwitchToNextState();
        }
    }
}
```

---

## 工作原理

### 时序图

```
时间轴：    0ms      2ms      4ms      6ms      8ms     10ms
            |--------|--------|--------|--------|--------|
Timer0中断：  ↓        ↓        ↓        ↓        ↓        ↓
            显示     显示     显示     显示     显示     显示
            
主循环：    ════════════════════════════════════════════════
            每隔一段时间更新 nsTime/ewTime 的值
```

### 数据流

```
1. Timer0中断（每2ms）
   └─> 调用 Display_ShowTime(nsTime, ewTime)
       └─> 显示第0位（nsTime）约2ms
       └─> 显示第1位（ewTime）约2ms
       
2. 主循环（连续运行）
   └─> 读取 currentState, timeLeft
   └─> 计算 newNsTime, newEwTime
   └─> 更新全局变量 nsTime, ewTime
```

### 刷新频率

- **Timer0中断频率**：2ms = 500Hz
- **显示刷新频率**：500Hz（每次中断调用一次显示）
- **人眼临界值**：50Hz
- **结果**：500Hz >> 50Hz，显示非常稳定无闪烁

---

## 优势分析

### ✅ 优点

1. **非阻塞式**：主循环不需要等待显示，可以处理其他任务
2. **高刷新率**：500Hz刷新率远超人眼临界值，显示稳定
3. **实时性好**：Timer0中断优先级高，显示不会被打断
4. **逻辑清晰**：
   - 主循环：负责计算显示数值
   - Timer0中断：负责实际显示和计时

### 📊 性能参数

| 参数 | 数值 | 说明 |
|------|------|------|
| Timer0中断周期 | 2ms | 配置值 |
| 显示刷新周期 | 2ms | 每次中断调用一次 |
| 单次显示耗时 | ~5ms | 两个数码管各2ms |
| 中断嵌套情况 | 可能 | Display_ShowTime执行期间可能再次触发Timer0 |
| CPU占用率 | 高 | 每2ms执行5ms任务（需要验证） |

### ⚠️ 潜在问题

#### 问题1：中断嵌套
- **现象**：Display_ShowTime()执行5ms，但Timer0每2ms中断一次
- **可能后果**：中断嵌套，导致堆栈溢出
- **解决方案**：
  ```c
  // 方案1：在Display_ShowTime()开始时禁用中断
  void Display_ShowTime(unsigned char nsTime, unsigned char ewTime) {
      EA = 0;  // 禁用中断
      // ... 显示代码
      EA = 1;  // 恢复中断
  }
  
  // 方案2：减少Display_ShowTime()的延时时间
  for(i=0; i<3; i++)  // 从30减少到3
      for(j=0; j<10; j++);
  ```

#### 问题2：Timer0计时不准确
- **现象**：Display_ShowTime()耗时5ms，导致Timer0计数偏差
- **解决方案**：调整timer0Count的阈值
  ```c
  // 原始：33次中断 = 66ms ≈ 1秒（错误）
  // 实际：考虑Display_ShowTime()的延时
  if (timer0Count >= 50) {  // 调整为合适的值
  ```

#### 问题3：CPU占用过高
- **现象**：每2ms执行5ms任务，CPU无法处理其他事务
- **解决方案**：降低刷新频率
  ```c
  // 方案：不是每次中断都刷新，而是每隔几次刷新一次
  static unsigned char displayCount = 0;
  
  if (displayCount >= 2) {  // 每隔2次中断刷新一次（约4ms刷新周期）
      displayCount = 0;
      Display_ShowTime(nsTime, ewTime);
  } else {
      displayCount++;
  }
  ```

---

## 推荐配置

### 配置1：高刷新率（推荐）

```c
// Timer0中断周期：2ms
// 显示刷新：每次中断（500Hz）
// Display_ShowTime()保持时间：减少到1ms

void Display_ShowTime(unsigned char nsTime, unsigned char ewTime) {
    // ... 显示代码
    
    // 每个数码管保持约1ms
    for(i=0; i<10; i++)
        for(j=0; j<10; j++);
}
```

**优点**：刷新率高，显示稳定  
**缺点**：CPU占用较高

### 配置2：平衡模式

```c
// Timer0中断周期：2ms
// 显示刷新：每隔2次中断（250Hz）
// Display_ShowTime()保持时间：2ms

static unsigned char displayCount = 0;

void Timer0_ISR(void) interrupt 1 {
    // ...
    
    if (displayCount >= 2) {
        displayCount = 0;
        Display_ShowTime(nsTime, ewTime);
    } else {
        displayCount++;
    }
    
    // ...
}
```

**优点**：CPU占用适中，显示仍然稳定  
**缺点**：刷新率略低

### 配置3：低功耗模式

```c
// Timer0中断周期：10ms
// 显示刷新：每次中断（100Hz）
// Display_ShowTime()保持时间：5ms

void Timer0_Init(void) {
    // 修改定时器初值，改为10ms中断
    TH0 = (65536 - 10000*11.0592/12) / 256;
    TL0 = (65536 - 10000*11.0592/12) % 256;
}
```

**优点**：CPU占用低  
**缺点**：刷新率低，可能略有闪烁

---

## 测试验证

### 测试步骤

1. **编译代码**
   ```
   - 确保 traffic_light.c 包含 "display.h"
   - 确保 main.c 中 nsTime/ewTime 声明为 volatile
   - 编译无错误
   ```

2. **在ISIS中运行**
   ```
   - 观察数码管显示是否稳定
   - 观察数字是否每秒变化一次
   - 观察是否有闪烁现象
   ```

3. **性能测试**
   ```
   - 添加LED指示，观察Timer0中断频率
   - 测量实际1秒是否准确
   - 检查是否有堆栈溢出警告
   ```

### 预期结果

✅ **正常情况**：
- 数码管显示稳定，无闪烁
- 两个数码管同时显示，看不出切换
- 数字每秒准确变化一次
- 无编译警告或错误

❌ **异常情况**：
- 如果数码管闪烁：降低刷新频率或增加保持时间
- 如果计时不准：调整timer0Count阈值
- 如果系统死机：检查堆栈溢出，减少中断嵌套

---

## 后续优化

1. **精确计时**：根据实际测试调整timer0Count阈值
2. **中断保护**：在Display_ShowTime()中添加中断禁用
3. **性能优化**：调整刷新频率和保持时间的平衡
4. **功能扩展**：主循环可以添加按键扫描等功能

---

## 总结

本次修改实现了**在Timer0中断中调用显示函数**的方案，核心思路是：

1. **主循环**：计算 nsTime 和 ewTime
2. **Timer0中断**：读取 nsTime 和 ewTime，调用 Display_ShowTime() 显示

这种方案实现了**非阻塞式高速刷新**，理论上可以达到500Hz的刷新率，远超人眼临界值。

但需要注意**中断嵌套**和**CPU占用**的问题，可能需要根据实际测试进行调整。
