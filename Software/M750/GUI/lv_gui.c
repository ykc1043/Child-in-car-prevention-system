/**
 * @file lv_GUI.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gui.h"
#include "lvgl.h"
#include "stdio.h"
#include <math.h>

#include "myiic.h"
#include "sgp30.h"
/*********************
 *      DEFINES
 *********************/
#define M_PI 3.14
/**********************
 *      TYPEDEFS
 **********************/

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

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
uint16_t CO2,TVOC;


// 按钮事件处理函数
void btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);            // 获取触发事件的按钮
    uint32_t id = (uint32_t)lv_event_get_user_data(e); // 获取按钮的用户数据
    printf("Button %d clicked!\r\n", id);              // 打印按钮 ID
    if (id == 1)
        HAL_UART_Transmit(&huart1, "1", 1, 0xffff);
    if (id == 3)
        HAL_UART_Transmit(&huart3, "3", 1, 0xffff);
    if (id == 4)
        HAL_UART_Transmit(&huart4, "4", 1, 0xffff);
    if (id == 7)
        HAL_UART_Transmit(&huart7, "7", 1, 0xffff);
}

void creat_uart_btn(lv_obj_t *scr)
{
    // 创建 3 个按钮并排放在左侧
    for (uint32_t i = 0; i < 8; i++)
    {
        // 创建按钮
        lv_obj_t *btn = lv_btn_create(scr);

        // 设置按钮大小
        lv_obj_set_size(btn, 80, 40);

        // 设置按钮位置，纵向间隔为 50 像素
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 10, 10 + i * 50);

        // 添加按钮的点击事件回调
        lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, (void *)(i + 1));

        // 添加标签到按钮上
        lv_obj_t *label = lv_label_create(btn);
        char btn_text[10];
        snprintf(btn_text, sizeof(btn_text), "Btn %d", i + 1);
        lv_label_set_text(label, btn_text);
        lv_obj_center(label);
    }
}
// 定时器回调函数
void update_sht40_task(lv_timer_t *timer)
{
    float temperature = 0.0f;
    float humidity = 0.0f;

    // 获取温湿度数据
    SHT40_Read_RHData(&temperature, &humidity);
    if (temperature > 100 || humidity > 100)
        return;
    // 更新标签显示内容
    static char buf[32];
    snprintf(buf, sizeof(buf), "Temp: %.2f °C\nHumi: %.2f %%", temperature, humidity);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);
}
void update_sgp30_task(lv_timer_t *timer)
{

    sgp30_read(&CO2,&TVOC);
    static char buf[32];
    snprintf(buf, sizeof(buf), "CO2 : %d ppm\nTVOC: %d ppb", CO2,TVOC);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);
}
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
void lv_gui(void)
{

    // 创建一个屏幕
    lv_obj_t *scr = lv_scr_act();

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


    // 可选：周期性更新温度数据的任务（如需要）
    lv_timer_t *timer = lv_timer_create(update_sht40_task, 1000, temp_label); // 每秒更新一次
    lv_timer_t *timer2 = lv_timer_create(update_sgp30_task, 200, sgp_label); // 每秒更新一次

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
    // creat_uart_btn(scr);
}




// 定时器任务：更新图表数据
void update_chart(lv_timer_t * timer)
{
    int i=0;
    
    // 将新值添加到图表数据系列中
    lv_chart_set_next_value(chart, series, CO2);

    // 设置图表的所有点值为LV_CHART_POINT_NONE，以清除数据
    if (i == 99) {
        lv_chart_set_all_value(chart, series, LV_CHART_POINT_NONE);  // 清除旧数据，重新绘制
        i = 0;  // 重置i，以便重新开始
    } else {
        i++;
    }
}



/**********************
 *   STATIC FUNCTIONS
 **********************/
