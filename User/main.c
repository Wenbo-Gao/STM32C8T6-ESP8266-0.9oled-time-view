#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "usart1.h"
#include "ESP8266.h"
#include "RTC.h"

// 显示坐标定义
#define DATE_X        0   // 日期起始列
#define DATE_Y        0   // 日期起始行（0-7页）
#define TIME_X        24  // 时间起始列
#define TIME_Y        4   // 时间起始行
#define STATUS_X      96  // 状态图标列
#define STATUS_Y      6   // 状态图标行

// 全局时间结构体
RTC_DateTime current_time;


void Display_Init(void) {
    OLED_Init();
    OLED_Clear();
    // 显示固定内容
    OLED_ShowString(0, 0, "Date:", OLED_8X16);
    OLED_ShowString(0, 16, "Time:", OLED_8X16);
    OLED_Update();
}

void Update_DateTime_Display(void) {
    static RTC_DateTime last_time = {0};
    char time_buf[9], date_buf[11];
    // 获取当前时间（UTC+8）
    current_time = RTC_GetDateTime();
    current_time.hour = (current_time.hour + 8) % 24;

    // 日期变化时刷新日期区域
    if(memcmp(&last_time, &current_time, sizeof(RTC_DateTime)) != 0) {
        sprintf(date_buf, "%04d-%02d-%02d", 
               current_time.year, current_time.month, current_time.day);
        OLED_UpdateArea(30, 0, 30, 88);
        OLED_ShowString(30, 0, date_buf, OLED_8X16);
    }

    // 时间变化时刷新时间区域
    if(last_time.second != current_time.second) {
        sprintf(time_buf, "%02d:%02d:%02d", 
               current_time.hour, current_time.minute, current_time.second);
        OLED_UpdateArea(TIME_X, TIME_Y+40, 0, 32);
        OLED_ShowString(TIME_X, TIME_Y+40, time_buf, OLED_8X16);
    }

    last_time = current_time;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
    // 硬件初始化
    Display_Init();
    ESP8266_Init();
	WIFI_ConnectTaoBao();
	GetTime_RecvData() ;
    // 首次时间同步
//    uint8_t sync_success = 0;
//    for(int retry=0; retry<3; retry++) {
//        if(WIFI_ConnectTaoBao() ) {
//            sync_success = 1;
//            break;
//        }
//        Delay_ms(2000);
//    }

    // 显示同步状态
    OLED_UpdateArea(STATUS_X, STATUS_Y, 32, 8);
     OLED_ShowString(STATUS_X, STATUS_Y, "[√]", OLED_8X16); 
     OLED_ShowString(STATUS_X, STATUS_Y+16, "[×]", OLED_8X16);
    OLED_Update();

    // 主显示循环
    while(1) {
        // 每秒更新一次时间显示
//        static uint32_t last_update = 0;
//        if(HAL_GetTick() - last_update >= 1000) {
//            last_update = HAL_GetTick();
//            Update_DateTime_Display();
//            OLED_Update();
//        }

//        // 每小时同步一次时间
//        static uint32_t last_sync = 0;
//        if(HAL_GetTick() - last_sync >= 3600000) {
//            if(WIFI_ConnectTaoBao() && GetTime_RecvData()) {
//                OLED_ShowString(STATUS_X, STATUS_Y, "[√]", OLED_8X16);
//            } else {
//                OLED_ShowString(STATUS_X, STATUS_Y, "[×]", OLED_8X16);
//            }
//            last_sync = HAL_GetTick();
//            OLED_Update();
//        }
   }
}