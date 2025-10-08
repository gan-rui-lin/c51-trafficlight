# 编译文件清单 - 带延时测试版本

## 📦 必须编译的文件

### C 源文件
```
✅ main.c              - 主程序（含延时测试）
✅ traffic_light.c     - 交通灯控制模块
✅ display.c           - 数码管显示模块
✅ timer.c             - 延时函数模块（新增）
```

### 头文件
```
✅ config.h            - 系统配置
✅ traffic_light.h     - 交通灯模块头文件
✅ display.h           - 显示模块头文件
✅ timer.h             - 延时模块头文件（新增）
```

### 汇编文件
```
✅ STARTUP.A51         - 启动代码
```

---

## 🔧 Keil 项目配置

### 1. 添加新文件到项目

在 Keil 中：
```
右键 "Source Group 1"
→ Add Files to Group
→ 选择 timer.c
```

### 2. 文件依赖关系

```
main.c
├── config.h
├── traffic_light.h
├── display.h
└── timer.h          ← 新增依赖

timer.c
└── timer.h

traffic_light.c
├── config.h
└── traffic_light.h

display.c
├── config.h
└── display.h
```

---

## ⚙️ 编译步骤

### 1. 清理项目
```
Project → Clean Targets
```

### 2. 重新编译
```
Project → Rebuild all target files (F7)
```

### 3. 检查输出
```
Build Output:
compiling main.c...
compiling traffic_light.c...
compiling display.c...
compiling timer.c...         ← 确认新文件已编译
assembling STARTUP.A51...
linking...
".\Objects\..." - 0 Error(s), 0 Warning(s).
```

---

## 📊 代码大小预估

| 模块 | ROM (估计) | RAM (估计) | 说明 |
|------|-----------|-----------|------|
| main.c | ~500 bytes | ~10 bytes | 主程序逻辑 |
| traffic_light.c | ~800 bytes | ~20 bytes | 交通灯控制 |
| display.c | ~400 bytes | ~10 bytes | 显示驱动 |
| timer.c | ~300 bytes | ~5 bytes | 延时函数 |
| **总计** | **~2000 bytes** | **~45 bytes** | 适用 AT89C51 |

---

## ✅ 编译检查清单

编译前确认：
- [ ] timer.c 已添加到项目
- [ ] timer.h 在相同目录
- [ ] main.c 包含 `#include "timer.h"`
- [ ] config.h 中晶振频率为 11.0592MHz
- [ ] 芯片选择为 AT89C51 或 AT89S52

编译成功标志：
- [ ] 0 Error(s), 0 Warning(s)
- [ ] 生成 .hex 文件
- [ ] 代码大小 < 4KB (AT89C51 容量)

---

## 🧪 测试准备

编译成功后：
1. ✅ 将 .hex 文件加载到 ISIS
2. ✅ 运行仿真
3. ✅ 观察启动序列：
   - 数码管显示 "99" (0.5秒)
   - 倒计时 3→2→1→0 (每秒)
   - 所有灯亮 1秒
   - 数码管显示 "88" (0.5秒)
   - 进入正常运行

---

## 🔍 常见编译错误

### 错误1: 找不到 timer.h
```
Error: cannot open source input file "timer.h"
```
**解决**：确保 timer.h 和 main.c 在同一目录

### 错误2: 未定义 Delay_ms
```
Error: undefined symbol 'Delay_ms'
```
**解决**：确保 timer.c 已添加到项目并编译

### 错误3: 未定义 Test_DelayAccuracy
```
Error: undefined symbol 'Test_DelayAccuracy'
```
**解决**：检查 timer.c 中是否实现该函数

### 错误4: 重复定义
```
Error: multiply defined symbol
```
**解决**：检查是否有多个 main() 函数，删除测试文件

---

## 📝 版本说明

**当前版本**: v2.1 - 带延时测试功能
- ✅ 标准延时函数 (timer.c/timer.h)
- ✅ 延时精度测试 (倒计时显示)
- ✅ 硬件自检 (1秒灯亮测试)
- ✅ 系统就绪提示

**上一版本**: v2.0 - 双数码管版本
- 简单软件延时
- 无延时测试功能

---

## 🚀 快速命令

| 操作 | 快捷键 | 菜单路径 |
|------|--------|---------|
| 编译 | F7 | Project → Build Target |
| 清理 | - | Project → Clean Targets |
| 重新编译 | - | Project → Rebuild all |
| 查看输出 | - | View → Output Window |

---

**编译完成后即可进行延时测试！** ⏱️
