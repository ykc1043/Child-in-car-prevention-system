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

// 定义一个全局的图表对象和数据系列
static lv_obj_t * chart;
static lv_chart_series_t * series;



/**********************
 *      MACROS
 **********************/
// 前向声明 update_chart 函数
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

    // 滑块到达最右端时关闭弹窗
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
    // 创建弹窗
    lv_obj_t *popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(popup, 310, 200);
    lv_obj_set_style_bg_color(popup, lv_color_hex(0xFF0000), LV_PART_MAIN); // 红色背景
    lv_obj_align(popup, LV_ALIGN_CENTER, 0, 0);

    // 添加错误信息文本
    lv_obj_t *label = lv_label_create(popup);
    lv_label_set_text(label, message);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // 白色字体
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // 创建滑块
    lv_obj_t *slider = lv_slider_create(popup);
    lv_obj_set_size(slider, 260, 40);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x777777), LV_PART_MAIN); // 灰色背景
    lv_obj_set_style_radius(slider, 20, LV_PART_MAIN);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -20);

    // 滑块手柄样式 (白色)
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 20, LV_PART_INDICATOR);
    lv_obj_set_style_pad_ver(slider, 10, LV_PART_INDICATOR);

    // 滑块上的文本
    lv_obj_t *slider_label = lv_label_create(slider);
    lv_label_set_text(slider_label, slidemsg);
    lv_obj_set_style_text_color(slider_label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 0);

    static void *user_data[3]; 
    user_data[0]=popup;
    user_data[1]=val;
    user_data[2]=beep_timer;
    // 绑定滑动事件
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, user_data);
}
/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓传感器的事件处理 约130行↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
void enable_button_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *btn = (lv_obj_t *)timer->user_data; // 获取需要启用的按钮
    lv_obj_clear_state(btn, LV_STATE_DISABLED);   // 清除禁用状态
    lv_timer_del(timer);                          // 删除定时器
}
void heat_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // 获取触发事件的按钮
    printf("Heat button clicked!\r\n");

    SHT_Write_Len(SHT_IIC_ADDR, SHT40_HEATER_200mW_1s, 1, NULL);

    // 禁用按钮
    lv_obj_add_state(btn, LV_STATE_DISABLED);

    // 创建一个定时器，在 1 秒后启用按钮
    lv_timer_t *timer = lv_timer_create(enable_button_timer_cb, 1000, btn);
}

void sgp_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // 获取触发事件的按钮
    printf("id button clicked!\r\n");
	uint8_t id[6];
	sgp30_get_serial_id(id);
	printf("%02x %02x %02x %02x %02x %02x\r\n",id[0],id[1],id[2],id[3],id[4],id[5]);
    

}


