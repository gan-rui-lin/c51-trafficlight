/**************************************************
 * 文件名:    main.c
 * 作者:        
 * 日期:      2025-10-08
 * 描述:      智能交通灯系统 - 正式版本
 *           使用2个数码管显示南北和东西方向剩余时间
 *           基于74HC139译码器和共阳极数码管
 * 
 * 【新增功能说明 - 三个按键设置颜色时间】
 *  按键：
 *   - KEY_SET_MODE: 进入/切换/退出设置模式（循环：正常→红→黄→绿→退出）
 *   - KEY_UP: 在当前选中颜色下增加时间
 *   - KEY_DOWN: 在当前选中颜色下减少时间
 *  显示：
 *   - 设置模式暂停倒计时与状态切换
 *   - 两位数码管显示当前选中颜色的时间：左=十位 右=个位
 *   - 选中颜色对应的两个方向同色灯常亮指示（红/黄/绿）
 *  规则：
 *   - 红灯时间 = 绿灯时间 + 黄灯时间 （保持对称路口逻辑）
 *   - 调整“红”时实际操作的是绿灯时间（简化硬件按键数量）
 *   - 范围：MIN_LIGHT_TIME..MAX_LIGHT_TIME
 *   - 退出后立即应用新配时间表，若当前状态剩余时间超过新设定则截断
 *  假设：
 *   - 按键为上拉输入，按下=0
 *   - 消抖使用简单计数法满足实验板需求
 **************************************************/

#include "config.h"
#include "traffic_light.h"
#include "display.h"
#include "timer.h"

extern unsigned char currentState;
extern unsigned char timeLeft;
extern unsigned char isFlashing;
extern unsigned char stateTimeTable[4];


/*==============================================
 *                全局变量定义
 *==============================================*/

// 显示刷新变量（供Timer0中断使用，必须是全局变量）
volatile unsigned char nsTime = 0;  // 南北方向显示时间
volatile unsigned char ewTime = 0;  // 东西方向显示时间

// ===================== 设置模式相关全局变量 =====================
volatile unsigned char g_isSettingMode = 0;      // 1=进入设置暂停倒计时
volatile unsigned char g_selectedColor = 0;      // 0=红 1=黄 2=绿
volatile unsigned char g_time_red = DEFAULT_RED_TIME;      // 红灯=绿+黄（显示/修改时单独存便于显示）
volatile unsigned char g_time_yellow = DEFAULT_YELLOW_TIME;
volatile unsigned char g_time_green = DEFAULT_GREEN_TIME;

// 函数原型（在 traffic_light.c 中实现）
void UpdateStateTimeTable(void);
void ShowSettingColorLights(void);

// 按键扫描函数原型
static void Keys_Scan(void);

/*==============================================
 *                系统初始化
 *==============================================*/
/**
 * @brief  系统初始化
 * @param  无
 * @retval 无
 */
void System_Init(void)
{
    // 初始化所有引脚为低电平
    DEBUG_1S_PIN = 0;    DEBUG_STATE_PIN = 0;
    NS_RED_PIN = 0;      NS_YELLOW_PIN = 0;    NS_GREEN_PIN = 0;
    EW_RED_PIN = 0;      EW_YELLOW_PIN = 0;    EW_GREEN_PIN = 0;
    
    // 初始化显示模块（先初始化，用于延时测试）
    Display_Init();
    
    // // 测试：左边显示1，右边显示2
    // Display_ShowTime(1, 2);
    // Delay_ms(3000);
    
    // ==========================================
    // 延时测试：通过数码管倒计时验证延时准确性
    // ==========================================
    // // 显示 "99" 提示即将开始测试
    // Display_ShowTime(9, 9);
    // Delay_ms(500);
    
    // // 测试3秒延时（数码管显示 3→2→1→0）
    // Test_DelayAccuracy(3);
    
    // ==========================================
    // 硬件自检：所有灯亮1秒
    // ==========================================
    // NS_RED_PIN = 1;      NS_YELLOW_PIN = 1;    NS_GREEN_PIN = 1;
    // EW_RED_PIN = 1;      EW_YELLOW_PIN = 1;    EW_GREEN_PIN = 1;
    // DEBUG_1S_PIN = 1;    DEBUG_STATE_PIN = 1;
    
    // 使用标准延时函数
    // Delay_ms(1000);  // 精确延时1秒
    
    // 关闭所有灯，准备正常工作
    // NS_RED_PIN = 0;      NS_YELLOW_PIN = 0;    NS_GREEN_PIN = 0;
    // EW_RED_PIN = 0;      EW_YELLOW_PIN = 0;    EW_GREEN_PIN = 0;
    // DEBUG_1S_PIN = 0;    DEBUG_STATE_PIN = 0;
    
    // 初始化系统状态
    currentState = STATE_NS_GREEN_EW_RED;
    timeLeft = stateTimeTable[currentState];
    isFlashing = 0;
    
    // 设置初始交通灯状态
    SetTrafficLights(currentState);
    
    // 初始化定时器（这将启动整个系统）
    Timer0_Init();
    
    // 延时测试完成，显示 "88" 表示系统准备就绪
    Display_ShowTime(8, 8);
    Delay_ms(1000);
}

