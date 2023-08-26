/**
	************************************************************
	************************************************************
	************************************************************
	*	ÎÄ¼þÃû£º 	esp8266.c
	*
	*	×÷Õß£º 		ÕÅ¼ÌÈð
	*
	*	ÈÕÆÚ£º 		2017-05-08
	*
	*	°æ±¾£º 		V1.0
	*
	*	ËµÃ÷£º 		ESP8266µÄ¼òµ¥Çý¶¯
	*
	*	ÐÞ¸Ä¼ÇÂ¼£º	
	************************************************************
	************************************************************
	************************************************************
**/

//µ¥Æ¬»úÍ·ÎÄ¼þ
#include "stm32f10x.h"

//ÍøÂçÉè±¸Çý¶¯
#include "esp8266.h"

//Ó²¼þÇý¶¯
#include "delay.h"
#include "usart.h"

//C¿â
#include <string.h>
#include <stdio.h>


#define ESP8266_WIFI_INFO		"AT+CWJAP=\"A92s\",\"12345678\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"192.168.114.250\",1883\r\n"


unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_Clear
//
//	º¯Êý¹¦ÄÜ£º	Çå¿Õ»º´æ
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_WaitRecive
//
//	º¯Êý¹¦ÄÜ£º	µÈ´ý½ÓÊÕÍê³É
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	REV_OK-½ÓÊÕÍê³É		REV_WAIT-½ÓÊÕ³¬Ê±Î´Íê³É
//
//	ËµÃ÷£º		Ñ­»·µ÷ÓÃ¼ì²âÊÇ·ñ½ÓÊÕÍê³É
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//Èç¹û½ÓÊÕ¼ÆÊýÎª0 ÔòËµÃ÷Ã»ÓÐ´¦ÓÚ½ÓÊÕÊý¾ÝÖÐ£¬ËùÒÔÖ±½ÓÌø³ö£¬½áÊøº¯Êý
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//Èç¹ûÉÏÒ»´ÎµÄÖµºÍÕâ´ÎÏàÍ¬£¬ÔòËµÃ÷½ÓÊÕÍê±Ï
	{
		esp8266_cnt = 0;							//Çå0½ÓÊÕ¼ÆÊý
			
		return REV_OK;								//·µ»Ø½ÓÊÕÍê³É±êÖ¾
	}
		
	esp8266_cntPre = esp8266_cnt;					//ÖÃÎªÏàÍ¬
	
	return REV_WAIT;								//·µ»Ø½ÓÊÕÎ´Íê³É±êÖ¾

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_SendCmd
//
//	º¯Êý¹¦ÄÜ£º	·¢ËÍÃüÁî
//
//	Èë¿Ú²ÎÊý£º	cmd£ºÃüÁî
//				res£ºÐèÒª¼ì²éµÄ·µ»ØÖ¸Áî
//
//	·µ»Ø²ÎÊý£º	0-³É¹¦	1-Ê§°Ü
//
//	ËµÃ÷£º		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//Èç¹ûÊÕµ½Êý¾Ý
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//Èç¹û¼ìË÷µ½¹Ø¼ü´Ê
			{
				ESP8266_Clear();									//Çå¿Õ»º´æ
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_SendData
//
//	º¯Êý¹¦ÄÜ£º	·¢ËÍÊý¾Ý
//
//	Èë¿Ú²ÎÊý£º	data£ºÊý¾Ý
//				len£º³¤¶È
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//Çå¿Õ½ÓÊÕ»º´æ
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//·¢ËÍÃüÁî
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//ÊÕµ½¡®>¡¯Ê±¿ÉÒÔ·¢ËÍÊý¾Ý
	{
		Usart_SendString(USART2, data, len);		//·¢ËÍÉè±¸Á¬½ÓÇëÇóÊý¾Ý
	}

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_GetIPD
//
//	º¯Êý¹¦ÄÜ£º	»ñÈ¡Æ½Ì¨·µ»ØµÄÊý¾Ý
//
//	Èë¿Ú²ÎÊý£º	µÈ´ýµÄÊ±¼ä(³ËÒÔ10ms)
//
//	·µ»Ø²ÎÊý£º	Æ½Ì¨·µ»ØµÄÔ­Ê¼Êý¾Ý
//
//	ËµÃ÷£º		²»Í¬ÍøÂçÉè±¸·µ»ØµÄ¸ñÊ½²»Í¬£¬ÐèÒªÈ¥µ÷ÊÔ
//				ÈçESP8266µÄ·µ»Ø¸ñÊ½Îª	"+IPD,x:yyy"	x´ú±íÊý¾Ý³¤¶È£¬yyyÊÇÊý¾ÝÄÚÈÝ
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//Èç¹û½ÓÊÕÍê³É
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//ËÑË÷¡°IPD¡±Í·
			if(ptrIPD == NULL)											//Èç¹ûÃ»ÕÒµ½£¬¿ÉÄÜÊÇIPDÍ·µÄÑÓ³Ù£¬»¹ÊÇÐèÒªµÈ´ýÒ»»á£¬µ«²»»á³¬¹ýÉè¶¨µÄÊ±¼ä
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//ÕÒµ½':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//ÑÓÊ±µÈ´
		timeOut--;
	} while(timeOut>0);
	
	return NULL;														//³¬Ê±»¹Î´ÕÒµ½£¬·µ»Ø¿ÕÖ¸Õë

}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_Init
//
//	º¯Êý¹¦ÄÜ£º	³õÊ¼»¯ESP8266
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//ESP8266¸´Î»Òý½Å
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-¸´Î»
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
	delay_ms(250);
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
	delay_ms(500);
	
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "0. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "1. RST\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "");
	delay_ms(500);
	
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
	delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

//==========================================================
//	º¯ÊýÃû³Æ£º	USART2_IRQHandler
//
//	º¯Êý¹¦ÄÜ£º	´®¿Ú3ÊÕ·¢ÖÐ¶Ï
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //½ÓÊÕÖÐ¶Ï
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //·ÀÖ¹´®¿Ú±»Ë¢±¬
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}
