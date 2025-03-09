#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"

// UNIX时间基准（1970-01-01 00:00:00）
#define UNIX_EPOCH_OFFSET  946684800  // 2000-01-01到1970-01-01的秒数差

// RTC日期时间结构体
typedef struct {
    uint16_t year;    // 2000~2099
    uint8_t month;    // 1~12
    uint8_t day;     // 1~31
    uint8_t hour;    // 0~23
    uint8_t minute;  // 0~59
    uint8_t second;  // 0~59
} RTC_DateTime;

/* RTC初始化函数 */
void RTC_Init(uint32_t timestamp) {
    // 1. 使能时钟和备份域访问
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
  
    // 2. 配置RTC时钟源（假设使用外部32.768KHz晶振）
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); // 等待LSE就绪
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
    RCC_RTCCLKCmd(ENABLE);
  
    // 3. 初始化RTC预分频器
    RTC_SetPrescaler(32768-1); // 1Hz计数
  
    // 4. 设置时间（仅在RTC未初始化时）
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        RTC_WaitForLastTask(); // 等待操作完成
        RTC_SetCounter(timestamp - UNIX_EPOCH_OFFSET); // 转换为2000年基准
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5); // 标记已初始化
    }
}

/* UNIX时间戳转换函数 */
RTC_DateTime ConvertTimestamp(uint32_t timestamp) {
    RTC_DateTime dt;
    uint32_t seconds = timestamp - UNIX_EPOCH_OFFSET;
  
    // 计算天数
    uint32_t days = seconds / 86400;
    seconds %= 86400;
  
    // 计算时间
    dt.hour = seconds / 3600;
    dt.minute = (seconds % 3600) / 60;
    dt.second = seconds % 60;
uint8_t IsLeapYear(uint16_t year);
    // 计算日期（基于2000-01-01）
    uint16_t year = 2000;
    while(1) {
        uint16_t daysInYear = IsLeapYear(year) ? 366 : 365;
        if(days >= daysInYear) {
            days -= daysInYear;
            year++;
        } else break;
    }
    dt.year = year;

    // 计算月份
    uint8_t monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeapYear(year)) monthDays[1] = 29;
  
    for(dt.month=1; dt.month<=12; dt.month++) {
        if(days >= monthDays[dt.month-1]) {
            days -= monthDays[dt.month-1];
        } else break;
    }
    dt.day = days + 1; // 天数从1开始
  
	    // 增加年份校验
    if (dt.year < 2000 || dt.year > 2099) {
        // 处理错误（如返回默认值或触发异常）
        dt.year = 2000;
        dt.month = 1;
        dt.day = 1;
    }
	
	
    return dt;
}

/* 闰年判断 */
uint8_t IsLeapYear(uint16_t year) {
    if (year % 4 != 0) return 0;        // 不能被4整除->平年
    if (year % 100 != 0) return 1;       // 能被4整除且不能被100整除->闰年
    return (year % 400 == 0) ? 1 : 0;    // 能被100整除则必须能被400整除
}

/* 获取当前时间（RTC寄存器读取） */
RTC_DateTime RTC_GetDateTime(void) {
    uint32_t counter = RTC_GetCounter();
    return ConvertTimestamp(counter + UNIX_EPOCH_OFFSET);
}