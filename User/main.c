#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "ESP8266.h"


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
        OLED_UpdateArea(40, 0, 80, 127);
        OLED_ShowString(40, 0, date_buf, OLED_8X16);
    }

    // 时间变化时刷新时间区域
    if(last_time.second != current_time.second) {
        sprintf(time_buf, "%02d:%02d:%02d", 
               current_time.hour, current_time.minute, current_time.second);
        OLED_UpdateArea(40, 16, 60, 32);
        OLED_ShowString(40, 16, time_buf, OLED_8X16);
    }

    last_time = current_time;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
    // 硬件初始化
    Display_Init();
    ESP8266_Init();
	WIFI_ConnectPinduoduo();
	GetTime_RecvData();
	Update_DateTime_Display() ;



    // 主显示循环
    while(1) {

	Delay_s(1);
	Update_DateTime_Display() ;	
		
   }
}
