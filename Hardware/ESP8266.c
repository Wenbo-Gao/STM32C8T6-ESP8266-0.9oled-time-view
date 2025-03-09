#include "ESP8266.h"
/**
  * @brief  ESP8266初始化 
            PA2--TX
            PA3--RX
            PA5--ESP8266使能引脚
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
  //使能时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能时钟A
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能时钟USART2
  //为初始化函数做准备
  GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//设置PA2引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;//设置输出模式为复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//设置输出速度为50MHZ
  //初始化函数PIN2
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//设置PA3引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;//设置输出模式为上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//设置输出速度为50MHZ
  //初始化函数PIN3↓
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//设置PA5引脚 -- //ESP8266使能引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;//设置输出模式为推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;//设置输出速度为50MHZ
  //初始化函数PIN5↓
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
  
  USART_InitTypeDef USART_InitStructure; //定义串口结构体
  USART_InitStructure.USART_BaudRate = 115200; //波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数字帧长度
  USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位
  USART_InitStructure.USART_Parity = USART_Parity_No; //不使用校验位 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用硬件流控制
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //模式为发送+接收
  //初始化串口2
  USART_Init(USART2, &USART_InitStructure); 
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//使能接收中断
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//使能空闲中断
   
  NVIC_InitTypeDef NVIC_InitStructure; //定义结构体
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //根据上面的我们所选取的USART2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//这里选择的是抢占 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //这里选择的是响应2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能指定的中断通道
  //初始化函数↓
  NVIC_Init(&NVIC_InitStructure);
  //使能串口2
  USART_Cmd(USART2, ENABLE);

  GPIO_SetBits(GPIOA,GPIO_Pin_5);//GPIOA 高电平有效 -- 使能ESP8266
  Delay_ms(2000);//延时  等待WiFi模块稳定
}

/**
  * @brief  串口2发送字节 -- 发送的最基本的函数 -->其它发送函数都是基于它
  * @param  
  * @retval 
  */
void Usart2_SendByte(uint8_t Byte)
{
  USART_SendData(USART2,Byte);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET );
}
/**
  * @brief  串口中断函数
            中断函数里面可以放你想要实现的功能函数
  * @param  
  * @retval 
  */
void USART2_IRQHandler(void)
{
  uint8_t data = 0;
  if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
  {
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    
    data = USART_ReceiveData(USART2);//ESP8266 发送给 STM32的数据
    
    wifi.rxbuff[wifi.rxcount++] = data;//将 ESP8266 发送给单片机的数据 转存到rxbuff里面
    
    USART_SendData(USART1,data); //可通过 串口助手在电脑屏幕上 显示
  }
  if(USART_GetITStatus(USART2, USART_IT_IDLE) == 1) //串口空闲中断
  {
      data = USART2->SR;
    
      data = USART2->DR;
    
      wifi.rxover = 1;
  }
}
/**
  * @brief  发送字符串
  * @param  string 字符串
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
  * @brief  发送字符串 + 带长度
  * @param  
  * @retval 
  */
void Esp8266_SendString(uint8_t *string,uint16_t len)//串口3 发送字符串
{
  for(uint16_t i=0;i<len;i++)
  Usart2_SendByte(string[i]);
}

/**
  * @brief  发送AT指令，检查AT指令的返回，判断AT指令是否发送成功
  * @param  cmd:发送的AT指令
  * @param  Rcmd:AT指令对应的返回值
  * @param  outtime:ESP8266超时回复判断
  * @retval 
  */
