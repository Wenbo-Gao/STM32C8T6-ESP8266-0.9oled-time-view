#include "usart1.h"

uint8_t RxFlag;
/*
 *PA9  --  TX
 *PA10 --  RX
*/
void Usart_Init(void)
{
  //ʹ��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);//ʹ��ʱ��A��USART1
  
  //Ϊ��ʼ��������׼��
  GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//����PA9����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;//�������ģʽΪ�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//��������ٶ�Ϊ50MHZ
  //��ʼ������PIN9��
  GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//����PA10����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;//�������ģʽΪ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//��������ٶ�Ϊ50MHZ
  //��ʼ������PIN10��
  GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
  
  USART_InitTypeDef USART_InitStructure; //���崮�ڽṹ��
  USART_InitStructure.USART_BaudRate = 115200; //������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//����֡����
  USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No; //��ʹ��У��λ 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��ʹ��Ӳ��������
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //ģʽΪ����+����
  //��ʼ������1
  USART_Init(USART1, &USART_InitStructure); 
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  NVIC_InitTypeDef NVIC_InitStructure; //����ṹ��
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //���������������ѡȡ��USART1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//����ѡ�������ռ 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //����ѡ�������Ӧ1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ��ָ�����ж�ͨ��
  //��ʼ��������
  NVIC_Init(&NVIC_InitStructure);
  //ʹ�ܴ���1
  USART_Cmd(USART1, ENABLE);
}
/**
  * @brief  ����1�����ֽ� -- ���͵�������ĺ��� -->�������ͺ������ǻ�����
  * @param  
  * @retval 
  */
void Usart1_SendByte(uint8_t Byte)
{
  USART_SendData(USART1,Byte);
  while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );
}

/**
  * @brief  �����жϺ���
            �жϺ���������Է�����Ҫʵ�ֵĹ��ܺ���
  * @param  
  * @retval 
  */
void USART1_IRQHandler(void)
{
  uint8_t data = 0;
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    data = USART_ReceiveData(USART1);//��ȡ�������ַ��͸���Ƭ��������
    USART_SendData(USART2,data);//ͨ������2���� --> ESP8266
  }
}
/**
  * @brief  �����ض���--��װprintf����
  * @param  
  * @retval 
  */
int fputc(int ch, FILE *f)
{
  Usart1_SendByte(ch);
  return ch;
}