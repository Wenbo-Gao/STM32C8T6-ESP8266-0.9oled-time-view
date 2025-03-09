#include "ESP8266.h"
/**
  * @brief  ESP8266��ʼ�� 
            PA2--TX
            PA3--RX
            PA5--ESP8266ʹ������
  */
  typedef struct {
    uint16_t year;   // 2000-2099
    uint8_t month;   // 1-12
    uint8_t day;     // 1-31
    uint8_t hour;    // 0-23
    uint8_t minute;  // 0-59
    uint8_t second;  // 0-59
} DateTime;
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
  * @brief  �����Ա�API ��ȡ��ǰʱ��
  * @param  
  * @retval 
  */
void WIFI_ConnectTaoBao(void)
{
    if(EspSendCmdAndCheckRecvData("AT\r\n","OK",1000))
    {
        if(EspSendCmdAndCheckRecvData("AT+CWMODE=1\r\n","OK",1000))
        {
            if(EspSendCmdAndCheckRecvData("AT+CWJAP=\"DESKTOP0001\",\"1234567890\"\r\n","OK",10000))
            {
                if(EspSendCmdAndCheckRecvData("AT+CIPSTART=\"TCP\",\"api.m.taobao.com\",80\r\n","CONNECT",10000))
                {
                    EspSendCmdAndCheckRecvData("AT+CIPMODE=1\r\n","OK",1000);
                    EspSendCmdAndCheckRecvData("AT+CIPSEND\r\n",">",1000);
                    
                    // ������ȷ��HTTP����
                    Usart2_SendString("GET /rest/api3.do?api=mtop.common.getTimestamp HTTP/1.1\r\n");
                    Usart2_SendString("Host: api.m.taobao.com\r\n");
                    Usart2_SendString("Connection: close\r\n\r\n");
                    
                    Delay_ms(2000); // �ȴ���Ӧ
                              //��ջ��յ�buff
          memset(wifi.rxbuff,0,1024);
  
          wifi.rxover = 0;
          
          wifi.rxcount = 0;
                    
//          Delay_ms(1000);
//          Usart2_SendString("GET http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp\r\n");
//          Delay_ms(1000);
                    // ��ȷ�˳�͸��
                    Delay_ms(1000);
                    Usart2_SendString("+++");
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
//void GetTime_RecvData(void)
//{
//  char Val[20] = {0};
//  char tep[20] = {0};
//  uint8_t i = 0;
//  uint32_t timeget = 0;
////  long long timeget = 0;
////  time_t time = 0;
//  if(wifi.rxover==1)
//  {
//    wifi.rxover = 0;
//    if(wifi.rxcount>10)//�ж��Ƿ��� �·�ָ��Ļش���ָ�� �������ϴ�ʱϵͳĬ���·��� //��ֹ��Ӱ��
//    {
//    char *addr =  strstr((char *)(wifi.rxbuff+6),"\"t\"");
//    addr+=5;
//    while(*(addr + i) != '"')
//    {
//      Val[i] = *( addr+i);
//      i++;
//    }
////    printf("%s\r\n",Val);
////    sscanf(Val, "%lld", &timeget);  // �洢����Ϊlong long int
////    printf("%lld\r\n",timeget);
//    
//    for(i = 0;i<10;i++) //����Ҫȥ��3λ ����ʹ��atoiת��
//    {
//      tep[i] = Val[i];
//    }
////     printf("%s\r\n",tep);
//    timeget = atoi(tep);
//    memset(wifi.rxbuff,0,1024);
//    
//    wifi.rxcount = 0;
//    
//    RTC_Init(timeget);
//    
//    }
//  }
//}
//void GetTime_RecvData(void) {
//if (wifi.rxover == 1) {
//	wifi.rxover = 0;
//  
//	// ����HTTPͷ����λJSON��ʼλ��
//	char *json_start = strchr((char*)wifi.rxbuff, '{');
//	if (!json_start) {
////            printf("Error: Invalid JSON format!\n");
//		return;
//	}

//	cJSON *root = cJSON_Parse(json_start);
//	if (!root) {
////            printf("JSON parse error: %s\n", cJSON_GetErrorPtr());
//		return;
//	}

//	cJSON *data = cJSON_GetObjectItem(root, "data");
//	if (data) {
//		cJSON *t = cJSON_GetObjectItem(data, "t");
//		if (t && cJSON_IsString(t)) {
//			// ת��13λ����ʱ���Ϊ��
//			uint64_t timestamp = strtoull(t->valuestring, NULL, 10);
//			timestamp /= 1000; 
//			RTC_Init((uint32_t)timestamp);
//		}
//	}
//  
////        cJSON_Delete(root);
////        memset(wifi.rxbuff, 0, sizeof(wifi.rxbuff));
////        wifi.rxcount = 0;
//}
//}


DateTime ConvertUnixTime(uint32_t timestamp) {
    DateTime dt;
    uint32_t seconds = timestamp;

    // ����ʱ����
    dt.second = seconds % 60;
    seconds /= 60;
    dt.minute = seconds % 60;
    seconds /= 60;
    dt.hour = seconds % 24;
    seconds /= 24;

    // ���������գ�����1970-01-01��
    int year = 1970;
    while (seconds >= (IsLeapYear(year) ? 366 : 365)) {
        seconds -= IsLeapYear(year) ? 366 : 365;
        year++;
    }
    dt.year = year;

    // �����·�
    uint8_t month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (IsLeapYear(year)) month_days[1] = 29;
  
    dt.month = 1;
    while (seconds >= month_days[dt.month-1]) {
        seconds -= month_days[dt.month-1];
        dt.month++;
    }
    dt.day = seconds + 1; // ������1��ʼ

    return dt;
}
void GetTime_RecvData(void) {
    if (wifi.rxover == 1) {
        wifi.rxover = 0;
      
        // ��λJSON��ʼλ��
        char *json_start = strchr((char*)wifi.rxbuff, '{');
        if (json_start) {
            cJSON *root = cJSON_Parse(json_start);
            if (root) {
                cJSON *data = cJSON_GetObjectItem(root, "data");
                if (data) {
                    cJSON *t = cJSON_GetObjectItem(data, "t");
                    if (t && cJSON_IsString(t)) {
                        // ת��ʱ���
                        uint64_t timestamp = strtoull(t->valuestring, NULL, 10);
                        timestamp /= 1000; // ����ת��
                      
                        // ����RTC
                        RTC_Init((uint32_t)timestamp);
                      
                        // ת������ӡʱ��
                        DateTime dt = ConvertUnixTime((uint32_t)timestamp);
////                        printf("[TimeSync] %04d-%02d-%02d %02d:%02d:%02d\r\n",
//                              dt.year, dt.month, dt.day, 
//                              dt.hour, dt.minute, dt.second);
                    }
                }
                cJSON_Delete(root);
            }
        }
        memset(wifi.rxbuff, 0, sizeof(wifi.rxbuff));
        wifi.rxcount = 0;
    }
}




// ʱ��ƫ�ƣ���λ��Сʱ��

	DateTime ConvertLocalTime(uint32_t utc_timestamp) {
    uint32_t local_seconds = utc_timestamp + TIMEZONE_OFFSET * 3600;
    DateTime dt = ConvertUnixTime(local_seconds);
  
    // �������
    if (dt.hour >= 24) {
        dt.hour -= 24;
        dt.day += 1;
        // ������¿��꣨�貹�������߼���
    }
    return dt;
}

