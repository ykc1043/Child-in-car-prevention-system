// DP_SysConfig.c
#include "DataProc.h"
#include "DataProc_Def.h"
#include "sdmmc_sd.h" //不使用cubemx生成的sdmmc.c
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void DP_ConfigSave(void);
void DP_ConfigLoad(void);



void DP_InitDefaultConfig(void)
{
    strcpy(sysConfig.wifi_ssid, "default_ssid");
    strcpy(sysConfig.wifi_password, "default_password");
	strcpy(sysConfig.phone_number, "12345678901");
    sysConfig.screen_brightness = 100;
    sysConfig.volume = 10;
    sysConfig.latitude = 0.0f;
    sysConfig.longitude = 0.0f;
}
void DP_InitDefaultData(void)
{
    
    sysData.init_status=false;
    sysData.network_status=false;
	sysData.network_is_gettime=false;
    sysData.timestamp=946684800-8*3600;
	
    sysData.CO2=0;
    sysData.TVOC=0;
    sysData.humidity=0.0f;
    sysData.temperature=0.0f;
	sysData.personNumber=0;
}

void DP_SysConfig_Init()
{

    FATFS fs;    // 文件系统对象
    FILINFO fno; // 文件信息对象
    FRESULT res; // 文件系统操作结果

    // 挂载文件系统
    //    res = f_mount(&fs, "", 1);
    //    if (res != FR_OK)
    //    {
    //        // 挂载失败的错误处理（例如日志记录或错误代码处理）
    //		printf("DP:mount failed\r\n");
    //        return;
    //    }

    // 检查文件是否存在
    res = f_stat(DP_CONFIG_FILE, &fno);
    if (res == FR_OK)
    {
        // 文件存在
        printf("DP:config file found\r\n");
		DP_ConfigLoad();

    }
    else if (res == FR_NO_FILE)
    {
        // 文件不存在，初始化默认配置
        DP_InitDefaultConfig();
        printf("DP:config file not found, use default\r\n");
        DP_ConfigSave();
    }
    else
    {
        // 其他错误处理
    }

    // 卸载文件系统
    //    f_mount(NULL, "", 1);
}
int DP_SaveConfigToSD(const char *filename)
{
    FIL file;
    FRESULT res;
    cJSON *json = cJSON_CreateObject();

    if (!json)
        return -1;

    cJSON_AddStringToObject(json, "wifi_ssid", sysConfig.wifi_ssid);
    cJSON_AddStringToObject(json, "wifi_password", sysConfig.wifi_password);
	cJSON_AddStringToObject(json, "phone_number", sysConfig.phone_number);
    cJSON_AddNumberToObject(json, "screen_brightness", sysConfig.screen_brightness);
    cJSON_AddNumberToObject(json, "volume", sysConfig.volume);

    // 格式化 latitude 和 longitude 为字符串，保留 8 位有效数字
    char latitude_str[16];
    char longitude_str[16];
    snprintf(latitude_str, sizeof(latitude_str), "%.8g", sysConfig.latitude);
    snprintf(longitude_str, sizeof(longitude_str), "%.8g", sysConfig.longitude);

    cJSON_AddStringToObject(json, "latitude", latitude_str);
    cJSON_AddStringToObject(json, "longitude", longitude_str);

    char *jsonString = cJSON_Print(json);
    if (!jsonString)
    {
        cJSON_Delete(json);
        return -1;
    }

    res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK)
    {
        free(jsonString);
        cJSON_Delete(json);
        return -1;
    }

    UINT written;
    f_write(&file, jsonString, strlen(jsonString), &written);
    f_close(&file);

    free(jsonString);
    cJSON_Delete(json);

    return 0;
}

int DP_LoadConfigFromSD(const char *filename)
{
    FIL file;
    FRESULT res;
    char buffer[512];
    UINT bytesRead;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK)
        return -1;

    res = f_read(&file, buffer, sizeof(buffer) - 1, &bytesRead);
    f_close(&file);

    if (res != FR_OK || bytesRead == 0)
        return -1;

    buffer[bytesRead] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    if (!json)
        return -1;

    cJSON *item;
    if ((item = cJSON_GetObjectItem(json, "wifi_ssid")))
    {
        strncpy(sysConfig.wifi_ssid, item->valuestring, sizeof(sysConfig.wifi_ssid));
    }
    if ((item = cJSON_GetObjectItem(json, "wifi_password")))
    {
        strncpy(sysConfig.wifi_password, item->valuestring, sizeof(sysConfig.wifi_password));
    }
    if ((item = cJSON_GetObjectItem(json, "phone_number")))
    {
        strncpy(sysConfig.phone_number, item->valuestring, sizeof(sysConfig.phone_number));
    }
    if ((item = cJSON_GetObjectItem(json, "screen_brightness")))
    {
        sysConfig.screen_brightness = item->valueint;
    }
    if ((item = cJSON_GetObjectItem(json, "volume")))
    {
        sysConfig.volume = item->valueint;
    }
    if ((item = cJSON_GetObjectItem(json, "latitude")))
    {
        sysConfig.latitude = (float)atof(item->valuestring);
    }
    if ((item = cJSON_GetObjectItem(json, "longitude")))
    {
        sysConfig.longitude = (float)atof(item->valuestring);
    }

    cJSON_Delete(json);
    return 0;
}

void DP_UpdateWiFiConfig(const char *ssid, const char *password)
{
    strncpy(sysConfig.wifi_ssid, ssid, sizeof(sysConfig.wifi_ssid));
    strncpy(sysConfig.wifi_password, password, sizeof(sysConfig.wifi_password));
}

void DP_UpdatePhoneNumber(const char *phone_number)
{
    strncpy(sysConfig.phone_number, phone_number, sizeof(sysConfig.phone_number));
}

void DP_UpdateScreenBrightness(int brightness)
{
    sysConfig.screen_brightness = brightness;
}

void DP_UpdateVolume(int volume)
{
    sysConfig.volume = volume;
}

void DP_UpdateLocation(float latitude, float longitude)
{
    sysConfig.latitude = latitude;
    sysConfig.longitude = longitude;
}

void DP_ConfigSave(void)
{
    if (DP_SaveConfigToSD(DP_CONFIG_FILE) == 0)
    {
        printf("Config saved successfully!\r\n");
    }
    else
    {
        printf("Failed to save config!\r\n");
    }
}
void DP_ConfigLoad(void)
{
        if (DP_LoadConfigFromSD(DP_CONFIG_FILE) == 0)
        {
            printf("Config loaded successfully!\r\n");
        }
        else
        {
            printf("Failed to load config!\r\n");
        }
}

