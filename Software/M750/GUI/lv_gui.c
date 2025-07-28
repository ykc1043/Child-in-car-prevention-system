/**
 * @file lv_GUI.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gui.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "lvgl.h"
#include "usart.h"
#include "rtc.h"
#include "tim.h"

#include "DataProc.h"
#include "myiic.h"
#include "sgp30.h"
#include "mqtt.h"
#include "esp8266.h"
#include "onenet.h"
#include "gsm.h"
/*********************
 *      DEFINES
 *********************/
#define M_PI 3.14
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

// ����һ��ȫ�ֵ�ͼ����������ϵ��
static lv_obj_t * chart;
static lv_chart_series_t * series;



/**********************
 *      MACROS
 **********************/
// ǰ������ update_chart ����
void update_chart(lv_timer_t * timer);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    void **user_data = lv_event_get_user_data(e);
    lv_obj_t *popup = (lv_obj_t *)user_data[0];
    uint8_t *val = (uint8_t *)user_data[1];
    lv_timer_t *beep_timer = (lv_timer_t *)user_data[2];

    int16_t value = lv_slider_get_value(slider);
    int16_t max_value = lv_slider_get_max_value(slider);

    // ���鵽�����Ҷ�ʱ�رյ���
    if (value >= max_value) {
        *val = 1;
        if (beep_timer) {
            lv_timer_del(beep_timer);
            Beep_Disable();
        }
        HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,GPIO_PIN_RESET);
        lv_obj_del(popup);
    }
}

void show_popup_with_pad(const char *message,const char *slidemsg,uint8_t *val,lv_timer_t *beep_timer)
{
    // ��������
    lv_obj_t *popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(popup, 310, 200);
    lv_obj_set_style_bg_color(popup, lv_color_hex(0xFF0000), LV_PART_MAIN); // ��ɫ����
    lv_obj_align(popup, LV_ALIGN_CENTER, 0, 0);

    // ��Ӵ�����Ϣ�ı�
    lv_obj_t *label = lv_label_create(popup);
    lv_label_set_text(label, message);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // ��ɫ����
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // ��������
    lv_obj_t *slider = lv_slider_create(popup);
    lv_obj_set_size(slider, 260, 40);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x777777), LV_PART_MAIN); // ��ɫ����
    lv_obj_set_style_radius(slider, 20, LV_PART_MAIN);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -20);

    // �����ֱ���ʽ (��ɫ)
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 20, LV_PART_INDICATOR);
    lv_obj_set_style_pad_ver(slider, 10, LV_PART_INDICATOR);

    // �����ϵ��ı�
    lv_obj_t *slider_label = lv_label_create(slider);
    lv_label_set_text(slider_label, slidemsg);
    lv_obj_set_style_text_color(slider_label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 0);

    static void *user_data[3]; 
    user_data[0]=popup;
    user_data[1]=val;
    user_data[2]=beep_timer;
    // �󶨻����¼�
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, user_data);
}
/*�����������������������������������������������������������������������������¼����� Լ130�С���������������������������������������������������������������������������������������������������������������*/
void enable_button_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *btn = (lv_obj_t *)timer->user_data; // ��ȡ��Ҫ���õİ�ť
    lv_obj_clear_state(btn, LV_STATE_DISABLED);   // �������״̬
    lv_timer_del(timer);                          // ɾ����ʱ��
}
void heat_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // ��ȡ�����¼��İ�ť
    printf("Heat button clicked!\r\n");

    SHT_Write_Len(SHT_IIC_ADDR, SHT40_HEATER_200mW_1s, 1, NULL);

    // ���ð�ť
    lv_obj_add_state(btn, LV_STATE_DISABLED);

    // ����һ����ʱ������ 1 ������ð�ť
    lv_timer_t *timer = lv_timer_create(enable_button_timer_cb, 1000, btn);
}

void sgp_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // ��ȡ�����¼��İ�ť
    printf("id button clicked!\r\n");
	uint8_t id[6];
	sgp30_get_serial_id(id);
	printf("%02x %02x %02x %02x %02x %02x\r\n",id[0],id[1],id[2],id[3],id[4],id[5]);
    

}


void gps_btn_event_cb(lv_event_t *e)
{
    printf("gps button clicked!\r\n");
    lv_obj_t *btn = lv_event_get_target(e); // ��ȡ�����¼��İ�ť

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    sTime.Hours = 0x14;
    sTime.Minutes = 0x00;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
      Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x5;
    sDate.Year = 0x1;
  
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
      Error_Handler();
    }
    // show_popup_with_pad("test");
}


