/**
 * @file lv_GUI.h
 *
 */

#ifndef LV_GUI_H
#define LV_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "main.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_gui(void);
void lv_gui_start(void);
void lv_gui_activic_system(uint32_t uid0,uint32_t uid1,uint32_t uid2);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*lv_GUI_H*/
