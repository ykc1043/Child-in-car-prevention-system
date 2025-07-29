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
uint16_t CO2,TVOC;


// ��ť�¼�������
void btn_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);            // ��ȡ�����¼��İ�ť
    uint32_t id = (uint32_t)lv_event_get_user_data(e); // ��ȡ��ť���û�����
    printf("Button %d clicked!\r\n", id);              // ��ӡ��ť ID
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
    // ���� 3 ����ť���ŷ������
    for (uint32_t i = 0; i < 8; i++)
    {
        // ������ť
        lv_obj_t *btn = lv_btn_create(scr);

        // ���ð�ť��С
        lv_obj_set_size(btn, 80, 40);

        // ���ð�ťλ�ã�������Ϊ 50 ����
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 10, 10 + i * 50);

        // ��Ӱ�ť�ĵ���¼��ص�
        lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, (void *)(i + 1));

        // ��ӱ�ǩ����ť��
        lv_obj_t *label = lv_label_create(btn);
        char btn_text[10];
        snprintf(btn_text, sizeof(btn_text), "Btn %d", i + 1);
        lv_label_set_text(label, btn_text);
        lv_obj_center(label);
    }
}
// ��ʱ���ص�����
void update_sht40_task(lv_timer_t *timer)
{
    float temperature = 0.0f;
    float humidity = 0.0f;

    // ��ȡ��ʪ������
    SHT40_Read_RHData(&temperature, &humidity);
    if (temperature > 100 || humidity > 100)
        return;
    // ���±�ǩ��ʾ����
    static char buf[32];
    snprintf(buf, sizeof(buf), "Temp: %.2f ��C\nHumi: %.2f %%", temperature, humidity);
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
void lv_gui(void)
{

    // ����һ����Ļ
    lv_obj_t *scr = lv_scr_act();

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


    // ��ѡ�������Ը����¶����ݵ���������Ҫ��
    lv_timer_t *timer = lv_timer_create(update_sht40_task, 1000, temp_label); // ÿ�����һ��
    lv_timer_t *timer2 = lv_timer_create(update_sgp30_task, 200, sgp_label); // ÿ�����һ��

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
    // creat_uart_btn(scr);
}




// ��ʱ�����񣺸���ͼ������
void update_chart(lv_timer_t * timer)
{
    int i=0;
    
    // ����ֵ��ӵ�ͼ������ϵ����
    lv_chart_set_next_value(chart, series, CO2);

    // ����ͼ������е�ֵΪLV_CHART_POINT_NONE�����������
    if (i == 99) {
        lv_chart_set_all_value(chart, series, LV_CHART_POINT_NONE);  // ��������ݣ����»���
        i = 0;  // ����i���Ա����¿�ʼ
    } else {
        i++;
    }
}



/**********************
 *   STATIC FUNCTIONS
 **********************/