void update_gps_info(lv_timer_t *timer) {
#if !LV_USE_GUIDER_SIMULATOR
//    HAL_UART_Receive_IT(&huart4, (uint8_t *)UART4_temp, REC_LENGTH);
    char status[20],t3[20],latitude[20], longitude[20];
    
    
    // ���״̬��
    if (UART4_Rx_flg == 1) {
        UART4_Rx_flg = 0;

        // ����GPS״̬
        if (gps.UsefulFlag == 'V') {
            strcpy(status, "Disconnected");

            // ���û������GPS����ϵͳ�����л�ȡ��γ��
            sprintf(latitude, "%.5f", sysConfig.latitude);
            sprintf(longitude, "%.5f", sysConfig.longitude);
        } else if (gps.UsefulFlag == 'A') {
            strcpy(status, "Connected");
			DP_UpdateLocation(gps.latitude_d,gps.longitude_d);
            // ����γ��
            sprintf(latitude, "%.5f", gps.latitude_d);
            if (gps.N_S == 'N') {
                strcat(latitude, " N");
            } else if (gps.N_S == 'S') {
                strcat(latitude, " S");
            }

            // ���þ���
            sprintf(longitude, "%.5f", gps.longitude_d);
            if (gps.E_W == 'E') {
                strcat(longitude, " E");
            } else if (gps.E_W == 'W') {
                strcat(longitude, " W");
            }
        }
        sprintf(t3,"%02d:%02d:%02d",gps.UTCTime_int/10000,gps.UTCTime_int/100%100,gps.UTCTime_int%100);
        // ���±�ǩ
        lv_obj_t *gps_label = (lv_obj_t *)timer->user_data;
        char gps_info[100];
        sprintf(gps_info, "GPS: %s\nCNTime: %s\nLatitude: %s\nLongitude: %s", status, t3,latitude, longitude);
        lv_label_set_text(gps_label, gps_info);
    }
#endif
}
// ��ʱ���ص�����
void update_sht40_task(lv_timer_t *timer)
{

    // ��ȡ��ʪ������
    SHT40_Read_RHData(&sysData.temperature, &sysData.humidity);
    if (sysData.temperature > 100 || sysData.humidity > 100)
        return;
    // ���±�ǩ��ʾ����
    static char buf[32];
    snprintf(buf, sizeof(buf), "Temp: %.2f ��C\nHumi: %.2f %%", sysData.temperature, sysData.humidity);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);
}
void update_sgp30_task(lv_timer_t *timer)
{

    sgp30_read(&sysData.CO2,&sysData.TVOC);
    static char buf[32];
    snprintf(buf, sizeof(buf), "CO2 : %d ppm\nTVOC: %d ppb", sysData.CO2,sysData.TVOC);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);
}
/*�����������������������������������������������������������������������������¼������������������������������������������������������������������������������������������������������������������*/

/*����������������������������������������������������������������������web���ͨ�Ŵ��� Լ60�С���������������������������������������������������������������������������������������������������������������*/
void update_mqtt(lv_timer_t *timer)
{
    /*ʹ��ϵͳunixʱ��*/
        // �� sysData.timestamp��uint64_t ���ͣ�ת��Ϊ��ʽ��ʱ���ַ���
        // char time_str[32] = {0};
        // if (sysData.timestamp > 0)
        // {
        //     // �� uint64_t ת��Ϊ time_t��Unix ʱ�����
        //     time_t t = (time_t)sysData.timestamp+3600*8;
        //     // ʹ�� localtime() ��ȡ����ʱ�䣨���� UTC ʱ�䣬���� gmtime(&t)��
        //     struct tm *tm_info = localtime(&t);
        //     if(tm_info != NULL)
        //     {
        //         if(strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%S", tm_info) == 0)
        //         {
        //             strcpy(time_str, "FormatError");
        //         }
        //     }
        //     else
        //     {
        //         strcpy(time_str, "InvalidTime");
        //     }
        // }
        // else
        // {
        //     strcpy(time_str, "No ts field");
        // }
    /*ʹ��rtcʱ��*/
    RTC_TimeTypeDef GetTime;
    RTC_DateTypeDef GetData;
    
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

    char time_str[64];
    snprintf(time_str, sizeof(time_str), "%04d-%02d-%02dT%02d:%02d:%02d",
    2000 + GetData.Year, GetData.Month, GetData.Date,
    GetTime.Hours, GetTime.Minutes, GetTime.Seconds);


    // ���� send_json ʱ����ת�����ʱ���ַ�������
    send_json(sysData.temperature,
                sysData.humidity,
                sysData.CO2,
                sysData.TVOC,
                sysConfig.longitude,
                sysConfig.latitude,
                time_str);


    ESP8266_Clear();

}
/*����������������������������������������������������������������������web���ͨ�Ŵ����������������������������������������������������������������������������������������������������������������*/

/*��������������������������������������������������������������������ʱ��ͬ������ Լ200�С���������������������������������������������������������������������������������������������������������*/
void time_rtc_to_sys(void)
{
    RTC_TimeTypeDef rtcTime;
    RTC_DateTypeDef rtcDate;
    
    // ��ȡ RTC ��ǰʱ������ڣ��Զ����Ƹ�ʽ��
    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
    
    // ���� struct tm �ṹ��
    struct tm timeinfo;
    timeinfo.tm_year = (2000 + rtcDate.Year) - 1900;  // RTC ��ݴ洢Ϊ��ƫ��2000����tm_year ��Ҫ��1900��ʼ����
    timeinfo.tm_mon  = rtcDate.Month - 1;             // tm_mon ��Χ 0~11
    timeinfo.tm_mday = rtcDate.Date;
    timeinfo.tm_hour = rtcTime.Hours;
    timeinfo.tm_min  = rtcTime.Minutes;
    timeinfo.tm_sec  = rtcTime.Seconds;
    timeinfo.tm_isdst = -1; // ��ʹ������ʱ�ж�

    // ������ʱ��ת��Ϊ Unix ʱ�����UTC������Ϊ RTC ����Ǳ���ʱ�䣬���Լ�ȥ8Сʱ��8*3600�룩
    time_t unix_time = mktime(&timeinfo) - 8 * 3600;
    sysData.timestamp = (uint64_t)unix_time;
}

