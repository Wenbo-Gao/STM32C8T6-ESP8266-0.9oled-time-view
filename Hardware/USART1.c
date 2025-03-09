#include "usart1.h"

uint8_t RxFlag;
/*
 *PA9  --  TX
 *PA10 --  RX
*/
void Usart_Init(void)
{
  //使能时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);//使能时钟A和USART1
  
  //为初始化函数做准备
  GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//设置PA9引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;//设置输出模式为复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//设置输出速度为50MHZ
  //初始化函数PIN9↓
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//设置PA10引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;//设置输出模式为上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//设置输出速度为50MHZ
  //初始化函数PIN10↓
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
  
  USART_InitTypeDef USART_InitStructure; //定义串口结构体
  USART_InitStructure.USART_BaudRate = 115200; //波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数字帧长度
  USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位
  USART_InitStructure.USART_Parity = USART_Parity_No; //不使用校验位 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控制
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //模式为发送+接收
  //初始化串口1
  USART_Init(USART1, &USART_InitStructure); 
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  NVIC_InitTypeDef NVIC_InitStructure; //定义结构体
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //根据上面的我们所选取的USART1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//这里选择的是抢占 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //这里选择的是响应1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能指定的中断通道
  //初始化函数↓
  NVIC_Init(&NVIC_InitStructure);
  //使能串口1
  USART_Cmd(USART1, ENABLE);
}
/**
  * @brief  串口1发送字节 -- 发送的最基本的函数 -->其它发送函数都是基于它
  * @param  
  * @retval 
  */
void Usart1_SendByte(uint8_t Byte)
{
  USART_SendData(USART1,Byte);
  while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );
}

/**
  * @brief  串口中断函数
            中断函数里面可以放你想要实现的功能函数
  * @param  
  * @retval 
  */
void USART1_IRQHandler(void)
{
  uint8_t data = 0;
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    data = USART_ReceiveData(USART1);//获取串口助手发送给单片机的数据
    USART_SendData(USART2,data);//通过串口2发送 --> ESP8266
  }
}
/**
  * @brief  函数重定义--封装printf函数
  * @param  
  * @retval 
  */
int fputc(int ch, FILE *f)
{
  Usart1_SendByte(ch);
  return ch;
}