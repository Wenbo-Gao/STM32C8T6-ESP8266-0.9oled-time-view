#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"

// UNIXʱ���׼��1970-01-01 00:00:00��
#define UNIX_EPOCH_OFFSET  946684800  // 2000-01-01��1970-01-01��������

// RTC����ʱ��ṹ��
typedef struct {
    uint16_t year;    // 2000~2099
    uint8_t month;    // 1~12
    uint8_t day;     // 1~31
    uint8_t hour;    // 0~23
    uint8_t minute;  // 0~59
    uint8_t second;  // 0~59
} RTC_DateTime;

/* RTC��ʼ������ */
void RTC_Init(uint32_t timestamp) {
    // 1. ʹ��ʱ�Ӻͱ��������
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
  
    // 2. ����RTCʱ��Դ������ʹ���ⲿ32.768KHz����
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); // �ȴ�LSE����
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
    RCC_RTCCLKCmd(ENABLE);
  
    // 3. ��ʼ��RTCԤ��Ƶ��
    RTC_SetPrescaler(32768-1); // 1Hz����
  
    // 4. ����ʱ�䣨����RTCδ��ʼ��ʱ��
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        RTC_WaitForLastTask(); // �ȴ��������
        RTC_SetCounter(timestamp - UNIX_EPOCH_OFFSET); // ת��Ϊ2000���׼
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5); // ����ѳ�ʼ��
    }
}

/* UNIXʱ���ת������ */
RTC_DateTime ConvertTimestamp(uint32_t timestamp) {
    RTC_DateTime dt;
    uint32_t seconds = timestamp - UNIX_EPOCH_OFFSET;
  
    // ��������
    uint32_t days = seconds / 86400;
    seconds %= 86400;
  
    // ����ʱ��
    dt.hour = seconds / 3600;
    dt.minute = (seconds % 3600) / 60;
    dt.second = seconds % 60;
uint8_t IsLeapYear(uint16_t year);
    // �������ڣ�����2000-01-01��
    uint16_t year = 2000;
    while(1) {
        uint16_t daysInYear = IsLeapYear(year) ? 366 : 365;
        if(days >= daysInYear) {
            days -= daysInYear;
            year++;
        } else break;
    }
    dt.year = year;

    // �����·�
    uint8_t monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeapYear(year)) monthDays[1] = 29;
  
    for(dt.month=1; dt.month<=12; dt.month++) {
        if(days >= monthDays[dt.month-1]) {
            days -= monthDays[dt.month-1];
        } else break;
    }
    dt.day = days + 1; // ������1��ʼ
  
	    // �������У��
    if (dt.year < 2000 || dt.year > 2099) {
        // ��������緵��Ĭ��ֵ�򴥷��쳣��
        dt.year = 2000;
        dt.month = 1;
        dt.day = 1;
    }
	
	
    return dt;
}

/* �����ж� */
uint8_t IsLeapYear(uint16_t year) {
    if (year % 4 != 0) return 0;        // ���ܱ�4����->ƽ��
    if (year % 100 != 0) return 1;       // �ܱ�4�����Ҳ��ܱ�100����->����
    return (year % 400 == 0) ? 1 : 0;    // �ܱ�100����������ܱ�400����
}

/* ��ȡ��ǰʱ�䣨RTC�Ĵ�����ȡ�� */
RTC_DateTime RTC_GetDateTime(void) {
    uint32_t counter = RTC_GetCounter();
    return ConvertTimestamp(counter + UNIX_EPOCH_OFFSET);
}