void time_sys_to_rtc(void)
{
    // ��ϵͳʱ�䣨UTC��ת��Ϊ����ʱ��
    time_t utc_time = (time_t)sysData.timestamp;
    time_t local_time = utc_time + 8 * 3600;  // ����ʱ�������������Զ�����Ϊ����
    struct tm *tm_info = localtime(&local_time);
    if (tm_info == NULL)
    {
        // ת��ʧ�ܣ�������
        return;
    }
    
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    
    // ��� RTC ʱ��ṹ
    sTime.Hours = tm_info->tm_hour;
    sTime.Minutes = tm_info->tm_min;
    sTime.Seconds = tm_info->tm_sec;
    // ���ʹ��12Сʱ�ƣ������� TimeFormat���˴��ٶ�ʹ��24Сʱ��
    sTime.TimeFormat = RTC_HOURFORMAT12_AM;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    // ��� RTC ���ڽṹ
    // RTC ��������Ϊ (ʵ����� - 2000)
    sDate.Year = (tm_info->tm_year + 1900) - 2000;
    sDate.Month = tm_info->tm_mon + 1;      // tm_mon ��Χ0~11�����Լ�1
    sDate.Date = tm_info->tm_mday;
    // �������ڼ���struct tm �� tm_wday ��Χ 0�������죩~6���� RTC ͨ��Ҫ�� 1~7������һ~�����죩
    sDate.WeekDay = (tm_info->tm_wday == 0) ? 7 : tm_info->tm_wday;
    
    // ���� RTC ʱ�������
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        // ������
    }
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        // ������
    }
    
    // д�뱸�ݼĴ�������� RTC �ѱ���ȷ����
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);
}
/*
1:rtc��Ч���Ĵ���Ϊ�գ���sysData.timestampʼ����Ч���������³ɹ�
2:rtc��Ч���Ĵ���Ϊ�գ���sysData.timestampʼ����Ч����������ʧ��(x)
3:rtc��Ч���Ĵ���Ϊ0x5051��������׼��sysData.timestampʼ����Ч���������³ɹ�
4:rtc��Ч���Ĵ���Ϊ0x5051��������׼��sysData.timestampʼ����Ч����������ʧ��(x)(�޷��ж�)
5:rtc��Ч���Ĵ���Ϊ0x5051����ʱ��׼��sysData.timestampʼ����Ч���������³ɹ�
6:rtc��Ч���Ĵ���Ϊ0x5051����ʱ��׼��sysData.timestampʼ����Ч����������ʧ��
��1��3��5�����Ӧ�ô�sysData.timestamp���µ�rtc����6�����Ӧ�ô�rtc���µ�sysData.timestamp����2��4�����û�пɿ���ʱ�ӣ�����ִ�ж�Ӧ�Ĳ���
*/
void get_net_time(lv_timer_t *timer)
{
    RTC_TimeTypeDef GetTime;
    RTC_DateTypeDef GetData;
    
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

    int *flag = (int *)timer->user_data;
    if (flag)
    {
        if (*flag == 0)//rtc��Ч��rtc����е磩
        {
            if(sysData.network_is_gettime==true)//�ɹ��������ȡunixʱ��
            {
                //��rtcת����unixʱ���
                struct tm timeinfo;
                timeinfo.tm_year = (2000 + GetData.Year) - 1900;  // ת�������ƫ��
                timeinfo.tm_mon  = GetData.Month - 1;             // �·ݴ� 0 ��ʼ
                timeinfo.tm_mday = GetData.Date;
                timeinfo.tm_hour = GetTime.Hours;
                timeinfo.tm_min  = GetTime.Minutes;
                timeinfo.tm_sec  = GetTime.Seconds;
                timeinfo.tm_isdst = 0;
            
                // ת��Ϊ Unix ʱ���
                uint64_t timebuf= mktime(&timeinfo) - 8 * 3600;
                if(abs(sysData.timestamp-timebuf)>60)//��������ȡ��ʱ����rtc��ֵ����60
                {
                    //3:rtc��Ч���Ĵ���Ϊ0x5051��������׼��sysData.timestampʼ����Ч���������³ɹ�
                    time_sys_to_rtc();//ʱ���sys���µ�rtc��ʱ�䲻׼
                }else{
                    //5:rtc��Ч���Ĵ���Ϊ0x5051����ʱ��׼��sysData.timestampʼ����Ч���������³ɹ�
                    time_rtc_to_sys();//ʱ���rtc���µ�sys��ʱ��׼
                }
                 
            }else//ʧ�ܴ������ȡunixʱ��
            {
                //4:rtc��Ч���Ĵ���Ϊ0x5051��������׼��sysData.timestampʼ����Ч����������ʧ��(x)(�޷��ж�)
                //6:rtc��Ч���Ĵ���Ϊ0x5051����ʱ��׼��sysData.timestampʼ����Ч����������ʧ��
                time_rtc_to_sys();//ʱ���rtc���µ�sys
            }
            
        }
        else if (*flag == 1)//rtc��Ч��rtc���û�磩
        {
            //�ɹ��������ȡunixʱ��
            if(sysData.network_is_gettime==true)
            {
                //1:rtc��Ч���Ĵ���Ϊ�գ���sysData.timestampʼ����Ч���������³ɹ�
                time_sys_to_rtc();//ʱ���sys���µ�rtc
            }else{
                //2:rtc��Ч���Ĵ���Ϊ�գ���sysData.timestampʼ����Ч����������ʧ��(x)
            }
            
        }
        lv_mem_free(flag); // �ͷŶ�̬������ڴ�
    }
    lv_timer_del(timer);//һ���Զ�ʱ��
}
void update_time(lv_timer_t *timer)
{
    static int rtc_checked = 0;  // ȷ��ֻ��ȡһ��
    char time_str[128] = {0};    // ���ڴ洢����ʱ����Ϣ

    RTC_TimeTypeDef GetTime;
    RTC_DateTypeDef GetData;
    
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

    if (!rtc_checked)  // ִֻ��һ��
    {
        int *flag = lv_mem_alloc(sizeof(int)); // �����ڴ�洢��־λ
        if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == 0x5051)
        {
            *flag = 0;
            OneNet_Publish("RX", "get/ts");
            lv_timer_t *timer = lv_timer_create(get_net_time, 200, flag); // һ���Զ�ʱ��
            // �����־����ֹ�ظ�д��
            rtc_checked = 1;
        }else 
        {
            *flag = 1;
            OneNet_Publish("RX", "get/ts");
            lv_timer_t *timer = lv_timer_create(get_net_time, 200, flag); // һ���Զ�ʱ��
        }
    }

    char unix_time_str[64] = {0};  // Unix ʱ��ת����ʱ���ַ���
    if (sysData.timestamp > 0)
    {
        time_t t = (time_t)sysData.timestamp + 3600 * 8;  // ת��Ϊ����ʱ��
        struct tm *tm_info = localtime(&t);
        if (tm_info != NULL)
        {
            if (strftime(unix_time_str, sizeof(unix_time_str), "%Y-%m-%d %H:%M:%S", tm_info) == 0)
            {
                strcpy(unix_time_str, "FormatError");
            }
        }
        else
        {
            strcpy(unix_time_str, "InvalidTime");
        }
    }
    else
    {
        strcpy(unix_time_str, "No ts field");
    }

    // ��ʽ�� RTC ��ȡ��ʱ��
    char rtc_time_str[64] = {0};
    snprintf(rtc_time_str, sizeof(rtc_time_str), "%04d-%02d-%02d %02d:%02d:%02d",
             2000 + GetData.Year, GetData.Month, GetData.Date,
             GetTime.Hours, GetTime.Minutes, GetTime.Seconds);

    // �������ʱ����Ϣ
    snprintf(time_str, sizeof(time_str), "U: %s\nR: %s", unix_time_str, rtc_time_str);

    // ���� UI
    lv_label_set_text((lv_obj_t *)timer->user_data, time_str);
}
/*��������������������������������������������������������������������ʱ��ͬ���������������������������������������������������������������������������������������������������������������*/

