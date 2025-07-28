#ifndef  __LCD_RGB_H
#define	__LCD_RGB_H

#include "stm32h7xx_hal.h"
#include "usart.h"
#include "lcd_fonts.h"
#include <stdio.h>
#include <string.h>


/*-------------------------------------------------------- LCD��ز��� -------------------------------------------------------*/

#define HBP  80	// ������Ļ���ֲ��������
#define VBP  40
#define HSW  1
#define VSW  3
#define HFP  200
#define VFP  22

// 1. FK750M3-VBT6 ���İ� ʹ�õ����ڲ�AXI SRAM��Ϊ�Դ棬��ʼ��ַ0x24000000����СΪ512K
// 2. �Դ�����ռ� = �ֱ��� * ÿ��������ռ�ֽ��������� 800*480������ʹ��L8��ɫ��ʽ��LTDC_PIXEL_FORMAT_L8������Ҫ�Դ� 800*480*1 = 384000 �ֽ�
// 3. �����û�ʹ��ʲô���ķֱ��ʺ͸�ʽ�����õ��Դ涼���ܳ��� AXI SRAM �Ĵ�С��512K��������

#define LCD_Width     			800				//	LCD�����س���
#define LCD_Height    			480				//	LCD�����ؿ��
#define LCD_MemoryAdd   		0x24000000 		//	�Դ����ʼ��ַ  
#define LCD_MemoryAdd_Offset	(0x24000000+LCD_Width*LCD_Height)	//�Դ�ƫ�Ƶ�ַ

// layer0 ÿ��������ռ�ֽ�

#define 	BytesPerPixel_0		1		//8λɫģʽÿ������ռ1�ֽ�

#define	ColorMode_0   LTDC_PIXEL_FORMAT_L8   		//���� layer0 ����ɫ��ʽ


/*---------------------------------------------------------- �������� -------------------------------------------------------*/
	
void  MX_LTDC_Init(void);		// LTDC�Լ����ʼ�� 

void  LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color);	// ���ƻ���������ʾ��


/*-------------------------------------------------------- LCD�������� -------------------------------------------------------*/

#define  LCD_Backlight_PIN								GPIO_PIN_15								
#define	LCD_Backlight_PORT							GPIOD									
#define 	GPIO_LDC_Backlight_CLK_ENABLE        	__HAL_RCC_GPIOD_CLK_ENABLE()	 	

#define	LCD_Backlight_OFF		HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_RESET);	// �رձ���
#define 	LCD_Backlight_ON		HAL_GPIO_WritePin(LCD_Backlight_PORT, LCD_Backlight_PIN, GPIO_PIN_SET);		// ��������



#endif //__LCD_RGB_H

