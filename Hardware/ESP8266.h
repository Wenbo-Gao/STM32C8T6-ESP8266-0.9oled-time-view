#ifndef  __ESP8266_H__//如果没有定义了则参加以下编译
#define  __ESP8266_H__//一旦定义就有了定义 所以 其目的就是防止模块重复编译

#include "stm32f10x.h"
#include "delay.h"
#include "string.h"
#include "stdlib.h"
#include "RTC.h"
#include "stdio.h"
#define TIMEZONE_OFFSET 8
void ESP8266_Init(void);
void Usart2_SendByte(uint8_t Byte);
void Usart2_SendString(const char *string);
void Esp8266_SendString(uint8_t *string, uint16_t len);//串口2 发送字符串
// 在文件顶部添加声明
uint8_t EspSendCmdAndCheckRecvData(const char *cmd, const char *Rcmd, uint16_t outtime);
void WIFI_ConnectPinduoduo(void);
void GetTime_RecvData(void);
typedef  struct{
    uint8_t rxbuff[1024];
    uint16_t rxcount;
    uint8_t rxover;
    uint8_t txbuff[1024];
    uint16_t txcount;
}WIFI;

extern WIFI wifi;
#endif  //结束编译