void trigger_sms(lv_timer_t *timer) {
    static uint8_t i=0;
    i++;
    gsm_send1();
    printf("trigger sms %d\r\n",i);
    if(i>8)
        lv_timer_del(timer); // ������ɣ�ɾ����ʱ��
}
void trigger_beep(lv_timer_t *timer) {
    static uint8_t i=0;
    i++;
    if(i%2)
        Beep_Enable();
    else
        Beep_Disable();
}
void update_check_danger(lv_timer_t *timer)
{
    // sysData.personNumber |=SEAT_PASSENGER;
    static uint8_t f_safe=1;//��ȫ��־λ
    if(f_safe==1)
        if(!(sysData.personNumber&SEAT_DRIVER)){//��ʻλû��
            if((sysData.temperature>=30.0f)||(sysData.CO2>=800)||(sysData.TVOC>=200)){//����������
                //ͳ������
                uint8_t count = 0;
                for (uint8_t i = 0; i < 8; i++) {
                    if (sysData.personNumber & (1 << i)) {
                        count++;
                    }
                }
                if(count>0){//��ʻԱ���ڣ���������λ����
                    //������(����5��delay��������Ҫ2500ms���)
                    lv_timer_create(trigger_sms, 500, NULL);
                    //�򿪷���
                    HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,GPIO_PIN_SET);
                    //�򿪷�����(��Ҫһ�Ὺһ���)
                    lv_timer_t *beep_timer = lv_timer_create(trigger_beep, 200, NULL);
                    //�򿪳���
                    //����Σ�յ���
                    f_safe=0;
                    printf("Warning! Dangerous vehicle detected!\nOccupants trapped inside!\r\n");
                    show_popup_with_pad("Warning! Dangerous vehicle detected!\nOccupants trapped inside!","Car is safe!",&f_safe,beep_timer);
                }else{//��ʻԱ���ڣ���������λû��
                    f_safe=1;
                }
            }else{//����������
                f_safe=1;
            }
        }else{//��ʻλ����
            f_safe=1;
        }
    else
        printf("car no safe\r\n");
}
// ��λ��ť����¼��ص�����
static void seat_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    // �� user_data �л�ȡ��ǰ��λ��Ӧ��λ����
    uint8_t seat = (uint8_t)(uintptr_t)lv_obj_get_user_data(btn);

    // �������λ�Ѿ����ˣ��������ǣ���������Ϊ����
    if (sysData.personNumber & seat) {
        sysData.personNumber &= ~seat;  // �����λ
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x00ff00), 0); // ��ɫ��ʾ����
    } else {
        sysData.personNumber |= seat;   // ���ø�λ��ʾ����
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xff0000), 0); // ��ɫ��ʾ��ռ
    }
}

