#ifndef __MQTT_H
#define __MQTT_H

#include <stdint.h>


void MQTT_WIFI_Init(void);
void send_json(float t, float h, int c, int v, double lng, double lat, const char *ts) ;
// 超时、重发次数、回应字符串
void MQTT_SendCommand(const char *command, uint32_t *timeout, uint8_t *retries, const char *expectedResponse);


// 串口接收的缓冲区
extern uint8_t Uart_Buffer_3[1];

#endif
