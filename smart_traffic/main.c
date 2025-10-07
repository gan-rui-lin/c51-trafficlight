

#include <reg51.h>    // 8051标准寄存器定义

/*==============================================
 *                引脚定义部分
 *==============================================*/
// 调试LED引脚定义
sbit DEBUG_1S_PIN    = P3^6;  // 1秒指示灯（心跳）
sbit DEBUG_STATE_PIN = P3^7;  // 状态指示灯

// 南北方向交通灯引脚定义
#define NS_RED_BIT      0x01  // P2.0
#define NS_YELLOW_BIT   0x02  // P2.1
#define NS_GREEN_BIT    0x04  // P2.2

// 东西方向交通灯引脚定义
#define EW_RED_BIT      0x08  // P2.3
#define EW_YELLOW_BIT   0x10  // P2.4
#define EW_GREEN_BIT    0x20  // P2.5

/*==============================================
 *                系统参数定义
 *==============================================*/
// 交通灯状态定义
#define STATE_NS_GREEN_EW_RED     0  // 南北绿灯，东西红灯
#define STATE_NS_YELLOW_EW_RED    1  // 南北黄灯，东西红灯
#define STATE_NS_RED_EW_GREEN     2  // 南北红灯，东西绿灯
#define STATE_NS_RED_EW_YELLOW    3  // 南北红灯，东西黄灯

// 时间配置（单位：秒）
#define GREEN_LIGHT_TIME    25    // 绿灯时间
#define YELLOW_LIGHT_TIME   3     // 黄灯时间
#define FLASH_START_TIME    3     // 开始闪烁的剩余时间

/*==============================================
 *                全局变量定义
 *==============================================*/
volatile unsigned char currentState = STATE_NS_GREEN_EW_RED;
volatile unsigned char timeLeft = GREEN_LIGHT_TIME;
volatile unsigned char isFlashing = 0;
volatile unsigned int timer0Count = 0;
volatile unsigned int flashCount = 0;

// 用于保存当前输出状态的影子变量（对应P2端口）
volatile unsigned char trafficOutput = 0x00;

// 状态时间配置表
unsigned char stateTimeTable[4] = {
    GREEN_LIGHT_TIME,
    YELLOW_LIGHT_TIME,
    GREEN_LIGHT_TIME,
    YELLOW_LIGHT_TIME
};

/*==============================================
 *                硬件控制函数
 *==============================================*/
/**
 * @brief  设置交通灯状态
 * @param  state: 交通灯状态(0-3)
 * @retval 无
 */
void SetTrafficLights(unsigned char state) {
    trafficOutput = 0x00; // 清空所有灯

    switch(state) {
        case STATE_NS_GREEN_EW_RED:
            trafficOutput = NS_GREEN_BIT | EW_RED_BIT;
            break;
        case STATE_NS_YELLOW_EW_RED:
            trafficOutput = NS_YELLOW_BIT | EW_RED_BIT;
            break;
        case STATE_NS_RED_EW_GREEN:
            trafficOutput = NS_RED_BIT | EW_GREEN_BIT;
            break;
        case STATE_NS_RED_EW_YELLOW:
            trafficOutput = NS_RED_BIT | EW_YELLOW_BIT;
            break;
        default:
            trafficOutput = NS_RED_BIT | EW_RED_BIT;
            break;
    }

    P2 = trafficOutput; // 一次性写入端口
}

/**
 * @brief  处理交通灯闪烁逻辑（影子变量版本）
 */
void HandleTrafficLightFlash(void) {
    if (timeLeft <= FLASH_START_TIME && timeLeft > 0) {
        isFlashing = 1;

        // 每约500ms翻转一次目标灯
        if ((flashCount % 250) == 0) {
            unsigned char mask = 0x00;

            switch(currentState) {
                case STATE_NS_GREEN_EW_RED:  mask = NS_GREEN_BIT;  break;
                case STATE_NS_YELLOW_EW_RED: mask = NS_YELLOW_BIT; break;
                case STATE_NS_RED_EW_GREEN:  mask = EW_GREEN_BIT;  break;
                case STATE_NS_RED_EW_YELLOW: mask = EW_YELLOW_BIT; break;
            }

            trafficOutput ^= mask;   // 翻转闪烁灯对应位
            P2 = trafficOutput;      // 更新端口输出
        }
    } else {
        isFlashing = 0;
    }
}

/**
 * @brief  切换到下一个交通灯状态
 */
void SwitchToNextState(void) {
    currentState = (currentState + 1) % 4;
    timeLeft = stateTimeTable[currentState];
    SetTrafficLights(currentState);
    isFlashing = 0;

    // 状态切换指示：DEBUG_STATE_PIN闪烁一次
    DEBUG_STATE_PIN = 1;
    DEBUG_STATE_PIN = 0;
}

/*==============================================
 *                定时器初始化
 *==============================================*/
void Timer0_Init(void) {
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = 0xA9;
    TL0 = 0x6A;

    TF0 = 0;
    ET0 = 1;
    PT0 = 1;
    EA = 1;
    TR0 = 1;

    timer0Count = 0;
    flashCount = 0;
}

/*==============================================
 *                中断服务函数
 *==============================================*/
void Timer0_ISR(void) interrupt 1 {
    TH0 = 0xA9;
    TL0 = 0x6A;
    TR0 = 0;  // 暂停计时器
    EA = 0;   // 全局关中断

    timer0Count++;
    flashCount++;

    // 心跳指示灯
    if ((timer0Count % 100) == 0){
			
        DEBUG_1S_PIN = !DEBUG_1S_PIN;
			if(P2 == 0x00) P2 = 0xFF;
			else P2 = 0x00;
			//P2 = 0x00; // 测试写入，观察P2变化
			//P2 = 0xFF;
		}	
		
    EA = 1;   // 全局关中断
    TR0 = 1;  // 暂停计时器
}

/*==============================================
 *                系统初始化
 *==============================================*/
void System_Init(void) {
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    P2 = 0x00;
    trafficOutput = 0x00;

    // 自检：所有灯亮1秒
    P2 = NS_RED_BIT | NS_YELLOW_BIT | NS_GREEN_BIT |
         EW_RED_BIT | EW_YELLOW_BIT | EW_GREEN_BIT;
    DEBUG_1S_PIN = 1;
    DEBUG_STATE_PIN = 1;

    {
        unsigned int i, j;
        for(i = 0; i < 1000; i++)
            for(j = 0; j < 100; j++);
    }

    // 熄灭所有灯
    P2 = 0x00;
    DEBUG_1S_PIN = 0;
    DEBUG_STATE_PIN = 0;
    trafficOutput = 0x00;

    // 初始化状态
    currentState = STATE_NS_GREEN_EW_RED;
    timeLeft = stateTimeTable[currentState];
    isFlashing = 0;

    SetTrafficLights(currentState);
    Timer0_Init();
}

/*==============================================
 *                主函数
 *==============================================*/
void main(void) {
    System_Init();

    while(1) {
        // 主循环空闲，可扩展功能
    }
}
