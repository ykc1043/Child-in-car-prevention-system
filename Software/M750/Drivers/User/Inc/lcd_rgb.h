#ifndef  __LCD_RGB_H
#define	__LCD_RGB_H

#include "stm32h7xx_hal.h"
#include "usart.h"
#include "lcd_fonts.h"
#include <stdio.h>
#include <string.h>


/*-------------------------------------------------------- LCD相关参数 -------------------------------------------------------*/

#define HBP  80	// 根据屏幕的手册进行设置
#define VBP  40
#define HSW  1
#define VSW  3
#define HFP  200
#define VFP  22

// 1. FK750M3-VBT6 核心板 使用的是内部AXI SRAM作为显存，起始地址0x24000000，大小为512K
// 2. 显存所需空间 = 分辨率 * 每个像素所占字节数，例如 800*480的屏，使用L8颜色格式（LTDC_PIXEL_FORMAT_L8），需要显存 800*480*1 = 384000 字节
// 3. 不管用户使用什么样的分辨率和格式，设置的显存都不能超过 AXI SRAM 的大小（512K）！！！

#define LCD_Width     			800				//	LCD的像素长度
#define LCD_Height    			480				//	LCD的像素宽度
#define LCD_MemoryAdd   		0x24000000 		//	显存的起始地址  
#define LCD_MemoryAdd_Offset	(0x24000000+LCD_Width*LCD_Height)	//显存偏移地址

// layer0 每个像素所占字节

#define 	BytesPerPixel_0		1		//8位色模式每个像素占1字节

#define	ColorMode_0   LTDC_PIXEL_FORMAT_L8   		//定义 layer0 的颜色格式


/*---------------------------------------------------------- 函数声明 -------------------------------------------------------*/
	
void  MX_LTDC_Init(void);		// LTDC以及层初始化 

void  LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color);	// 复制缓冲区到显示区


/*-------------------------------------------------------- LCD背光引脚 -------------------------------------------------------*/

#define  LCD_Backlight_PIN								GPIO_PIN_15								
#define	LCD_Backlight_PORT							GPIOD									
#define 	GPIO_LDC_Backlight_CLK_ENABLE        	__HAL_RCC_GPIOD_CLK_ENABLE()	 	

#define	LCD_Backlight_OFF		HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_RESET);	// 关闭背光
#define 	LCD_Backlight_ON		HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_SET);		// 开启背光



#endif //__LCD_RGB_H

