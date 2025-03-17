#include "ESP8266.h"
/**
  * @brief  ESP8266��ʼ�� 
            PA2--TX
            PA3--RX
            PA5--ESP8266ʹ������
  */

WIFI wifi = {0};
void ESP8266_Init(void)
{
  //ʹ��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��ʱ��A
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��ʱ��USART2
  //Ϊ��ʼ��������׼��
  GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//����PA2����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;//�������ģʽΪ�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//��������ٶ�Ϊ50MHZ
  //��ʼ������PIN2
  GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//����PA3����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;//�������ģʽΪ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//��������ٶ�Ϊ50MHZ
  //��ʼ������PIN3��
  GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//����PA5���� -- //ESP8266ʹ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;//�������ģʽΪ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//��������ٶ�Ϊ50MHZ
  //��ʼ������PIN5��
  GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
  
  USART_InitTypeDef USART_InitStructure; //���崮�ڽṹ��
  USART_InitStructure.USART_BaudRate = 115200; //������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//����֡����
  USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No; //��ʹ��У��λ 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��ʹ��Ӳ��������
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //ģʽΪ����+����
  //��ʼ������2
  USART_Init(USART2, &USART_InitStructure); 
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//ʹ�ܿ����ж�
   
  NVIC_InitTypeDef NVIC_InitStructure; //����ṹ��
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //���������������ѡȡ��USART2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//����ѡ�������ռ 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //����ѡ�������Ӧ2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ��ָ�����ж�ͨ��
  //��ʼ��������
  NVIC_Init(&NVIC_InitStructure);
  //ʹ�ܴ���2
  USART_Cmd(USART2, ENABLE);

  GPIO_SetBits(GPIOA,GPIO_Pin_5);//GPIOA �ߵ�ƽ��Ч -- ʹ��ESP8266
  Delay_ms(2000);//��ʱ  �ȴ�WiFiģ���ȶ�
}

/**
  * @brief  ����2�����ֽ� -- ���͵�������ĺ��� -->�������ͺ������ǻ�����
  * @param  
  * @retval 
  */
void Usart2_SendByte(uint8_t Byte)
{
  USART_SendData(USART2,Byte);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET );
}
/**
  * @brief  �����жϺ���
            �жϺ���������Է�����Ҫʵ�ֵĹ��ܺ���
  * @param  
  * @retval 
  */
void USART2_IRQHandler(void)
{
  uint8_t data = 0;
  if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
  {
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    
    data = USART_ReceiveData(USART2);//ESP8266 ���͸� STM32������
    
    wifi.rxbuff[wifi.rxcount++] = data;//�� ESP8266 ���͸���Ƭ�������� ת�浽rxbuff����
    
    USART_SendData(USART1,data); //��ͨ�� ���������ڵ�����Ļ�� ��ʾ
  }
  if(USART_GetITStatus(USART2, USART_IT_IDLE) == 1) //���ڿ����ж�
  {
      data = USART2->SR;
    
      data = USART2->DR;
    
      wifi.rxover = 1;
  }
}
/**
  * @brief  �����ַ���
  * @param  string �ַ���
  * @retval 
  */
void Usart2_SendString(const char *string)
{
  for(uint8_t i = 0;string[i] != '\0';i++)
  {
    Usart2_SendByte(string[i]);
  }
}

/**
  * @brief  �����ַ��� + ������
  * @param  
  * @retval 
  */
void Esp8266_SendString(uint8_t *string,uint16_t len)//����3 �����ַ���
{
  for(uint16_t i=0;i<len;i++)
  Usart2_SendByte(string[i]);
}

/**
  * @brief  ����ATָ����ATָ��ķ��أ��ж�ATָ���Ƿ��ͳɹ�
  * @param  cmd:���͵�ATָ��
  * @param  Rcmd:ATָ���Ӧ�ķ���ֵ
  * @param  outtime:ESP8266��ʱ�ظ��ж�
  * @retval 
  */
// ���ļ������������
uint8_t EspSendCmdAndCheckRecvData(const char *cmd, const char *Rcmd, uint16_t outtime)
{
  uint8_t data = 0;
  
  //����ATָ��֮ǰҪ���
  
  memset(wifi.rxbuff,0,1024);
  
  wifi.rxover = 0;
  
  wifi.rxcount = 0;
  
  Usart2_SendString(cmd);//����ATָ�� -- > �Ż���봮��2�жϷ���
  
  while(outtime) 
  {
    if(wifi.rxover == 1)
    {
      if(strstr((char *)wifi.rxbuff,(char *)Rcmd) != NULL)
      {
        data = 1;
        break;
      }
    }
    outtime--;
    Delay_ms(1);
  }
  return data;
}
/**
  * @brief  ����ƴ���API ��ȡ��ǰʱ��
  * @param  
  * @retval 
  */
void WIFI_ConnectPinduoduo(void)
{
    if(EspSendCmdAndCheckRecvData("AT\r\n","OK",1000))
		{
        if(EspSendCmdAndCheckRecvData("AT+CWMODE=1\r\n","OK",1000))
        {
            if(EspSendCmdAndCheckRecvData("AT+CWJAP=\"Redmi Note 12T Pro\",\"1234567890\"\r\n","OK",10000))
            {
                if(EspSendCmdAndCheckRecvData("AT+CIPSTART=\"TCP\",\"api.pinduoduo.com\",80\r\n","CONNECT",10000))
                {
                    EspSendCmdAndCheckRecvData("AT+CIPMODE=1\r\n","OK",5000);
					EspSendCmdAndCheckRecvData("AT+CIPSEND\r\n",">",5000);
					
			Usart2_SendString("GET https://api.pinduoduo.com/api/server/_stm\r\n\r\n");
			Delay_ms(1000);

                }
            }
        }
    }	
}






/**
  * @brief  ����JSON����
  * @param  
  * @retval 
  */
void GetTime_RecvData(void)
{
  char Val[28] = {0};
  char tep[28] = {0};
  uint8_t i = 0;
  uint32_t timeget = 0;
  if(wifi.rxover==1)
  {
    wifi.rxover = 0;
    if(wifi.rxcount>10)//�ж��Ƿ��� �·�ָ��Ļش���ָ�� �������ϴ�ʱϵͳĬ���·��� //��ֹ��Ӱ��
    {
    char *addr =  strstr((char *)(wifi.rxbuff),"\"server_time\":");
		addr+=14;
    while(*(addr + i) != '}')
    {
      Val[i] = *(addr+i);
      i++;
    }
    
    for(i = 0;i<10;i++) //����Ҫȥ��3λ ����ʹ��atoiת��
    {
      tep[i] = Val[i];
    }
    timeget = atoi(tep);
    memset(wifi.rxbuff,0,1024);
    
    wifi.rxcount = 0;
    
    RTC_Init(timeget);
    
    }
  }
}