// �����¼��ص�����
void keyboard_event_cb(lv_event_t *e)
{
    lv_obj_t *keyboard = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CANCEL) {
        // ���ؼ���
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

// �ı����¼��ص�����
void textarea_event_cb(lv_event_t *e)
{
    lv_obj_t *textarea = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_FOCUSED) {
        // ��ȡ���̶���
        lv_obj_t *keyboard = lv_event_get_user_data(e);

        // �����������ı���
        lv_keyboard_set_textarea(keyboard, textarea);

        // ��ʾ����
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}


void reset_system(lv_timer_t *timer)
{
    __disable_irq();        // �����жϣ�ȷ����ȫ
    NVIC_SystemReset();     // ����ϵͳ��λ
}
// ��ʱ���ص�������ɾ�����󵯴�
void delete_error_popup_cb(lv_timer_t *timer)
{
    lv_obj_t *obj_error = (lv_obj_t *)timer->user_data; // ��ȡ��������
    lv_obj_del(obj_error); // ɾ������
    lv_timer_del(timer);
}

// ��ʾ���󵯴������ö�ʱ��ɾ��
void show_error_popup(const char *message)
{
    // ��������
    lv_obj_t *obj_error = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_error, 300, 200);
    lv_obj_set_style_bg_color(obj_error, lv_color_hex(0xFF0000), LV_PART_MAIN); // ��ɫ����
    lv_obj_align(obj_error, LV_ALIGN_CENTER, 0, 0);

    // ��Ӵ�����Ϣ�ı�
    lv_obj_t *label = lv_label_create(obj_error);
    lv_label_set_text(label, message);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), NULL); // ��ɫ����
    lv_obj_center(label);

    // ��ʱ��ɾ������
    lv_timer_t *timer = lv_timer_create(delete_error_popup_cb, 3000, obj_error);
    (void)timer; // ����δʹ�þ���
}


void rebootsystem(void)
{

    // ������ǩ��ʾ "Rebooting..."
    lv_obj_t *obj_reboot = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_reboot, 300, 200);
    lv_obj_set_style_bg_color(obj_reboot, lv_color_hex(0x0000FF), LV_PART_MAIN); // ��ɫ����
    lv_obj_align(obj_reboot, LV_ALIGN_CENTER, 0, 0);

    // ���������Ϣ�ı�
    lv_obj_t *label = lv_label_create(obj_reboot);
    lv_label_set_text(label, "Rebooting...");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), NULL); // ��ɫ����
    lv_obj_center(label);

    // ��ʱ��������������
    lv_timer_create(reset_system, 3000, NULL);
}
// ���Ӱ�ť�¼��ص�����
void connect_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // ��ȡ�����¼��Ķ���
    lv_obj_t *ssid_input = lv_event_get_user_data(e); // ��ȡ������ WiFi �����ı���
    lv_obj_t *password_input = (lv_obj_t *)lv_obj_get_user_data(ssid_input); // ��ȡ������ WiFi �����ı���
    lv_obj_t *phone_input = (lv_obj_t *)lv_obj_get_user_data(password_input); // ��ȡ�������ֻ����ı���

    // ��ȡ WiFi ���ơ�������ֻ���
    const char *wifi_ssid = lv_textarea_get_text(ssid_input);
    const char *wifi_password = lv_textarea_get_text(password_input);
    const char *phone_number = lv_textarea_get_text(phone_input);

    // ��ӡ WiFi ��Ϣ
    printf("Connecting to WiFi...\r\n");
    printf("SSID: %s\r\n", wifi_ssid);
    printf("Password: %s\r\n", wifi_password);
    printf("PhoneNumber: %s\r\n", phone_number);

    // ����ֻ��ų���
    if (strlen(phone_number) != 11) {
        show_error_popup("Invalid Phone Number!"); // ��ʾ���󵯴�
        return;
    }
    // ���wifissid
    if (strlen(wifi_ssid) ==0||strlen(wifi_ssid)>32) {
        show_error_popup("Invalid Wifi SSID lenth!"); // ��ʾ���󵯴�
        return;
    }
        if (strlen(wifi_password) <6||strlen(wifi_password)>32) {
        show_error_popup("Invalid Wifi password lenth!"); // ��ʾ���󵯴�
        return;
    }
    printf("Rebooting 3s later, save config.\r\n");

    DP_UpdateWiFiConfig(wifi_ssid, wifi_password);
    DP_UpdatePhoneNumber(phone_number);
    DP_ConfigSave();

    rebootsystem();
}
void cancle_btn_event_cb(lv_event_t *e)
{
    rebootsystem();
}

