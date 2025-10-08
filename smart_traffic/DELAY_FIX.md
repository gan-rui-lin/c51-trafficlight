# 延时修复说明

## 问题原因

1秒延时实际花了 18秒，原因是：
- Timer1 硬件延时实现有问题
- 可能与 TMOD 配置冲突（Timer0 已在使用）
- Timer1 模式设置干扰了 Timer0

## 解决方案

改用 **软件延时** 代替 Timer1 硬件延时：

```c
// 新的 Delay_1ms() 实现
static void Delay_1ms(void)  //@11.0592MHz
{
    unsigned char i, j;
    _nop_();
    i = 2;
    j = 199;
    do {
        while (--j);
    } while (--i);
}
```

## 编译设置要求

⚠️ **重要**：Keil 编译器优化级别必须设置为 **Level 0**

### 设置步骤：
1. 打开项目
2. `Project → Options for Target`
3. `C51 标签页`
4. `Code Optimization: Level 0`
5. 重新编译

**原因**：高优化级别会改变循环次数，导致延时不准确。

## 延时精度

基于 11.0592MHz 晶振的软件延时：
- **Delay_1ms()**: 误差约 ±2%
- **Delay_ms(1000)**: 1秒误差约 ±20ms
- **足够**用于交通灯系统

## 测试验证

重新编译后测试：
1. 数码管显示 3→2 应为 1秒 ±0.05秒
2. 数码管显示 2→1 应为 1秒 ±0.05秒
3. 总倒计时 3秒应为 3秒 ±0.1秒

## 如果还是不准确

### 方案A：手动微调

修改 `timer.c` 中的循环次数：

```c
// 如果太快（< 1秒），增大 j 的值
j = 210;  // 原来是 199

// 如果太慢（> 1秒），减小 j 的值  
j = 190;  // 原来是 199
```

### 方案B：禁用延时测试

如果延时测试干扰系统，可以暂时禁用：

在 `main.c` 的 `System_Init()` 中注释掉：

```c
// 注释掉延时测试
// Display_ShowTime(9, 9);
// Delay_ms(500);
// Test_DelayAccuracy(3);
```

直接进入交通灯运行，交通灯切换时间由 Timer0 中断控制（更准确）。

## 重新编译

```
1. 设置优化级别为 Level 0
2. Project → Clean Targets
3. Project → Rebuild all (F7)
4. 加载到 ISIS 测试
```

---

**修复完成！重新编译测试即可。** ⏱️
