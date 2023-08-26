#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "esp8266.h"
#include "MqttKit.h"
#include "onenet.h"
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
int fputc(int ch, FILE *f)
{
USART_SendData(USART1, (unsigned char) ch);
while (!(USART1->SR & USART_FLAG_TXE));
return (ch);
}
char xinxi_id[256];
char PUB_BUF[256];//
int main(void)
 {	
	const char *topics[] = {"keys001"};
	
	unsigned short timeCount = 0;	
	
	unsigned char *dataPtr = NULL;
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);								
	Usart1_Init(115200);						
	Usart2_Init(115200);														
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	LED_Init();		  	//初始化与LED连接的硬件接口
	
	ESP8266_Init();	
	while(OneNet_DevLink())			
	delay_ms(500);
	OneNet_Subscribe(topics, 1);

	while(1)
	{
		if(++timeCount >= 500)								
		{
			UsartPrintf(USART_DEBUG, "EMQX_Publish\r\n");
			 strcpy(xinxi_id,"ID20220710");
			sprintf(PUB_BUF,"{\"Student ID\":%s}",xinxi_id);
			OneNet_Publish("keys001server",PUB_BUF);
			OneNet_Publish("keys001server", "Students 10 resistors");
			timeCount = 0;
			ESP8266_Clear();
		}		
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		delay_ms(10);

	}
 }

