#ifndef  __USART_H__//���û�ж�������μ����±���
#define  __USART_H__//һ����������˶��� ���� ��Ŀ�ľ��Ƿ�ֹģ���ظ�����

#include "stm32f10x.h"
#include "stdio.h"
#include <stdarg.h>

void Usart_Init(void);
void Usart1_SendByte(uint8_t Byte);

#endif  //��������

