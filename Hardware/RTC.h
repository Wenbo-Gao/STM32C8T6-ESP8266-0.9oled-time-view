#ifndef __RTC_H
#define __RTC_H

/*---------------------------- 类型定义 ----------------------------*/
#include <stdint.h>

/**
  * @brief RTC日期时间结构体
  * @note 时间范围：2000-01-01 00:00:00 至 2099-12-31 23:59:59
  */
typedef struct {
    uint16_t year;    ///< 2000-2099
    uint8_t month;    ///< 1-12
    uint8_t day;     ///< 1-31
    uint8_t hour;    ///< 0-23
    uint8_t minute;  ///< 0-59
    uint8_t second;  ///< 0-59
} RTC_DateTime;

/*------------------------- 函数声明 -------------------------*/

/**
  * @brief  初始化RTC并设置时间
  * @param  timestamp: UNIX时间戳（1970-01-01起计的秒数）
  * @note   使用外部32.768KHz晶振，需要备份电池
  */
void RTC_Init(uint32_t timestamp);

/**
  * @brief  获取当前日期时间
  * @retval RTC_DateTime结构体
  * @note   返回值基于2000年基准自动转换
  */
RTC_DateTime RTC_GetDateTime(void);

/**
  * @brief  闰年判断函数
  * @param  year: 要判断的年份（2000-2099）
  * @retval 1-闰年，0-平年
  */
uint8_t IsLeapYear(uint16_t year);

#endif /* __RTC_H */
