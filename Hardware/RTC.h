#ifndef __RTC_H
#define __RTC_H

/*---------------------------- ���Ͷ��� ----------------------------*/
#include <stdint.h>

/**
  * @brief RTC����ʱ��ṹ��
  * @note ʱ�䷶Χ��2000-01-01 00:00:00 �� 2099-12-31 23:59:59
  */
typedef struct {
    uint16_t year;    ///< 2000-2099
    uint8_t month;    ///< 1-12
    uint8_t day;     ///< 1-31
    uint8_t hour;    ///< 0-23
    uint8_t minute;  ///< 0-59
    uint8_t second;  ///< 0-59
} RTC_DateTime;

/*------------------------- �������� -------------------------*/

/**
  * @brief  ��ʼ��RTC������ʱ��
  * @param  timestamp: UNIXʱ�����1970-01-01��Ƶ�������
  * @note   ʹ���ⲿ32.768KHz������Ҫ���ݵ��
  */
void RTC_Init(uint32_t timestamp);

/**
  * @brief  ��ȡ��ǰ����ʱ��
  * @retval RTC_DateTime�ṹ��
  * @note   ����ֵ����2000���׼�Զ�ת��
  */
RTC_DateTime RTC_GetDateTime(void);

/**
  * @brief  �����жϺ���
  * @param  year: Ҫ�жϵ���ݣ�2000-2099��
  * @retval 1-���꣬0-ƽ��
  */
uint8_t IsLeapYear(uint16_t year);

#endif /* __RTC_H */
