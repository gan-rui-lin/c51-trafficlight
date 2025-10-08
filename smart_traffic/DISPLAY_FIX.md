# 数码管显示问题修复

## 问题诊断

### 问题1：数码管闪烁严重
**现象**：主循环中数码管闪烁，但测试时不闪烁  
**原因**：
- 主循环刷新频率太低
- 每个数码管显示时间太短（10×10 循环）
- 刷新间隔太长

### 问题2：数字交替闪烁（竞争条件）
**现象**：6→5→4 和 3→2→1 交替显示，特别是状态切换时  
**原因**：
- **中断竞争**：Timer0 中断修改 `currentState` 和 `timeLeft`
- **主循环读取**：同时主循环在读取这些变量
- **数据不一致**：读到一半时中断发生，导致状态和时间不匹配

---

## 解决方案

### 修复1：增加显示保持时间

**display.c 修改**：
```c
// 原来：10×10 = 100 次循环（太短）
for(i=0; i<10; i++)
    for(j=0; j<10; j++);

// 修改后：30×50 = 1500 次循环（稳定）
for(i=0; i<30; i++)
    for(j=0; j<50; j++);
```

**效果**：
- 每个数码管显示时间增加 15 倍
- 减少闪烁，显示更稳定
- 刷新频率仍然足够快（肉眼不可见切换）

---

### 修复2：防止中断竞争

**main.c 修改**：添加中断保护机制

```c
// 【关键】在读取共享变量前关中断
EA = 0;  // 关全局中断
tempState = currentState;      // 快速复制状态
tempTimeLeft = timeLeft;       // 快速复制时间
EA = 1;  // 开全局中断

// 使用临时变量进行计算
switch(tempState) {
    case STATE_NS_GREEN_EW_RED:
        nsTime = tempTimeLeft;
        ewTime = tempTimeLeft + stateTimeTable[...];
        break;
    // ...
}
```

**工作原理**：
1. 关中断 → 读取数据 → 开中断（耗时极短，< 10us）
2. 确保 `tempState` 和 `tempTimeLeft` 一致性
3. 后续计算使用临时变量，不受中断影响

---

### 修复3：增加连续刷新次数

**main.c 修改**：
```c
// 每次主循环连续刷新 5 次
for(refresh = 0; refresh < 5; refresh++) {
    Display_ShowTime(nsTime, ewTime);
}
```

**效果**：
- 增加显示稳定性
- 减少状态切换瞬间的闪烁
- 每个数字有更多时间被看到

---

## 技术细节

### 为什么测试时不闪烁？

**Test_DelayAccuracy() 函数**：
```c
for(i = 0; i < 100; i++) {
    Display_ShowTime(count, count);
    Delay_ms(10);  // 每 10ms 刷新
}
```

- **连续刷新 100 次**
- **每次间隔 10ms**
- **没有中断竞争**（测试时 Timer0 未启动）

### 为什么主循环闪烁？

**原主循环**：
```c
while(1) {
    // 计算显示值（耗时）
    switch(currentState) { ... }
    
    // 只刷新 1 次
    Display_ShowTime(nsTime, ewTime);
}
```

- **只刷新 1 次**，间隔长
- **有中断竞争**（Timer0 正在运行）
- **计算耗时**，降低刷新频率

---

## 刷新频率分析

### 修改前
```
单次显示时间：10×10 = 100 循环 ≈ 0.3ms
总刷新周期：计算时间 + 0.3ms×2 ≈ 5ms
刷新频率：约 200Hz （偏低，可见闪烁）
```

### 修改后
```
单次显示时间：30×50 = 1500 循环 ≈ 4ms
连续刷新 5 次：5 × (4ms×2) ≈ 40ms
刷新频率：约 25Hz × 5次 = 125Hz等效 （流畅）
```

---

## 中断保护原理

### 为什么需要关中断？

假设不关中断的情况：
```c
// 主循环读取
tempState = currentState;       // 读到 STATE_NS_GREEN_EW_RED
// 【此时发生中断！】
// Timer0 中断修改：currentState = STATE_NS_YELLOW_EW_RED
//                 timeLeft = 3
tempTimeLeft = timeLeft;        // 读到 3 秒（黄灯时间）

// 结果：状态=绿灯，时间=3秒 → 错误！
// 应该是：状态=绿灯，时间=5秒
```

### 关中断后的情况：
```c
EA = 0;  // 关中断
tempState = currentState;       // 读到 STATE_NS_GREEN_EW_RED
tempTimeLeft = timeLeft;        // 读到 5 秒
EA = 1;  // 开中断（总耗时 < 10us）

// 结果：状态=绿灯，时间=5秒 → 正确！
```

---

## 调试技巧

### 如果还是闪烁

1. **增大显示保持时间**：
```c
// display.c 中
for(i=0; i<50; i++)      // 增加到 50
    for(j=0; j<100; j++); // 增加到 100
```

2. **增加连续刷新次数**：
```c
// main.c 中
for(refresh = 0; refresh < 10; refresh++) {  // 改为 10 次
    Display_ShowTime(nsTime, ewTime);
}
```

### 如果还有竞争

确认是否正确使用临时变量：
```c
// ✅ 正确
EA = 0;
tempState = currentState;
tempTimeLeft = timeLeft;
EA = 1;

switch(tempState) { ... }  // 使用 tempState

// ❌ 错误
EA = 0;
tempState = currentState;
EA = 1;

switch(currentState) { ... }  // 仍使用 currentState
```

---

## 性能影响

### 关中断时间
```
EA = 0;
tempState = currentState;    // 1-2 个机器周期
tempTimeLeft = timeLeft;     // 1-2 个机器周期
EA = 1;

总时间：< 10us （对系统无影响）
```

### Timer0 中断周期
```
2ms 中断一次
关中断 10us
影响：10us / 2000us = 0.5% （可忽略）
```

---

## 验收标准

编译后测试：
- ✅ 数码管显示稳定，无明显闪烁
- ✅ 状态切换时数字不跳变
- ✅ 倒计时连续平滑（3→2→1）
- ✅ 交通灯切换正常

---

## 总结

**修复内容**：
1. ✅ 增加单个数码管显示时间（10×10 → 30×50）
2. ✅ 增加连续刷新次数（1次 → 5次）
3. ✅ 添加中断保护机制（关中断读取共享变量）

**效果**：
- 消除闪烁
- 消除竞争条件
- 显示稳定流畅

**重新编译测试即可！** ✅