void lv_gui(void)
{

    // ����һ����Ļ
    lv_obj_t *scr = lv_scr_act();
    /********��ʾwifi������Ϣ********/
    // ����һ����ɫ��������
    lv_obj_t *obj_config = lv_obj_create(scr);
    lv_obj_set_size(obj_config, 255, 450);
    lv_obj_set_style_bg_color(obj_config, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(obj_config, LV_ALIGN_TOP_LEFT, 0, 0);

    // ���� WiFi ״̬
    lv_obj_t *wifiStat_label = lv_label_create(obj_config);
    if(sysData.network_status)
        lv_label_set_text(wifiStat_label, "connect");
    else{
            lv_label_set_text(wifiStat_label, "disconnet");
            
    }
    lv_obj_align(wifiStat_label, LV_ALIGN_TOP_LEFT, 10, 0);

    // ����ʱ����ʾ
    lv_obj_t *time_label = lv_label_create(obj_config);
    lv_label_set_text(time_label, "1970-01-01 08:00:00");
    lv_obj_align_to(time_label,wifiStat_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // ���� WiFi ���������
    lv_obj_t *ssid_label = lv_label_create(obj_config);
    lv_label_set_text(ssid_label, "WiFi Name:");
    lv_obj_align(ssid_label, LV_ALIGN_TOP_LEFT, 10, 20);

    lv_obj_t *ssid_input = lv_textarea_create(obj_config);
    lv_textarea_set_text(ssid_input, sysConfig.wifi_ssid); // �Զ���� WiFi ����
    lv_textarea_set_one_line(ssid_input, true);
    lv_obj_set_width(ssid_input, 200);
    lv_obj_align_to(ssid_input, ssid_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // ���� WiFi ���������
    lv_obj_t *password_label = lv_label_create(obj_config);
    lv_label_set_text(password_label, "WiFi Password:");
    lv_obj_align_to(password_label, ssid_input, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    lv_obj_t *password_input = lv_textarea_create(obj_config);
    lv_textarea_set_text(password_input, sysConfig.wifi_password); // �Զ���� WiFi ����
    lv_textarea_set_one_line(password_input, true);
    lv_textarea_set_password_mode(password_input, true);
    lv_obj_set_width(password_input, 200);
    lv_obj_align_to(password_input, password_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // ���� �ֻ��������
    lv_obj_t *phone_label = lv_label_create(obj_config);
    lv_label_set_text(phone_label, "Phone Number:");
    lv_obj_align_to(phone_label, password_input, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    lv_obj_t *phone_input = lv_textarea_create(obj_config);
    lv_textarea_set_text(phone_input, sysConfig.phone_number); // �Զ���� WiFi ����
    lv_textarea_set_one_line(phone_input, true);
    lv_obj_set_width(phone_input, 200);
    lv_obj_align_to(phone_input, phone_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // �������ı�������Ϊ WiFi �����ı�����û�����
    lv_obj_set_user_data(ssid_input, password_input);

    // ���ֻ����ı�������Ϊ WiFi �����ı�����û�����
    lv_obj_set_user_data(password_input, phone_input);

    // �������̶���
    lv_obj_t *keyboard = lv_keyboard_create(scr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // Ĭ�����ؼ���
    lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_CANCEL, NULL);
    lv_obj_set_parent(keyboard, lv_layer_top());

    // ����ı����¼��ص�
    lv_obj_add_event_cb(ssid_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);
    lv_obj_add_event_cb(password_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);
    lv_obj_add_event_cb(phone_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);

    // �������水ť
    lv_obj_t *save_btn = lv_btn_create(obj_config);
    lv_obj_align_to(save_btn, phone_input, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_obj_set_size(save_btn, 80, 40);
    lv_obj_t *btn_label = lv_label_create(save_btn);
    lv_label_set_text(btn_label, "Save");
    lv_obj_center(btn_label);

    // ����ȡ����ť
    lv_obj_t *cancle_btn = lv_btn_create(obj_config);
    lv_obj_align_to(cancle_btn, save_btn, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_size(cancle_btn, 80, 40);
    lv_obj_t *btn_label2 = lv_label_create(cancle_btn);
    lv_label_set_text(btn_label2, "Cancle");
    lv_obj_center(btn_label2);

    // Ϊ���Ӱ�ť����¼��ص������������� ssid_input Ϊ�û�����
    lv_obj_add_event_cb(save_btn, connect_btn_event_cb, LV_EVENT_CLICKED, ssid_input);
    lv_obj_add_event_cb(cancle_btn, cancle_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********��ʾ��ʪ��********/
    // ����һ����ɫ��������
    lv_obj_t *obj = lv_obj_create(scr);
    lv_obj_set_size(obj, 300, 100);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);

    // ����һ����ǩ������ʾ�¶�
    lv_obj_t *temp_label = lv_label_create(obj);
    lv_label_set_text(temp_label, "Temp: 0.00 ��C\nHumi: 0.00 %%"); // ��ʼ�ı�
    lv_obj_align(temp_label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *sht_heat_btn = lv_btn_create(obj);
    lv_obj_set_size(sht_heat_btn, 80, 40);
    lv_obj_align(sht_heat_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *label = lv_label_create(sht_heat_btn);
    lv_label_set_text(label, "heat");
    lv_obj_add_event_cb(sht_heat_btn, heat_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********��ʾ����������Ϣ********/
    // ����һ����ɫ��������
    lv_obj_t *obj2 = lv_obj_create(scr);
    lv_obj_set_size(obj2, 300, 100);
    lv_obj_set_style_bg_color(obj2, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(obj2, LV_ALIGN_CENTER, 0, -95);

    // ����һ����ǩ������ʾ�¶�
    lv_obj_t *sgp_label = lv_label_create(obj2);
    lv_label_set_text(sgp_label, "CO2 : 400 ppm\nTVOC: 0 ppb"); // ��ʼ�ı�
    lv_obj_align(sgp_label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *sgp_id_btn = lv_btn_create(obj2);
    lv_obj_set_size(sgp_id_btn, 80, 40);
    lv_obj_align(sgp_id_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *sgp_id_label = lv_label_create(sgp_id_btn);
    lv_label_set_text(sgp_id_label, "id");
    lv_obj_add_event_cb(sgp_id_btn, sgp_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********��ʾGPS��Ϣ********/
// ����һ����ɫ��������������ʾGPS��Ϣ
    lv_obj_t *gps_obj = lv_obj_create(scr);
    lv_obj_set_size(gps_obj, 300, 110);
    lv_obj_set_style_bg_color(gps_obj, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(gps_obj, LV_ALIGN_CENTER, 0, 0); // ����λ��

    // ������ǩ������ʾGPS��Ϣ
    lv_obj_t *gps_label = lv_label_create(gps_obj);
    lv_label_set_text(gps_label, "GPS: Disconnected\nCNTime:------\nLatitude: ---.-----\nLongitude: ---.-----"); // ��ʼ�ı�
    lv_obj_align(gps_label, LV_ALIGN_LEFT_MID, 0, 0);

    // ��ѡ������һ����ť��������ʾ�����GPS��������Ϣ
    lv_obj_t *gps_btn = lv_btn_create(gps_obj);
    lv_obj_set_size(gps_btn, 80, 40);
    lv_obj_align(gps_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *gps_btn_label = lv_label_create(gps_btn);
    lv_label_set_text(gps_btn_label, "Info");
    lv_obj_add_event_cb(gps_btn, gps_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********��λģ����� ����������λ��ť********/
    // ����һ����������С 300x200�������� gps_obj ���·�
    lv_obj_t *seat_container = lv_obj_create(scr);
    lv_obj_set_size(seat_container, 300, 200);
    lv_obj_align_to(seat_container, gps_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(seat_container, lv_color_hex(0xd3d3d3), LV_PART_MAIN);




    // ��ʻ����ť�����Ͻǣ�
    lv_obj_t *btn_driver = lv_btn_create(seat_container);
    lv_obj_set_size(btn_driver, 80, 40);
    lv_obj_set_style_bg_color(btn_driver, lv_color_hex(0x00ff00), 0); // ��ʼ��ɫ������
    lv_obj_align(btn_driver, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_user_data(btn_driver, (void*)(uintptr_t)SEAT_DRIVER);
    lv_obj_add_event_cb(btn_driver, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_driver = lv_label_create(btn_driver);
    lv_label_set_text(label_driver, "Driver");
    lv_obj_center(label_driver);

    // ����ʻ����ť�����Ͻǣ�
    lv_obj_t *btn_passenger = lv_btn_create(seat_container);
    lv_obj_set_size(btn_passenger, 80, 40);
    lv_obj_set_style_bg_color(btn_passenger, lv_color_hex(0x00ff00), 0);
    lv_obj_align(btn_passenger, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_set_user_data(btn_passenger, (void*)(uintptr_t)SEAT_PASSENGER);
    lv_obj_add_event_cb(btn_passenger, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_passenger = lv_label_create(btn_passenger);
    lv_label_set_text(label_passenger, "Passenger");
    lv_obj_center(label_passenger);

    // ������ఴť�����½ǣ�
    lv_obj_t *btn_rear_left = lv_btn_create(seat_container);
    lv_obj_set_size(btn_rear_left, 80, 40);
    lv_obj_set_style_bg_color(btn_rear_left, lv_color_hex(0x00ff00), 0);
    lv_obj_align(btn_rear_left, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_set_user_data(btn_rear_left, (void*)(uintptr_t)SEAT_REAR_LEFT);
    lv_obj_add_event_cb(btn_rear_left, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_rear_left = lv_label_create(btn_rear_left);
    lv_label_set_text(label_rear_left, "Rear Left");
    lv_obj_center(label_rear_left);

    // �����Ҳఴť�����½ǣ�
    lv_obj_t *btn_rear_right = lv_btn_create(seat_container);
    lv_obj_set_size(btn_rear_right, 80, 40);
    lv_obj_set_style_bg_color(btn_rear_right, lv_color_hex(0x00ff00), 0);
    lv_obj_align(btn_rear_right, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_set_user_data(btn_rear_right, (void*)(uintptr_t)SEAT_REAR_RIGHT);
    lv_obj_add_event_cb(btn_rear_right, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_rear_right = lv_label_create(btn_rear_right);
    lv_label_set_text(label_rear_right, "Rear Right");
    lv_obj_center(label_rear_right);


    // ��ʱ����ʱ��
    lv_timer_create(update_time, 1000, time_label);
    // MQTT��Ϣ��ʱ����
    if(sysData.network_status==true){
        lv_timer_create(update_mqtt, 5000, NULL); // ÿ200ms����һ��mqtt��Ϣ

    }else show_error_popup("No Network!");

    // ��ʱ���������ڸ���GPS��Ϣ
    lv_timer_create(update_gps_info, 1000, gps_label); // ÿ�����һ��


    // �����Ը����¶����ݵ�����
    lv_timer_create(update_sht40_task, 1000, temp_label); // ÿ�����һ��
    lv_timer_create(update_sgp30_task, 200, sgp_label); // ÿ200ms����һ��

    
    lv_timer_create(update_check_danger, 1000, NULL); // ��Ŀ���߼���飨�����s��

    // ����ͼ�����
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 230, 200);
    
    lv_obj_align(chart, LV_ALIGN_TOP_RIGHT, 0, 0);
    // ���ñ�����ɫ
    lv_obj_set_style_bg_color(chart, lv_color_hex(0xd3d3d3), LV_PART_MAIN);

    // ����ͼ������Ϊ����������ʾԲ��
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 100);  // ���Ӹ���ĵ㣬ʹ���߸�ƽ��

    // ����Y�᷶Χ
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 400, 1000);

    // �������ϵ��
    series = lv_chart_add_series(chart, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);

    // ����һ����ʱ�����񣬶��ڸ�������
    lv_timer_create(update_chart, 200, NULL);  // ÿ50�������һ��ͼ��

    
}




// ��ʱ�����񣺸���ͼ������
void update_chart(lv_timer_t * timer)
{
    int i=0;
    
    // ����ֵ��ӵ�ͼ������ϵ����
    lv_chart_set_next_value(chart, series, sysData.CO2);

    // ����ͼ������е�ֵΪLV_CHART_POINT_NONE�����������
    if (i == 99) {
        lv_chart_set_all_value(chart, series, LV_CHART_POINT_NONE);  // ��������ݣ����»���
        i = 0;  // ����i���Ա����¿�ʼ
    } else {
        i++;
    }
}

void lv_gui_start(void)
{
	lv_obj_t *scr = lv_scr_act();
	lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "System Initting...");
		lv_task_handler();

}
// �ύ��ť�¼��ص�����
void submit_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // ��ȡ�����¼��Ķ���
    lv_obj_t *code_input = lv_event_get_user_data(e); // ��ȡ������ code �ı���

    // ��ȡ code
    const char *code = lv_textarea_get_text(code_input);

    // �رռ���
    lv_obj_t *keyboard = lv_obj_get_user_data(code_input); // ��ȡ�洢�ļ��̶���
    if (keyboard) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // ���ؼ���
    }

    // ȡ���ı��򽹵㣬��ֹ�����ٴε���
    lv_keyboard_set_textarea(keyboard, NULL);

    uint32_t codehex = 0;
    if (code != NULL && *code != '\0') {
        char *endptr;
        codehex = strtoul(code, &endptr, 16);

        // ��� endptr �����ַ���ĩβ��˵���зǷ��ַ�
        if (*endptr != '\0') {
            show_error_popup("Invalid input");
            codehex = 0; // ��Ϊ 0 ��Ϊ�����ʶ���ɸ��������޸ģ�
            return;
        }
    } else {
        show_error_popup("Empty input!");
        return;
    }

    uint32_t uid0 = READ_REG(*((uint32_t*)0x1FF1E800)); // UID[31:0]
    uint32_t uid1 = READ_REG(*((uint32_t*)0x1FF1E804)); // UID[63:32]
    uint32_t uid2 = READ_REG(*((uint32_t*)0x1FF1E808)); // UID[95:64]

    // ����򵥵� XOR У���
    uint32_t checksum = uid0 ^ uid1 ^ uid2;

    if(codehex!=checksum)
    {
        show_error_popup("code is not correct!");
        return;

    }
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR16, codehex);
    rebootsystem();

}



void lv_gui_activic_system(uint32_t uid0,uint32_t uid1,uint32_t uid2)
{
	lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xd3d3d3), 0);
    
	lv_obj_t *label = lv_label_create(scr);
    lv_obj_align(label,LV_ALIGN_TOP_MID,0,20);
    char textbuf[100];
    sprintf(textbuf,"System unsafe detected!\n Please enter activation code\nCHIPID : 0x%x 0x%x 0x%x", uid0, uid1, uid2);
    lv_label_set_text(label, textbuf);

    // ���������
    lv_obj_t *code_label = lv_label_create(scr);
    lv_label_set_text(code_label, "activation code: 0x");
    lv_obj_align_to(code_label, label,LV_ALIGN_OUT_BOTTOM_LEFT, 10, 20);

    lv_obj_t *code_input = lv_textarea_create(scr);
    lv_textarea_set_one_line(code_input, true);
    lv_obj_set_width(code_input, 200);
    lv_obj_align_to(code_input, code_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // �������̶���
    lv_obj_t *keyboard = lv_keyboard_create(scr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // Ĭ�����ؼ���
    lv_obj_set_parent(keyboard, lv_layer_top());

    // �������̺��ı���
    lv_obj_add_event_cb(code_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);
    lv_obj_set_user_data(code_input, keyboard); // �� `code_input` ��ס `keyboard`


    lv_obj_t *submit_btn = lv_btn_create(scr);
    lv_obj_align_to(submit_btn, code_input, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_size(submit_btn, 80, 40);
    lv_obj_t *btn_label2 = lv_label_create(submit_btn);
    lv_label_set_text(btn_label2, "submit");
    lv_obj_center(btn_label2);


    //�ύ��ť�ص�����
    lv_obj_add_event_cb(submit_btn, submit_btn_event_cb, LV_EVENT_CLICKED, code_input);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

