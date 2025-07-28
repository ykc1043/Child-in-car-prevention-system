/**
 * @file lv_GUI.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_GUI.h"
#include "lvgl.h"
#include "stdio.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#include "lvgl.h"





void lv_example_show_all_colors(void)
{
    lv_obj_t * scr = lv_scr_act();  // ��ȡ��ǰ���Ļ

    // ������Ļ��С��ɫ���������
    const int screen_width = 800;
    const int screen_height = 480;
    const int block_size = 40;  // ÿ��ɫ��ı߳� (40x40)
    const int cols = screen_width / block_size;  // ɫ������
    const int rows = screen_height / block_size;  // ɫ������

    // ��������ɫ��
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // ����ɫ�����
            lv_obj_t * block = lv_obj_create(scr);
            lv_obj_set_size(block, block_size, block_size);  // ����ɫ���С
            lv_obj_align(block, LV_ALIGN_TOP_LEFT, col * block_size, row * block_size);  // ����ɫ��λ��

            // ������������������ɫ (HSV ��ת)
            uint8_t h = (row * cols + col) % 360;  // ɫ�� (0~360)
            uint8_t s = 100;  // ���Ͷ�
            uint8_t v = 100;  // ����
            lv_color_t color = lv_color_hsv_to_rgb(h, s, v);

            // ����ɫ����ɫ
            lv_obj_set_style_bg_color(block, color, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(block, LV_OPA_COVER, LV_PART_MAIN);  // ��͸��
        }
    }
}

LV_IMG_DECLARE(pic)
void lv_gui(void)
{

    // lv_example_show_all_colors();

    lv_obj_t * area=lv_obj_create(lv_scr_act());
    lv_obj_set_size(area,100,200);
    lv_obj_set_style_bg_color(area,lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(area, LV_OPA_COVER, LV_PART_MAIN); 

    lv_obj_t * sw = lv_switch_create(lv_scr_act());
    lv_obj_set_size(sw,400,200);
    lv_obj_set_style_bg_color(sw,lv_color_hex(0xCCCCCC) , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_MAIN); 
    lv_obj_align(sw,LV_ALIGN_CENTER,0,0);
    
}




/**********************
 *   STATIC FUNCTIONS
 **********************/





