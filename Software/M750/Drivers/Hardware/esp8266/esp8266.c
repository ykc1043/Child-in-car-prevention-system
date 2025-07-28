/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "main.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>

#include "DataProc.h"

#define ESP8266_WIFI_INFO		"AT+CWJAP_CUR=\"wifissid\",\"wifipwd\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"pve.afrain.top\",1883\r\n"


unsigned char esp8266_buf[512];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

//	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen((const char *)cmd), HAL_MAX_DELAY);
	printf("\033[32m%s \033[0m", cmd);

	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================

void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
//		Usart_SendString(USART2, data, len);		//�����豸������������
		HAL_UART_Transmit(&huart3, (uint8_t *)data, len, HAL_MAX_DELAY);
//		printf("\033[36mbuf:%s\033[0m",esp8266_buf);
//		printf("\033[33m");
//		for(int i=0;i<len;i++)
//		{
//			printf("%02X ",data[i]);
//			if(i==len-1)printf("\r\n");
//		}
//		printf(" \033[0m");
	}

//	HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", strlen((const char *)"\r\n"), HAL_MAX_DELAY);

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
//				USART_Printf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//��ʱ��?
		timeOut--;
	} while(timeOut>0);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	falseʧ��, true�ɹ�
//
//	˵����		
//==========================================================

uint8_t ESP8266_Init(void)
{
    ESP8266_Clear();

    USART_Printf(USART_DEBUG, "0. AT\r\n");
    int retry_count = 0;
    while (ESP8266_SendCmd("AT\r\n", "OK"))
    {
        if (++retry_count > 3) {
            USART_Printf(USART_DEBUG, "Error: ESP8266 Init Failed (AT)\r\n");
            return 0;
        }
        delay_ms(250);
    }

    USART_Printf(USART_DEBUG, "1. RST\r\n");
    ESP8266_SendCmd("AT+RST\r\n", "");
    delay_ms(250);

    ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
    delay_ms(250);

    USART_Printf(USART_DEBUG, "2. CWMODE\r\n");
    retry_count = 0;
    while (ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
    {
        if (++retry_count > 3) {
            USART_Printf(USART_DEBUG, "Error: ESP8266 Init Failed (CWMODE)\r\n");
            return 0;
        }
        delay_ms(250);
    }

    USART_Printf(USART_DEBUG, "3. AT+CWDHCP\r\n");
    retry_count = 0;
    while (ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
    {
        if (++retry_count > 3) {
            USART_Printf(USART_DEBUG, "Error: ESP8266 Init Failed (CWDHCP)\r\n");
            return 0;
        }
        delay_ms(250);
    }

    char at_cwjap_cmd[128];
    snprintf(at_cwjap_cmd, sizeof(at_cwjap_cmd),
             "AT+CWJAP=\"%s\",\"%s\"\r\n",
             sysConfig.wifi_ssid, sysConfig.wifi_password);

    USART_Printf(USART_DEBUG, "4. CWJAP\r\n");
    retry_count = 0;
    while (ESP8266_SendCmd(at_cwjap_cmd, "GOT IP"))
    {
        if (++retry_count > 3) {
            USART_Printf(USART_DEBUG, "Error: ESP8266 Init Failed (CWJAP)\r\n");
            return 0;
        }
        delay_ms(250);
    }

    USART_Printf(USART_DEBUG, "5. CIPSTART\r\n");
    retry_count = 0;
    while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
    {
        if (++retry_count > 3) {
            USART_Printf(USART_DEBUG, "Error: ESP8266 Init Failed (CIPSTART)\r\n");
            return 0;
        }
        delay_ms(250);
    }

    USART_Printf(USART_DEBUG, "6. ESP8266 Init OK\r\n");
    return 1;
}
// void ESP8266_Init(void)
// {

// 	ESP8266_Clear();
	
// 	USART_Printf(USART_DEBUG, "0. AT\r\n");
// 	while(ESP8266_SendCmd("AT\r\n", "OK"))
// 		delay_ms(250);
	
// 	USART_Printf(USART_DEBUG, "1. RST\r\n");
// 	ESP8266_SendCmd("AT+RST\r\n", "");
// 	delay_ms(250);
	
// 	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
// 	delay_ms(250);
	
// 	USART_Printf(USART_DEBUG, "2. CWMODE\r\n");
// 	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
// 		delay_ms(250);
	
// 	USART_Printf(USART_DEBUG, "3. AT+CWDHCP\r\n");
// 	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
// 		delay_ms(250);
	
// 	char at_cwjap_cmd[128];
//     snprintf(at_cwjap_cmd, sizeof(at_cwjap_cmd),
//              "AT+CWJAP=\"%s\",\"%s\"\r\n",
//              sysConfig.wifi_ssid, sysConfig.wifi_password);
	
// 	USART_Printf(USART_DEBUG, "4. CWJAP\r\n");
// //	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
// 	while(ESP8266_SendCmd(at_cwjap_cmd, "GOT IP"))
// 		delay_ms(250);
	
	
// 	USART_Printf(USART_DEBUG, "5. CIPSTART\r\n");
// 	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
// 		delay_ms(250);
	
// 	USART_Printf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

// }

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����3�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
//void USART2_IRQHandler(void)
//{

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
//	{
//		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
//		esp8266_buf[esp8266_cnt++] = USART2->DR;
//		
//		USART_ClearFlag(USART2, USART_FLAG_RXNE);
//	}

//}