void gps_btn_event_cb(lv_event_t *e)
{
    printf("gps button clicked!\r\n");
    lv_obj_t *btn = lv_event_get_target(e); // 获取触发事件的按钮

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
    
    
    // 检查状态码
    if (UART4_Rx_flg == 1) {
        UART4_Rx_flg = 0;

        // 设置GPS状态
        if (gps.UsefulFlag == 'V') {
            strcpy(status, "Disconnected");

            // 如果没有连接GPS，从系统配置中获取经纬度
            sprintf(latitude, "%.5f", sysConfig.latitude);
            sprintf(longitude, "%.5f", sysConfig.longitude);
        } else if (gps.UsefulFlag == 'A') {
            strcpy(status, "Connected");
			DP_UpdateLocation(gps.latitude_d,gps.longitude_d);
            // 设置纬度
            sprintf(latitude, "%.5f", gps.latitude_d);
            if (gps.N_S == 'N') {
                strcat(latitude, " N");
            } else if (gps.N_S == 'S') {
                strcat(latitude, " S");
            }

            // 设置经度
            sprintf(longitude, "%.5f", gps.longitude_d);
            if (gps.E_W == 'E') {
                strcat(longitude, " E");
            } else if (gps.E_W == 'W') {
                strcat(longitude, " W");
            }
        }
        sprintf(t3,"%02d:%02d:%02d",gps.UTCTime_int/10000,gps.UTCTime_int/100%100,gps.UTCTime_int%100);
        // 更新标签
        lv_obj_t *gps_label = (lv_obj_t *)timer->user_data;
        char gps_info[100];
        sprintf(gps_info, "GPS: %s\nCNTime: %s\nLatitude: %s\nLongitude: %s", status, t3,latitude, longitude);
        lv_label_set_text(gps_label, gps_info);
    }
#endif
}
// 定时器回调函数
void update_sht40_task(lv_timer_t *timer)
{

    // 获取温湿度数据
    SHT40_Read_RHData(&sysData.temperature, &sysData.humidity);
    if (sysData.temperature > 100 || sysData.humidity > 100)
        return;
    // 更新标签显示内容
    static char buf[32];
    snprintf(buf, sizeof(buf), "Temp: %.2f °C\nHumi: %.2f %%", sysData.temperature, sysData.humidity);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);
}
void update_sgp30_task(lv_timer_t *timer)
{

    sgp30_read(&sysData.CO2,&sysData.TVOC);
    static char buf[32];
    snprintf(buf, sizeof(buf), "CO2 : %d ppm\nTVOC: %d ppb", sysData.CO2,sysData.TVOC);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);
}
/*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑传感器的事件处理↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/

/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓与web后端通信处理 约60行↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
void update_mqtt(lv_timer_t *timer)
{
    /*使用系统unix时间*/
        // 将 sysData.timestamp（uint64_t 类型）转换为格式化时间字符串
        // char time_str[32] = {0};
        // if (sysData.timestamp > 0)
        // {
        //     // 将 uint64_t 转换为 time_t（Unix 时间戳）
        //     time_t t = (time_t)sysData.timestamp+3600*8;
        //     // 使用 localtime() 获取本地时间（如需 UTC 时间，可用 gmtime(&t)）
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
    /*使用rtc时间*/
    RTC_TimeTypeDef GetTime;
    RTC_DateTypeDef GetData;
    
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

    char time_str[64];
    snprintf(time_str, sizeof(time_str), "%04d-%02d-%02dT%02d:%02d:%02d",
    2000 + GetData.Year, GetData.Month, GetData.Date,
    GetTime.Hours, GetTime.Minutes, GetTime.Seconds);


    // 调用 send_json 时，将转换后的时间字符串传入
    send_json(sysData.temperature,
                sysData.humidity,
                sysData.CO2,
                sysData.TVOC,
                sysConfig.longitude,
                sysConfig.latitude,
                time_str);


    ESP8266_Clear();

}
/*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑与web后端通信处理↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/

/*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓时间同步处理 约200行↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
void time_rtc_to_sys(void)
{
    RTC_TimeTypeDef rtcTime;
    RTC_DateTypeDef rtcDate;
    
    // 获取 RTC 当前时间和日期（以二进制格式）
    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
    
    // 构造 struct tm 结构体
    struct tm timeinfo;
    timeinfo.tm_year = (2000 + rtcDate.Year) - 1900;  // RTC 年份存储为“偏移2000”，tm_year 需要从1900开始计算
    timeinfo.tm_mon  = rtcDate.Month - 1;             // tm_mon 范围 0~11
    timeinfo.tm_mday = rtcDate.Date;
    timeinfo.tm_hour = rtcTime.Hours;
    timeinfo.tm_min  = rtcTime.Minutes;
    timeinfo.tm_sec  = rtcTime.Seconds;
    timeinfo.tm_isdst = -1; // 不使用夏令时判断

    // 将本地时间转换为 Unix 时间戳（UTC），因为 RTC 存的是本地时间，所以减去8小时（8*3600秒）
    time_t unix_time = mktime(&timeinfo) - 8 * 3600;
    sysData.timestamp = (uint64_t)unix_time;
}

void time_sys_to_rtc(void)
{
    // 将系统时间（UTC）转换为本地时间
    time_t utc_time = (time_t)sysData.timestamp;
    time_t local_time = utc_time + 8 * 3600;  // 根据时区调整（这里以东八区为例）
    struct tm *tm_info = localtime(&local_time);
    if (tm_info == NULL)
    {
        // 转换失败，错误处理
        return;
    }
    
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    
    // 填充 RTC 时间结构
    sTime.Hours = tm_info->tm_hour;
    sTime.Minutes = tm_info->tm_min;
    sTime.Seconds = tm_info->tm_sec;
    // 如果使用12小时制，可设置 TimeFormat，此处假定使用24小时制
    sTime.TimeFormat = RTC_HOURFORMAT12_AM;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    // 填充 RTC 日期结构
    // RTC 保存的年份为 (实际年份 - 2000)
    sDate.Year = (tm_info->tm_year + 1900) - 2000;
    sDate.Month = tm_info->tm_mon + 1;      // tm_mon 范围0~11，所以加1
    sDate.Date = tm_info->tm_mday;
    // 计算星期几：struct tm 中 tm_wday 范围 0（星期天）~6，而 RTC 通常要求 1~7（星期一~星期天）
    sDate.WeekDay = (tm_info->tm_wday == 0) ? 7 : tm_info->tm_wday;
    
    // 更新 RTC 时间和日期
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        // 错误处理
    }
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        // 错误处理
    }
    
    // 写入备份寄存器，标记 RTC 已被正确设置
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);
}
/*
1:rtc无效（寄存器为空）。sysData.timestamp始终有效，联网更新成功
2:rtc无效（寄存器为空）。sysData.timestamp始终有效，联网更新失败(x)
3:rtc有效（寄存器为0x5051），但不准。sysData.timestamp始终有效，联网更新成功
4:rtc有效（寄存器为0x5051），但不准。sysData.timestamp始终有效，联网更新失败(x)(无法判断)
5:rtc有效（寄存器为0x5051），时间准。sysData.timestamp始终有效，联网更新成功
6:rtc有效（寄存器为0x5051），时间准。sysData.timestamp始终有效，联网更新失败
在1，3，5情况，应该从sysData.timestamp更新到rtc；在6情况，应该从rtc更新到sysData.timestamp；在2，4情况，没有可靠的时钟，可以执行对应的操作
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
        if (*flag == 0)//rtc有效（rtc电池有电）
        {
            if(sysData.network_is_gettime==true)//成功从网络获取unix时间
            {
                //将rtc转换成unix时间戳
                struct tm timeinfo;
                timeinfo.tm_year = (2000 + GetData.Year) - 1900;  // 转换成年份偏移
                timeinfo.tm_mon  = GetData.Month - 1;             // 月份从 0 开始
                timeinfo.tm_mday = GetData.Date;
                timeinfo.tm_hour = GetTime.Hours;
                timeinfo.tm_min  = GetTime.Minutes;
                timeinfo.tm_sec  = GetTime.Seconds;
                timeinfo.tm_isdst = 0;
            
                // 转换为 Unix 时间戳
                uint64_t timebuf= mktime(&timeinfo) - 8 * 3600;
                if(abs(sysData.timestamp-timebuf)>60)//如果网络获取的时间与rtc差值大于60
                {
                    //3:rtc有效（寄存器为0x5051），但不准。sysData.timestamp始终有效，联网更新成功
                    time_sys_to_rtc();//时间从sys更新到rtc，时间不准
                }else{
                    //5:rtc有效（寄存器为0x5051），时间准。sysData.timestamp始终有效，联网更新成功
                    time_rtc_to_sys();//时间从rtc更新到sys，时间准
                }
                 
            }else//失败从网络获取unix时间
            {
                //4:rtc有效（寄存器为0x5051），但不准。sysData.timestamp始终有效，联网更新失败(x)(无法判断)
                //6:rtc有效（寄存器为0x5051），时间准。sysData.timestamp始终有效，联网更新失败
                time_rtc_to_sys();//时间从rtc更新到sys
            }
            
        }
        else if (*flag == 1)//rtc无效（rtc电池没电）
        {
            //成功从网络获取unix时间
            if(sysData.network_is_gettime==true)
            {
                //1:rtc无效（寄存器为空）。sysData.timestamp始终有效，联网更新成功
                time_sys_to_rtc();//时间从sys更新到rtc
            }else{
                //2:rtc无效（寄存器为空）。sysData.timestamp始终有效，联网更新失败(x)
            }
            
        }
        lv_mem_free(flag); // 释放动态分配的内存
    }
    lv_timer_del(timer);//一次性定时器
}
void update_time(lv_timer_t *timer)
{
    static int rtc_checked = 0;  // 确保只读取一次
    char time_str[128] = {0};    // 用于存储两行时间信息

    RTC_TimeTypeDef GetTime;
    RTC_DateTypeDef GetData;
    
    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

    if (!rtc_checked)  // 只执行一次
    {
        int *flag = lv_mem_alloc(sizeof(int)); // 申请内存存储标志位
        if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == 0x5051)
        {
            *flag = 0;
            OneNet_Publish("RX", "get/ts");
            lv_timer_t *timer = lv_timer_create(get_net_time, 200, flag); // 一次性定时器
            // 清除标志，防止重复写入
            rtc_checked = 1;
        }else 
        {
            *flag = 1;
            OneNet_Publish("RX", "get/ts");
            lv_timer_t *timer = lv_timer_create(get_net_time, 200, flag); // 一次性定时器
        }
    }

    char unix_time_str[64] = {0};  // Unix 时间转换的时间字符串
    if (sysData.timestamp > 0)
    {
        time_t t = (time_t)sysData.timestamp + 3600 * 8;  // 转换为本地时间
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

    // 格式化 RTC 读取的时间
    char rtc_time_str[64] = {0};
    snprintf(rtc_time_str, sizeof(rtc_time_str), "%04d-%02d-%02d %02d:%02d:%02d",
             2000 + GetData.Year, GetData.Month, GetData.Date,
             GetTime.Hours, GetTime.Minutes, GetTime.Seconds);

    // 组合两行时间信息
    snprintf(time_str, sizeof(time_str), "U: %s\nR: %s", unix_time_str, rtc_time_str);

    // 更新 UI
    lv_label_set_text((lv_obj_t *)timer->user_data, time_str);
}
/*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑时间同步处理↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/

void trigger_sms(lv_timer_t *timer) {
    static uint8_t i=0;
    i++;
    gsm_send1();
    printf("trigger sms %d\r\n",i);
    if(i>8)
        lv_timer_del(timer); // 任务完成，删除定时器
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
    static uint8_t f_safe=1;//安全标志位
    if(f_safe==1)
        if(!(sysData.personNumber&SEAT_DRIVER)){//驾驶位没人
            if((sysData.temperature>=30.0f)||(sysData.CO2>=800)||(sysData.TVOC>=200)){//传感器超标
                //统计人数
                uint8_t count = 0;
                for (uint8_t i = 0; i < 8; i++) {
                    if (sysData.personNumber & (1 << i)) {
                        count++;
                    }
                }
                if(count>0){//驾驶员不在，且其他座位有人
                    //发短信(内有5个delay函数，需要2500ms完成)
                    lv_timer_create(trigger_sms, 500, NULL);
                    //打开风扇
                    HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,GPIO_PIN_SET);
                    //打开蜂鸣器(需要一会开一会关)
                    lv_timer_t *beep_timer = lv_timer_create(trigger_beep, 200, NULL);
                    //打开车门
                    //创建危险弹窗
                    f_safe=0;
                    printf("Warning! Dangerous vehicle detected!\nOccupants trapped inside!\r\n");
                    show_popup_with_pad("Warning! Dangerous vehicle detected!\nOccupants trapped inside!","Car is safe!",&f_safe,beep_timer);
                }else{//驾驶员不在，且其他座位没人
                    f_safe=1;
                }
            }else{//传感器正常
                f_safe=1;
            }
        }else{//驾驶位有人
            f_safe=1;
        }
    else
        printf("car no safe\r\n");
}
// 座位按钮点击事件回调函数
static void seat_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    // 从 user_data 中获取当前座位对应的位掩码
    uint8_t seat = (uint8_t)(uintptr_t)lv_obj_get_user_data(btn);

    // 如果该座位已经有人，则清除标记；否则设置为有人
    if (sysData.personNumber & seat) {
        sysData.personNumber &= ~seat;  // 清除该位
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x00ff00), 0); // 绿色表示空座
    } else {
        sysData.personNumber |= seat;   // 设置该位表示有人
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xff0000), 0); // 红色表示已占
    }
}

// 键盘事件回调函数
void keyboard_event_cb(lv_event_t *e)
{
    lv_obj_t *keyboard = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CANCEL) {
        // 隐藏键盘
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

// 文本框事件回调函数
void textarea_event_cb(lv_event_t *e)
{
    lv_obj_t *textarea = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_FOCUSED) {
        // 获取键盘对象
        lv_obj_t *keyboard = lv_event_get_user_data(e);

        // 关联键盘与文本框
        lv_keyboard_set_textarea(keyboard, textarea);

        // 显示键盘
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}


void reset_system(lv_timer_t *timer)
{
    __disable_irq();        // 禁用中断，确保安全
    NVIC_SystemReset();     // 触发系统复位
}
// 定时器回调函数：删除错误弹窗
void delete_error_popup_cb(lv_timer_t *timer)
{
    lv_obj_t *obj_error = (lv_obj_t *)timer->user_data; // 获取弹窗对象
    lv_obj_del(obj_error); // 删除弹窗
    lv_timer_del(timer);
}

// 显示错误弹窗并设置定时器删除
void show_error_popup(const char *message)
{
    // 创建弹窗
    lv_obj_t *obj_error = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_error, 300, 200);
    lv_obj_set_style_bg_color(obj_error, lv_color_hex(0xFF0000), LV_PART_MAIN); // 红色背景
    lv_obj_align(obj_error, LV_ALIGN_CENTER, 0, 0);

    // 添加错误信息文本
    lv_obj_t *label = lv_label_create(obj_error);
    lv_label_set_text(label, message);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), NULL); // 白色字体
    lv_obj_center(label);

    // 定时器删除弹窗
    lv_timer_t *timer = lv_timer_create(delete_error_popup_cb, 3000, obj_error);
    (void)timer; // 避免未使用警告
}


void rebootsystem(void)
{

    // 创建标签显示 "Rebooting..."
    lv_obj_t *obj_reboot = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_reboot, 300, 200);
    lv_obj_set_style_bg_color(obj_reboot, lv_color_hex(0x0000FF), LV_PART_MAIN); // 蓝色背景
    lv_obj_align(obj_reboot, LV_ALIGN_CENTER, 0, 0);

    // 添加重启信息文本
    lv_obj_t *label = lv_label_create(obj_reboot);
    lv_label_set_text(label, "Rebooting...");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), NULL); // 白色字体
    lv_obj_center(label);

    // 定时器调用重启函数
    lv_timer_create(reset_system, 3000, NULL);
}
// 连接按钮事件回调函数
void connect_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // 获取触发事件的对象
    lv_obj_t *ssid_input = lv_event_get_user_data(e); // 获取关联的 WiFi 名称文本框
    lv_obj_t *password_input = (lv_obj_t *)lv_obj_get_user_data(ssid_input); // 获取关联的 WiFi 密码文本框
    lv_obj_t *phone_input = (lv_obj_t *)lv_obj_get_user_data(password_input); // 获取关联的手机号文本框

    // 获取 WiFi 名称、密码和手机号
    const char *wifi_ssid = lv_textarea_get_text(ssid_input);
    const char *wifi_password = lv_textarea_get_text(password_input);
    const char *phone_number = lv_textarea_get_text(phone_input);

    // 打印 WiFi 信息
    printf("Connecting to WiFi...\r\n");
    printf("SSID: %s\r\n", wifi_ssid);
    printf("Password: %s\r\n", wifi_password);
    printf("PhoneNumber: %s\r\n", phone_number);

    // 检测手机号长度
    if (strlen(phone_number) != 11) {
        show_error_popup("Invalid Phone Number!"); // 显示错误弹窗
        return;
    }
    // 检测wifissid
    if (strlen(wifi_ssid) ==0||strlen(wifi_ssid)>32) {
        show_error_popup("Invalid Wifi SSID lenth!"); // 显示错误弹窗
        return;
    }
        if (strlen(wifi_password) <6||strlen(wifi_password)>32) {
        show_error_popup("Invalid Wifi password lenth!"); // 显示错误弹窗
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

    // 创建一个屏幕
    lv_obj_t *scr = lv_scr_act();
    /********显示wifi连接信息********/
    // 创建一个灰色背景对象
    lv_obj_t *obj_config = lv_obj_create(scr);
    lv_obj_set_size(obj_config, 255, 450);
    lv_obj_set_style_bg_color(obj_config, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(obj_config, LV_ALIGN_TOP_LEFT, 0, 0);

    // 创建 WiFi 状态
    lv_obj_t *wifiStat_label = lv_label_create(obj_config);
    if(sysData.network_status)
        lv_label_set_text(wifiStat_label, "connect");
    else{
            lv_label_set_text(wifiStat_label, "disconnet");
            
    }
    lv_obj_align(wifiStat_label, LV_ALIGN_TOP_LEFT, 10, 0);

    // 创建时间显示
    lv_obj_t *time_label = lv_label_create(obj_config);
    lv_label_set_text(time_label, "1970-01-01 08:00:00");
    lv_obj_align_to(time_label,wifiStat_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // 创建 WiFi 名称输入框
    lv_obj_t *ssid_label = lv_label_create(obj_config);
    lv_label_set_text(ssid_label, "WiFi Name:");
    lv_obj_align(ssid_label, LV_ALIGN_TOP_LEFT, 10, 20);

    lv_obj_t *ssid_input = lv_textarea_create(obj_config);
    lv_textarea_set_text(ssid_input, sysConfig.wifi_ssid); // 自动填充 WiFi 名称
    lv_textarea_set_one_line(ssid_input, true);
    lv_obj_set_width(ssid_input, 200);
    lv_obj_align_to(ssid_input, ssid_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // 创建 WiFi 密码输入框
    lv_obj_t *password_label = lv_label_create(obj_config);
    lv_label_set_text(password_label, "WiFi Password:");
    lv_obj_align_to(password_label, ssid_input, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    lv_obj_t *password_input = lv_textarea_create(obj_config);
    lv_textarea_set_text(password_input, sysConfig.wifi_password); // 自动填充 WiFi 密码
    lv_textarea_set_one_line(password_input, true);
    lv_textarea_set_password_mode(password_input, true);
    lv_obj_set_width(password_input, 200);
    lv_obj_align_to(password_input, password_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // 创建 手机号输入框
    lv_obj_t *phone_label = lv_label_create(obj_config);
    lv_label_set_text(phone_label, "Phone Number:");
    lv_obj_align_to(phone_label, password_input, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    lv_obj_t *phone_input = lv_textarea_create(obj_config);
    lv_textarea_set_text(phone_input, sysConfig.phone_number); // 自动填充 WiFi 密码
    lv_textarea_set_one_line(phone_input, true);
    lv_obj_set_width(phone_input, 200);
    lv_obj_align_to(phone_input, phone_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // 将密码文本框设置为 WiFi 名称文本框的用户数据
    lv_obj_set_user_data(ssid_input, password_input);

    // 将手机号文本框设置为 WiFi 密码文本框的用户数据
    lv_obj_set_user_data(password_input, phone_input);

    // 创建键盘对象
    lv_obj_t *keyboard = lv_keyboard_create(scr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // 默认隐藏键盘
    lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_CANCEL, NULL);
    lv_obj_set_parent(keyboard, lv_layer_top());

    // 添加文本框事件回调
    lv_obj_add_event_cb(ssid_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);
    lv_obj_add_event_cb(password_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);
    lv_obj_add_event_cb(phone_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);

    // 创建保存按钮
    lv_obj_t *save_btn = lv_btn_create(obj_config);
    lv_obj_align_to(save_btn, phone_input, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_obj_set_size(save_btn, 80, 40);
    lv_obj_t *btn_label = lv_label_create(save_btn);
    lv_label_set_text(btn_label, "Save");
    lv_obj_center(btn_label);

    // 创建取消按钮
    lv_obj_t *cancle_btn = lv_btn_create(obj_config);
    lv_obj_align_to(cancle_btn, save_btn, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_size(cancle_btn, 80, 40);
    lv_obj_t *btn_label2 = lv_label_create(cancle_btn);
    lv_label_set_text(btn_label2, "Cancle");
    lv_obj_center(btn_label2);

    // 为连接按钮添加事件回调函数，并设置 ssid_input 为用户数据
    lv_obj_add_event_cb(save_btn, connect_btn_event_cb, LV_EVENT_CLICKED, ssid_input);
    lv_obj_add_event_cb(cancle_btn, cancle_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********显示温湿度********/
    // 创建一个灰色背景对象
    lv_obj_t *obj = lv_obj_create(scr);
    lv_obj_set_size(obj, 300, 100);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);

    // 创建一个标签用于显示温度
    lv_obj_t *temp_label = lv_label_create(obj);
    lv_label_set_text(temp_label, "Temp: 0.00 °C\nHumi: 0.00 %%"); // 初始文本
    lv_obj_align(temp_label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *sht_heat_btn = lv_btn_create(obj);
    lv_obj_set_size(sht_heat_btn, 80, 40);
    lv_obj_align(sht_heat_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *label = lv_label_create(sht_heat_btn);
    lv_label_set_text(label, "heat");
    lv_obj_add_event_cb(sht_heat_btn, heat_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********显示空气质量信息********/
    // 创建一个灰色背景对象
    lv_obj_t *obj2 = lv_obj_create(scr);
    lv_obj_set_size(obj2, 300, 100);
    lv_obj_set_style_bg_color(obj2, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(obj2, LV_ALIGN_CENTER, 0, -95);

    // 创建一个标签用于显示温度
    lv_obj_t *sgp_label = lv_label_create(obj2);
    lv_label_set_text(sgp_label, "CO2 : 400 ppm\nTVOC: 0 ppb"); // 初始文本
    lv_obj_align(sgp_label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *sgp_id_btn = lv_btn_create(obj2);
    lv_obj_set_size(sgp_id_btn, 80, 40);
    lv_obj_align(sgp_id_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *sgp_id_label = lv_label_create(sgp_id_btn);
    lv_label_set_text(sgp_id_label, "id");
    lv_obj_add_event_cb(sgp_id_btn, sgp_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********显示GPS信息********/
// 新增一个灰色背景对象用于显示GPS信息
    lv_obj_t *gps_obj = lv_obj_create(scr);
    lv_obj_set_size(gps_obj, 300, 110);
    lv_obj_set_style_bg_color(gps_obj, lv_color_hex(0xd3d3d3), LV_PART_MAIN);
    lv_obj_align(gps_obj, LV_ALIGN_CENTER, 0, 0); // 调整位置

    // 创建标签用于显示GPS信息
    lv_obj_t *gps_label = lv_label_create(gps_obj);
    lv_label_set_text(gps_label, "GPS: Disconnected\nCNTime:------\nLatitude: ---.-----\nLongitude: ---.-----"); // 初始文本
    lv_obj_align(gps_label, LV_ALIGN_LEFT_MID, 0, 0);

    // 可选：创建一个按钮，用于显示额外的GPS操作或信息
    lv_obj_t *gps_btn = lv_btn_create(gps_obj);
    lv_obj_set_size(gps_btn, 80, 40);
    lv_obj_align(gps_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_t *gps_btn_label = lv_label_create(gps_btn);
    lv_label_set_text(gps_btn_label, "Info");
    lv_obj_add_event_cb(gps_btn, gps_btn_event_cb, LV_EVENT_CLICKED, NULL);

    /********座位模拟界面 创建各个座位按钮********/
    // 创建一个容器，大小 300x200，并放在 gps_obj 正下方
    lv_obj_t *seat_container = lv_obj_create(scr);
    lv_obj_set_size(seat_container, 300, 200);
    lv_obj_align_to(seat_container, gps_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(seat_container, lv_color_hex(0xd3d3d3), LV_PART_MAIN);




    // 驾驶座按钮（左上角）
    lv_obj_t *btn_driver = lv_btn_create(seat_container);
    lv_obj_set_size(btn_driver, 80, 40);
    lv_obj_set_style_bg_color(btn_driver, lv_color_hex(0x00ff00), 0); // 初始绿色：无人
    lv_obj_align(btn_driver, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_user_data(btn_driver, (void*)(uintptr_t)SEAT_DRIVER);
    lv_obj_add_event_cb(btn_driver, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_driver = lv_label_create(btn_driver);
    lv_label_set_text(label_driver, "Driver");
    lv_obj_center(label_driver);

    // 副驾驶座按钮（右上角）
    lv_obj_t *btn_passenger = lv_btn_create(seat_container);
    lv_obj_set_size(btn_passenger, 80, 40);
    lv_obj_set_style_bg_color(btn_passenger, lv_color_hex(0x00ff00), 0);
    lv_obj_align(btn_passenger, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_set_user_data(btn_passenger, (void*)(uintptr_t)SEAT_PASSENGER);
    lv_obj_add_event_cb(btn_passenger, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_passenger = lv_label_create(btn_passenger);
    lv_label_set_text(label_passenger, "Passenger");
    lv_obj_center(label_passenger);

    // 后排左侧按钮（左下角）
    lv_obj_t *btn_rear_left = lv_btn_create(seat_container);
    lv_obj_set_size(btn_rear_left, 80, 40);
    lv_obj_set_style_bg_color(btn_rear_left, lv_color_hex(0x00ff00), 0);
    lv_obj_align(btn_rear_left, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_set_user_data(btn_rear_left, (void*)(uintptr_t)SEAT_REAR_LEFT);
    lv_obj_add_event_cb(btn_rear_left, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_rear_left = lv_label_create(btn_rear_left);
    lv_label_set_text(label_rear_left, "Rear Left");
    lv_obj_center(label_rear_left);

    // 后排右侧按钮（右下角）
    lv_obj_t *btn_rear_right = lv_btn_create(seat_container);
    lv_obj_set_size(btn_rear_right, 80, 40);
    lv_obj_set_style_bg_color(btn_rear_right, lv_color_hex(0x00ff00), 0);
    lv_obj_align(btn_rear_right, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_set_user_data(btn_rear_right, (void*)(uintptr_t)SEAT_REAR_RIGHT);
    lv_obj_add_event_cb(btn_rear_right, seat_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_rear_right = lv_label_create(btn_rear_right);
    lv_label_set_text(label_rear_right, "Rear Right");
    lv_obj_center(label_rear_right);


    // 定时更新时间
    lv_timer_create(update_time, 1000, time_label);
    // MQTT信息定时发送
    if(sysData.network_status==true){
        lv_timer_create(update_mqtt, 5000, NULL); // 每200ms更新一次mqtt信息

    }else show_error_popup("No Network!");

    // 定时器任务用于更新GPS信息
    lv_timer_create(update_gps_info, 1000, gps_label); // 每秒更新一次


    // 周期性更新温度数据的任务
    lv_timer_create(update_sht40_task, 1000, temp_label); // 每秒更新一次
    lv_timer_create(update_sgp30_task, 200, sgp_label); // 每200ms更新一次

    
    lv_timer_create(update_check_danger, 1000, NULL); // 项目总逻辑检查（待完成s）

    // 创建图表对象
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 230, 200);
    
    lv_obj_align(chart, LV_ALIGN_TOP_RIGHT, 0, 0);
    // 设置背景颜色
    lv_obj_set_style_bg_color(chart, lv_color_hex(0xd3d3d3), LV_PART_MAIN);

    // 设置图表类型为线条，不显示圆点
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 100);  // 增加更多的点，使曲线更平滑

    // 设置Y轴范围
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 400, 1000);

    // 添加数据系列
    series = lv_chart_add_series(chart, lv_color_hex(0xff0000), LV_CHART_AXIS_PRIMARY_Y);

    // 创建一个定时器任务，定期更新曲线
    lv_timer_create(update_chart, 200, NULL);  // 每50毫秒更新一次图表

    
}




// 定时器任务：更新图表数据
void update_chart(lv_timer_t * timer)
{
    int i=0;
    
    // 将新值添加到图表数据系列中
    lv_chart_set_next_value(chart, series, sysData.CO2);

    // 设置图表的所有点值为LV_CHART_POINT_NONE，以清除数据
    if (i == 99) {
        lv_chart_set_all_value(chart, series, LV_CHART_POINT_NONE);  // 清除旧数据，重新绘制
        i = 0;  // 重置i，以便重新开始
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
// 提交按钮事件回调函数
void submit_btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e); // 获取触发事件的对象
    lv_obj_t *code_input = lv_event_get_user_data(e); // 获取关联的 code 文本框

    // 获取 code
    const char *code = lv_textarea_get_text(code_input);

    // 关闭键盘
    lv_obj_t *keyboard = lv_obj_get_user_data(code_input); // 获取存储的键盘对象
    if (keyboard) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // 隐藏键盘
    }

    // 取消文本框焦点，防止键盘再次弹出
    lv_keyboard_set_textarea(keyboard, NULL);

    uint32_t codehex = 0;
    if (code != NULL && *code != '\0') {
        char *endptr;
        codehex = strtoul(code, &endptr, 16);

        // 如果 endptr 不是字符串末尾，说明有非法字符
        if (*endptr != '\0') {
            show_error_popup("Invalid input");
            codehex = 0; // 设为 0 作为错误标识（可根据需求修改）
            return;
        }
    } else {
        show_error_popup("Empty input!");
        return;
    }

    uint32_t uid0 = READ_REG(*((uint32_t*)0x1FF1E800)); // UID[31:0]
    uint32_t uid1 = READ_REG(*((uint32_t*)0x1FF1E804)); // UID[63:32]
    uint32_t uid2 = READ_REG(*((uint32_t*)0x1FF1E808)); // UID[95:64]

    // 计算简单的 XOR 校验和
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

    // 创建输入框
    lv_obj_t *code_label = lv_label_create(scr);
    lv_label_set_text(code_label, "activation code: 0x");
    lv_obj_align_to(code_label, label,LV_ALIGN_OUT_BOTTOM_LEFT, 10, 20);

    lv_obj_t *code_input = lv_textarea_create(scr);
    lv_textarea_set_one_line(code_input, true);
    lv_obj_set_width(code_input, 200);
    lv_obj_align_to(code_input, code_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // 创建键盘对象
    lv_obj_t *keyboard = lv_keyboard_create(scr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // 默认隐藏键盘
    lv_obj_set_parent(keyboard, lv_layer_top());

    // 关联键盘和文本框
    lv_obj_add_event_cb(code_input, textarea_event_cb, LV_EVENT_FOCUSED, keyboard);
    lv_obj_set_user_data(code_input, keyboard); // 让 `code_input` 记住 `keyboard`


    lv_obj_t *submit_btn = lv_btn_create(scr);
    lv_obj_align_to(submit_btn, code_input, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_size(submit_btn, 80, 40);
    lv_obj_t *btn_label2 = lv_label_create(submit_btn);
    lv_label_set_text(btn_label2, "submit");
    lv_obj_center(btn_label2);


    //提交按钮回调函数
    lv_obj_add_event_cb(submit_btn, submit_btn_event_cb, LV_EVENT_CLICKED, code_input);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