/*==============================================
 *             按键扫描与设置逻辑
 *  硬件假设：
 *  - KEY_SET_MODE: 循环进入设置/切换颜色/退出
 *    流程：正常→按一次进入(选红)→再按(选黄)→再按(选绿)→再按退出保存
 *  - KEY_UP: 当前颜色时间+1 (限制 MIN_LIGHT_TIME..MAX_LIGHT_TIME)
 *  - KEY_DOWN: 当前颜色时间-1
 *  - 设置时倒计时暂停，数码管显示当前颜色时间（十位=高位，个位=低位）
 *  - 红灯时间 = 绿 + 黄 自动更新，不单独可调（显示时仍可在红模式显示组合结果）
 */




#define ReadKey(key)   (key)

static void Keys_Scan(void)
{
    static unsigned char lastSet = 1, lastUp = 1, lastDown = 1;
    static unsigned int debounceCnt = 0;
    unsigned char curSet = ReadKey(KEY_SET_MODE);
    unsigned char curUp = ReadKey(KEY_UP);
    unsigned char curDown = ReadKey(KEY_DOWN);

    // 简单消抖：每次调用间隔>~0.5ms（主循环很快），通过计数限制处理频率
    if(debounceCnt < 200) debounceCnt++; // 约几毫秒

    // SET 键：下降沿
    if(lastSet == 1 && curSet == 0 && debounceCnt > 5) {
        debounceCnt = 0;
        if(!g_isSettingMode) {
            g_isSettingMode = 1; // 进入设置
            g_selectedColor = 0; // 先红
            // 红=绿+黄
            g_time_red = g_time_green + g_time_yellow;
            ShowSettingColorLights();
        } else {
            // 在设置模式中循环：红->黄->绿->退出
            g_selectedColor++;
            if(g_selectedColor >= 3) {
                // 退出，保存：更新状态时间表，重新计算当前状态剩余时间（保持不变，只更新后续）
                g_isSettingMode = 0;
                UpdateStateTimeTable();
                SetTrafficLights(currentState); // 恢复当前状态灯
                // 如果当前状态剩余时间大于新时间，截断为新时间
                if(timeLeft > stateTimeTable[currentState]) timeLeft = stateTimeTable[currentState];
            } else {
                ShowSettingColorLights();
            }
        }
    }

    // 仅在设置模式且未准备退出时处理加减
    if(g_isSettingMode && g_selectedColor < 3) {
        if(lastUp == 1 && curUp == 0 && debounceCnt > 5) {
            debounceCnt = 0;
            if(g_selectedColor == 0) {
                // 红灯=绿+黄，不直接加，提示：通过加绿实现
                // 这里选择加绿
                if(g_time_green < MAX_LIGHT_TIME) g_time_green++;
            } else if(g_selectedColor == 1) {
                if(g_time_yellow < MAX_LIGHT_TIME) g_time_yellow++;
            } else if(g_selectedColor == 2) {
                if(g_time_green < MAX_LIGHT_TIME) g_time_green++; // 绿模式下直接调绿
            }
            g_time_red = g_time_green + g_time_yellow;
        }
        if(lastDown == 1 && curDown == 0 && debounceCnt > 5) {
            debounceCnt = 0;
            if(g_selectedColor == 0) {
                if(g_time_green > MIN_LIGHT_TIME) g_time_green--; // 通过减绿
            } else if(g_selectedColor == 1) {
                if(g_time_yellow > MIN_LIGHT_TIME) g_time_yellow--;
            } else if(g_selectedColor == 2) {
                if(g_time_green > MIN_LIGHT_TIME) g_time_green--;
            }
            g_time_red = g_time_green + g_time_yellow;
        }
    }

    lastSet = curSet;
    lastUp = curUp;
    lastDown = curDown;
}

