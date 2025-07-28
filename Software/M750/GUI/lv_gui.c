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
    lv_obj_t * scr = lv_scr_act();  // 获取当前活动屏幕

    // 定义屏幕大小和色块的行列数
    const int screen_width = 800;
    const int screen_height = 480;
    const int block_size = 40;  // 每个色块的边长 (40x40)
    const int cols = screen_width / block_size;  // 色块列数
    const int rows = screen_height / block_size;  // 色块行数

    // 生成所有色块
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // 创建色块对象
            lv_obj_t * block = lv_obj_create(scr);
            lv_obj_set_size(block, block_size, block_size);  // 设置色块大小
            lv_obj_align(block, LV_ALIGN_TOP_LEFT, col * block_size, row * block_size);  // 设置色块位置

            // 根据行列索引计算颜色 (HSV 轮转)
            uint8_t h = (row * cols + col) % 360;  // 色相 (0~360)
            uint8_t s = 100;  // 饱和度
            uint8_t v = 100;  // 亮度
            lv_color_t color = lv_color_hsv_to_rgb(h, s, v);

            // 设置色块颜色
            lv_obj_set_style_bg_color(block, color, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(block, LV_OPA_COVER, LV_PART_MAIN);  // 不透明
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





