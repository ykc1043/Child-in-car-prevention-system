#include "mqtt.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include "string.h"
#include "DataProc.h"  // 需要定义环形缓冲区操作函数
#include "onenet.h"
static const char *AT_COMMANDS[] = {
    "AT+RESTORE\r\n",
	"ATE0\r\n",
    "AT+CWMODE=1\r\n",
    "AT+CWJAP=\"wifissid\",\"wifipassword\"\r\n",
    "AT+MQTTUSERCFG=0,1,\"clientId\",\"username\",\"password\",0,0,\"\"\r\n",
    "AT+MQTTCONN=0,\"pve.afrain.top\",1883,0\r\n",
    "AT+MQTTCONN?\r\n",
	"AT+MQTTSUB=0,\"emq\",1\r\n",
	"AT+MQTTPUB=0,\"emq\",\"MQOK\",1,0\r\n"
};
// 发送 AT 指令的封装函数
void send_command(const char *cmd) {
    HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
    //printf("%s",cmd);
}

void MQTT_WIFI_Init(void) {
    char at_cwjap_cmd[128]; // 用于存储修改后的 AT+CWJAP 命令
	HAL_UART_Receive_IT(&huart3, (uint8_t *)Uart_Buffer_3, 1);

    // 生成 Wi-Fi 连接命令
    snprintf(at_cwjap_cmd, sizeof(at_cwjap_cmd),
             "AT+CWJAP=\"%s\",\"%s\"\r\n",
             sysConfig.wifi_ssid, sysConfig.wifi_password);
	send_command(AT_COMMANDS[0]);HAL_Delay(500);
	send_command(AT_COMMANDS[1]);HAL_Delay(500);
	send_command(AT_COMMANDS[2]);HAL_Delay(500);
	send_command(at_cwjap_cmd);  HAL_Delay(5000);
	send_command(AT_COMMANDS[4]);HAL_Delay(500);
	send_command(AT_COMMANDS[5]);HAL_Delay(500);
	send_command(AT_COMMANDS[6]);HAL_Delay(500);
	send_command(AT_COMMANDS[7]);HAL_Delay(999);
	send_command(AT_COMMANDS[8]);HAL_Delay(500);
    // 依次发送 AT 指令

}
void send_json(float t, float h, int c, int v, double lng, double lat, const char *ts) {
    char json[256];
    
    snprintf(json, sizeof(json),
             "{\"t\": %.1f, \"h\": %.1f, \"c\": %d, \"v\": %d, \"lng\": %.4f, \"lat\": %.5f, \"ts\": \"%s\"}",
             t, h, c, v, lng, lat, ts);
    OneNet_Publish("RX",json);
//    printf("%s\r\n", json);
}
void MQTT_SendCommand(const char *command, uint32_t *timeout, uint8_t *retries, const char *expectedResponse)
{
    uint32_t startTick = HAL_GetTick();

	HAL_UART_Transmit(&huart3, (uint8_t *)command, strlen(command), HAL_MAX_DELAY);
}