/*==============================================
 *                主函数
 *==============================================*/
/**
 * @brief  主函数
 * @param  无
 * @retval 无
 */
unsigned char tens = 0;
unsigned char ones = 0;
void main(void)
{
    // 系统初始化
    System_Init();

    
    // 主循环：定时器中断处理交通灯逻辑和显示刷新
    // 主循环只负责计算显示数值，实际显示由Timer0中断处理
    while(1) {
        // 扫描按键
        Keys_Scan();
        
        if(g_isSettingMode) {
            // 设置模式：数码管显示当前选颜色的时间（限制 0-99 -> 仅显示个位：显示秒数的最后一位，另一位显示高位）
            unsigned char showValue;
            if(g_selectedColor == 0) showValue = g_time_red;
            else if(g_selectedColor == 1) showValue = g_time_yellow;
            else showValue = g_time_green;
            // 取十位和个位（但硬件现在仅两位，把十位放在南北，个位放东西）
            tens = showValue / 10;
            ones = showValue % 10;
            if(tens > 9) tens = 9; // 安全限制
            EA = 0;
            nsTime = tens;
            ewTime = ones;
            EA = 1;
            // 跳过正常倒计时显示更新
            continue;
        }
        // ==========================================
        // 计算显示数值（由主循环计算，Timer0中断显示）
        // ==========================================
        {
            unsigned char tempState;
            unsigned char tempTimeLeft;
            unsigned char newNsTime, newEwTime;
            
            // 快速读取当前状态（关中断保护）
            EA = 0;  // 关中断
            tempState = currentState;
            tempTimeLeft = timeLeft;
            EA = 1;  // 开中断
            
            // 根据当前交通灯状态计算两个方向的剩余时间
            // 【关键】：红灯方向显示需要等待的总时间
            //          绿灯/黄灯方向显示当前剩余时间
            switch(tempState) {
                case STATE_NS_GREEN_EW_RED:     // 状态0: 南北绿灯，东西红灯
                    newNsTime = tempTimeLeft;      // 南北：绿灯剩余时间（3→2→1）
                    // 东西红灯需要等：当前绿灯剩余 + 后续黄灯时间
                    newEwTime = tempTimeLeft + stateTimeTable[STATE_NS_YELLOW_EW_RED];
                    break;
                    
                case STATE_NS_YELLOW_EW_RED:    // 状态1: 南北黄灯，东西红灯
                    newNsTime = tempTimeLeft;      // 南北：黄灯剩余时间（3→2→1）
                    newEwTime = tempTimeLeft;      // 东西：红灯即将结束（3→2→1）
                    break;
                    
                case STATE_NS_RED_EW_GREEN:     // 状态2: 南北红灯，东西绿灯
                    // 南北红灯需要等：当前东西绿灯剩余 + 后续东西黄灯时间
                    newNsTime = tempTimeLeft + stateTimeTable[STATE_NS_RED_EW_YELLOW];
                    newEwTime = tempTimeLeft;      // 东西：绿灯剩余时间（3→2→1）
                    break;
                    
                case STATE_NS_RED_EW_YELLOW:    // 状态3: 南北红灯，东西黄灯
                    newNsTime = tempTimeLeft;      // 南北：红灯即将结束（3→2→1）
                    newEwTime = tempTimeLeft;      // 东西：黄灯剩余时间（3→2→1）
                    break;
                    
                default:
                    newNsTime = 0;
                    newEwTime = 0;
                    break;
            }
            
            // 限制显示范围 (0-9)，因为只使用2个数码管
            if (newNsTime > 9) newNsTime = 9;
            if (newEwTime > 9) newEwTime = 9;
            
            // 更新全局显示变量（供Timer0中断使用）
            EA = 0;  // 关中断
            nsTime = newNsTime;
            ewTime = newEwTime;
            EA = 1;  // 开中断
        }
        
        // ==========================================
        // 未来扩展功能
        // ==========================================
        // - 按键扫描处理
        // - 蓝牙通信处理
        // - 故障检测与报警
        // - 温度监控（DS18B20）
        // - 红外遥控接收
        
        // 主循环延时，降低CPU占用
        // 注意：不能延时太长，否则显示数值更新不及时
        // Delay_ms(10);  // 可选：如果CPU占用过高可以加
    }
}