// 在文件顶部添加声明
uint8_t EspSendCmdAndCheckRecvData(const char *cmd, const char *Rcmd, uint16_t outtime)
{
  uint8_t data = 0;
  
  //发送AT指令之前要清空
  
  memset(wifi.rxbuff,0,1024);
  
  wifi.rxover = 0;
  
  wifi.rxcount = 0;
  
  Usart2_SendString(cmd);//发送AT指令 -- > 才会进入串口2中断服务
  
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
  * @brief  连接淘宝API 获取当前时间
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
                    
                    // 发送正确的HTTP请求
                    Usart2_SendString("GET /rest/api3.do?api=mtop.common.getTimestamp HTTP/1.1\r\n");
                    Usart2_SendString("Host: api.m.taobao.com\r\n");
                    Usart2_SendString("Connection: close\r\n\r\n");
                    
                    Delay_ms(2000); // 等待响应
                              //清空回收的buff
          memset(wifi.rxbuff,0,1024);
  
          wifi.rxover = 0;
          
          wifi.rxcount = 0;
                    
//          Delay_ms(1000);
//          Usart2_SendString("GET http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp\r\n");
//          Delay_ms(1000);
                    // 正确退出透传
                    Delay_ms(1000);
                    Usart2_SendString("+++");
                    Delay_ms(1000);
                }
            }
        }
    }
}






/**
  * @brief  解析JSON数据
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
//    if(wifi.rxcount>10)//判断是否是 下发指令的回传的指令 而不是上传时系统默认下发的 //防止有影响
//    {
//    char *addr =  strstr((char *)(wifi.rxbuff+6),"\"t\"");
//    addr+=5;
//    while(*(addr + i) != '"')
//    {
//      Val[i] = *( addr+i);
//      i++;
//    }
////    printf("%s\r\n",Val);
////    sscanf(Val, "%lld", &timeget);  // 存储类型为long long int
////    printf("%lld\r\n",timeget);
//    
//    for(i = 0;i<10;i++) //这种要去掉3位 才能使用atoi转换
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
//	// 跳过HTTP头，定位JSON起始位置
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
//			// 转换13位毫秒时间戳为秒
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

    // 计算时分秒
    dt.second = seconds % 60;
    seconds /= 60;
    dt.minute = seconds % 60;
    seconds /= 60;
    dt.hour = seconds % 24;
    seconds /= 24;

    // 计算年月日（基于1970-01-01）
    int year = 1970;
    while (seconds >= (IsLeapYear(year) ? 366 : 365)) {
        seconds -= IsLeapYear(year) ? 366 : 365;
        year++;
    }
    dt.year = year;

    // 计算月份
    uint8_t month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (IsLeapYear(year)) month_days[1] = 29;
  
    dt.month = 1;
    while (seconds >= month_days[dt.month-1]) {
        seconds -= month_days[dt.month-1];
        dt.month++;
    }
    dt.day = seconds + 1; // 天数从1开始

    return dt;
}
void GetTime_RecvData(void) {
    if (wifi.rxover == 1) {
        wifi.rxover = 0;
      
        // 定位JSON起始位置
        char *json_start = strchr((char*)wifi.rxbuff, '{');
        if (json_start) {
            cJSON *root = cJSON_Parse(json_start);
            if (root) {
                cJSON *data = cJSON_GetObjectItem(root, "data");
                if (data) {
                    cJSON *t = cJSON_GetObjectItem(data, "t");
                    if (t && cJSON_IsString(t)) {
                        // 转换时间戳
                        uint64_t timestamp = strtoull(t->valuestring, NULL, 10);
                        timestamp /= 1000; // 毫秒转秒
                      
                        // 设置RTC
                        RTC_Init((uint32_t)timestamp);
                      
                        // 转换并打印时间
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




// 时区偏移（单位：小时）

	DateTime ConvertLocalTime(uint32_t utc_timestamp) {
    uint32_t local_seconds = utc_timestamp + TIMEZONE_OFFSET * 3600;
    DateTime dt = ConvertUnixTime(local_seconds);
  
    // 处理跨天
    if (dt.hour >= 24) {
        dt.hour -= 24;
        dt.day += 1;
        // 处理跨月跨年（需补充完整逻辑）
    }
    return dt;
}

