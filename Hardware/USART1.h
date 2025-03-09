#ifndef  __USART_H__//如果没有定义了则参加以下编译
#define  __USART_H__//一旦定义就有了定义 所以 其目的就是防止模块重复编译

#include "stm32f10x.h"
#include "stdio.h"
#include <stdarg.h>

void Usart_Init(void);
void Usart1_SendByte(uint8_t Byte);

#endif  //结束编译

