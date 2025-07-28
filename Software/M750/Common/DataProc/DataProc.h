// DataProc.h
#ifndef DATAPROC_H
#define DATAPROC_H

#include <stdint.h>
#include <stdbool.h>
#include "rtc.h"
// ����ϵͳ���ýṹ��
typedef struct {
    char wifi_ssid[32];
    char wifi_password[32];
	char phone_number[16];
    int screen_brightness;
    int volume;
    float latitude;
    float longitude;
} SystemConfig;

// ȫ��ϵͳ���ñ���
extern SystemConfig sysConfig;

// ����ȫ������ṹ��
#define SEAT_DRIVER     (1 << 0)  // ��ʻ��
#define SEAT_PASSENGER  (1 << 1)  // ����ʻ��
#define SEAT_REAR_LEFT  (1 << 2)  // �������
#define SEAT_REAR_RIGHT (1 << 3)  // �����Ҳ�

typedef struct {
	RTC_TimeTypeDef sTime;
	
} date;
typedef struct {
    bool init_status;
    bool network_status;
	bool network_is_gettime;
    uint64_t timestamp;
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
    uint16_t CO2;
    uint16_t TVOC;
    float temperature;
    float humidity;
	uint8_t personNumber;
} SystemData;

// ȫ��ϵͳ����
extern SystemData sysData;

// ��������
void DataProc_Init(void);
void DP_InitDefaultConfig(void);
void DP_InitDefaultData(void);

////DP_SysConfig
void DP_SysConfig_Init(void);
int DP_SaveConfigToSD(const char *filename);
int DP_LoadConfigFromSD(const char *filename);
void DP_UpdateWiFiConfig(const char *ssid, const char *password);
void DP_UpdatePhoneNumber(const char *phone_number);
void DP_UpdateScreenBrightness(int brightness);
void DP_UpdateVolume(int volume);
void DP_UpdateLocation(float latitude, float longitude);
void DP_ConfigSave(void);
void DP_ConfigLoad(void);

#endif // DATAPROC_H
