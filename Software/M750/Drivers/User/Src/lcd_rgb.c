/***
	************************************************************************************************************************************************************************************************
	*	@file  	lcd_rgb.c
	*	@version V1.0
	*  @date    2020-9-23 
	*	@author  ���ͿƼ�
	*	@brief   ����RGB��ʾ��������ʾ	
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨��1. ����STM32H750VBT6���İ� ���ͺţ�FK750M3-VBT6�� + 4.3��RGBҺ����(��Ļ�ͺ�RGB043M2-800*480)
	*				 2. ����STM32H750VBT6���İ� ���ͺţ�FK750M3-VBT6�� + 5.0��RGBҺ����(��Ļ�ͺ�RGB050M1-800*480)
	*				 3. ����STM32H750VBT6���İ� ���ͺţ�FK750M3-VBT6�� + 7.0��RGBҺ����(��Ļ�ͺ�RGB070M1-800*480)
	*			
	*	�Ա���ַ��https://shop212360197.taobao.com
	*	QQ����Ⱥ��536665479
	*
>>>>> ��Ҫ˵����
	*
	*	1. FK750M3-VBT6 ���İ� ʹ�õ����ڲ�AXI SRAM��Ϊ�Դ棬��ʼ��ַ0x24000000����СΪ512K
	*	2. �����ǵ�����ʾ����˫����ʾ�������ܳ��� AXI SRAM �Ĵ�С
	*	3. �ڸ����������ʱ����Ļ����΢��������˸�����������󣬵ȴ�Ƭ�̻��������ϵ缴�ɻָ�����
	* 	4. LTDCʱ���� main.c �ļ���� SystemClock_Config()�������ã�����Ϊ33MHz����ˢ������60֡���ң����߻��߹��Ͷ��������˸
	*	5. �������Դ��С������800*480�ֱ��ʵ���Ļֻ��ʹ�� L8��ɫ��ʽ����ֻ����ʾ256����ɫ
	* 
>>>>> ����L8��ɫ��ʽ��
	*
	*	1. ʹ�õ��Ǽ����ɫ����һ����ɫ����ֵ��Ӧ��ɫ���ʵ�ʵ���ɫֵ��ֻ����ʾ256����ɫ
	*	2. �� lcd_clut.h �ļ��У� L8_CLUT[256] ��ɫ������256��RGB888��ʽ����ɫ
	*	3. �û������ɶ�����Ҫʹ�õ���ɫ��ֻ�轫��ɫ���Ӧ����ɫ�滻������
	*	4. ��ɫ�����ɫɫ���Լ��������Բ鿴���̸�����ͼƬ
	*	5. ʹ�� lcd_test.c �ļ��е� LCD_Test_Color()	���Խ�������ɫ����ʾ����Ļ��
	*	6. ʹ�� LCD_SetBackColor() ���� LCD_SetColor() �����ɫ�����ɫ�����������ö�Ӧ����ɫ������ LCD_SetColor(128) ��ʾ���ò�ɫ���128�ŵ���ɫ
	*	7. ��ȻL8��ɫ��ʽֻ��ӵ��256����ɫֵ������ʵ������Ļ���24λRGB888����ɫ
	*	8. FK750M3-VBT6 ���İ�ʹ�õ���RGB666 18λɫ����ɫ�ӿڣ�ʵ����Ŀ���û�Ҳ����ֻ��RGB565�Ľӿ�	
	*	
>>>>> ����˵����
	*
	*	1. �����ֿ�ʹ�õ���С�ֿ⣬���õ��˶�Ӧ�ĺ�����ȥȡģ���û����Ը����������������ɾ��
	*	2. ���������Ĺ��ܺ�ʹ�ÿ��Բο�������˵���Լ� lcd_test.c �ļ���Ĳ��Ժ���
	*
	*********************************************************************************************************************************************************************************************FANKE*****
***/

#include "lcd_rgb.h"


DMA2D_HandleTypeDef hdma2d;	// DMA2D���
LTDC_HandleTypeDef hltdc;		// LTDC���



/*************************************************************************************************
*	�� �� ��:	HAL_LTDC_MspInit
*	��ڲ���:	��
*	�� �� ֵ:	��
*	��������:	��ʼ��LTDC���ŵ�IO��
*	˵    ��:	�ᱻ HAL_LTDC_Init() ��������			
*************************************************************************************************/

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hltdc->Instance==LTDC)
	{
		__HAL_RCC_LTDC_CLK_ENABLE();		// ʹ��LTDCʱ��

		__HAL_RCC_GPIOC_CLK_ENABLE();		// ʹ��IO��ʱ��
		__HAL_RCC_GPIOA_CLK_ENABLE();		// ʹ��IO��ʱ��
		__HAL_RCC_GPIOB_CLK_ENABLE();		// ʹ��IO��ʱ��
		__HAL_RCC_GPIOE_CLK_ENABLE();		// ʹ��IO��ʱ��
		__HAL_RCC_GPIOD_CLK_ENABLE();		// ʹ��IO��ʱ��
		GPIO_LDC_Backlight_CLK_ENABLE;	// ʹ�ܱ�������IO��ʱ��

/*------------------------------LTDC GPIO Configuration--------------------------
		
FK750M3-VBT6 ���İ�ʹ��18λ�ӿڣ�RGB666
������750�ڲ�SRAM�Ĵ�С�����������ʾ������ͼƬ�����д�������ɫ����Ҫƽ�����ɵĳ��ϣ�
ʹ��L8��ɫ��ʽ���ɣ��������̶ȼ�С��Դ��ռ��
		
		PE15    ------> LTDC_R7		PD3     ------> LTDC_G7 	PB9     ------> LTDC_B7
		PB1     ------> LTDC_R6		PC7     ------> LTDC_G6 	PB8     ------> LTDC_B6
		PC0     ------> LTDC_R5		PB11    ------> LTDC_G5		PA3     ------> LTDC_B5
		PA5     ------> LTDC_R4		PB10    ------> LTDC_G4		PE12    ------> LTDC_B4
		PB0     ------> LTDC_R3		PE11    ------> LTDC_G3 	PD10    ------> LTDC_B3
		PA1     ------> LTDC_R2		PA6     ------> LTDC_G2		PD6     ------> LTDC_B2
			
		PA4     ------> LTDC_VSYNC
		PE13    ------> LTDC_DE
		PE14    ------> LTDC_CLK
		PC6     ------> LTDC_HSYNC
		
----------------------------------------------------------------------------------*/
		
		GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 		= GPIO_NOPULL;					// ��������
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_MEDIUM;	// �ٶȵȼ���
		GPIO_InitStruct.Alternate 	= GPIO_AF14_LTDC;				// ���õ�AF14��LTDC
		
		GPIO_InitStruct.Pin 			= GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7;		
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	// ��ʼ�� GPIOC ��Ӧ��LTDC����

		GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;								
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	// ��ʼ�� GPIOA ��Ӧ��LTDC����

		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;								  
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	// ��ʼ�� GPIOE ��Ӧ��LTDC����

		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_8|GPIO_PIN_9;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	// ��ʼ�� GPIOB ��Ӧ��LTDC����

		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_6;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	// ��ʼ�� GPIOD ��Ӧ��LTDC����


		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;	// ��Ҫע�⣬PB0��PB1��Ӧ��LTDC����������AF9
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	

		LCD_Backlight_OFF;	// �ȹرձ�������
		
		// ��ʼ����������
		GPIO_InitStruct.Pin 		= LCD_Backlight_PIN;				// ��������
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// �������ģʽ
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;						// ��������
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// �ٶȵȼ���
		HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct);	// ��ʼ��

	}
}


/*************************************************************************************************
*	��    �� :	RGB332_ConvertTo_RGB888
*	��ڲ���:	Color332 - RGB332��ɫ
*	�� �� ֵ:	RGB888 ��ɫ
*	��������:	��ɫת������RGB332��ɫת����RGB888
*	˵    ��:	��			
*************************************************************************************************/

uint32_t RGB332_ConvertTo_RGB888(uint32_t Color332)
{
	uint32_t r, g, b;

	r = (Color332 & 0xe0) << 16;
	g = (Color332 & 0x1c) << 11;
	b = (Color332 & 0x03) << 6;
	return (r|g|b);		
}

/*************************************************************************************************
*	�� �� ��:	MX_LTDC_Init
*	��ڲ���:	��
*	�� �� ֵ:	��
*	��������:	��ʼ��LTDC���ŵ�IO�ڡ�ȫ�ֲ����������õ�
*	˵    ��:	��			
*************************************************************************************************/

void MX_LTDC_Init(void)
{
	uint16_t Index = 0;
	uint32_t RGB332_CLUT[256];
	
	LTDC_LayerCfgTypeDef pLayerCfg = {0};		// layer0 ��ز���
	__HAL_RCC_DMA2D_CLK_ENABLE();					// ʹ��DMA2Dʱ��

	hltdc.Instance 		 = LTDC;
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;		// �͵�ƽ��Ч
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;     // �͵�ƽ��Ч
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;     // �͵�ƽ��Ч��Ҫע����ǣ��ܶ���嶼�Ǹߵ�ƽ��Ч������750��Ҫ���óɵ͵�ƽ����������ʾ
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;    // ����ʱ���ź�
  
	hltdc.Init.HorizontalSync 		= HSW - 1;									// ������Ļ���ò�������
	hltdc.Init.VerticalSync 		= VSW	-1 ;
	hltdc.Init.AccumulatedHBP		= HBP + HSW -1;
	hltdc.Init.AccumulatedVBP 		= VBP + VSW -1;
	hltdc.Init.AccumulatedActiveW = LCD_Width  + HSW + HBP -1;
	hltdc.Init.AccumulatedActiveH = LCD_Height + VSW + VBP -1;
	hltdc.Init.TotalWidth 			= LCD_Width  + HSW + HBP + HFP - 1; 
	hltdc.Init.TotalHeigh 			= LCD_Height + VSW + VBP + VFP - 1;
	
	hltdc.Init.Backcolor.Red 		= 0;	// ��ʼ����ɫ��R	
	hltdc.Init.Backcolor.Green 	= 0;  // ��ʼ����ɫ��G	
	hltdc.Init.Backcolor.Blue 		= 0;	// ��ʼ����ɫ��B
	
	HAL_LTDC_Init(&hltdc);	// ��ʼ��LTDC����


/*---------------------------------- layer0 ��ʾ���� --------------------------------*/

	pLayerCfg.WindowX0 			= 0;										// ˮƽ���
	pLayerCfg.WindowX1 			= LCD_Width;							// ˮƽ�յ�
	pLayerCfg.WindowY0 			= 0;										// ��ֱ���
	pLayerCfg.WindowY1 			= LCD_Height;							// ��ֱ�յ�
	pLayerCfg.ImageWidth 		= LCD_Width;                     // ��ʾ������
	pLayerCfg.ImageHeight 		= LCD_Height;                    // ��ʾ����߶�	
	pLayerCfg.PixelFormat 		= ColorMode_0;							// ��ɫ��ʽ	

// ���� layer0 �ĺ㶨͸���ȣ�����д�� LTDC_LxCACR �Ĵ��� 
//	��Ҫע����ǣ����������ֱ���������� layer0 ��͸���ȣ���������Ϊ255����͸�� 
	pLayerCfg.Alpha 				= 255;									// ȡֵ��Χ0~255��255��ʾ��͸����0��ʾ��ȫ͸��

// ���� layer0 �Ĳ���ϵ��������д�� LTDC_LxBFCR �Ĵ��� 
// �ò����������� layer0 �� �ײ㱳�� ֮�����ɫ���ϵ�������㹫ʽΪ ��
// ��Ϻ����ɫ =  BF1 * layer0����ɫ + BF2 * �ײ㱳������ɫ
// ��� layer0 ʹ����͸��ɫ����������ó� LTDC_BLENDING_FACTOR1_PAxCA �� LTDC_BLENDING_FACTOR2_PAxCA������ARGB�е�Aͨ����������
//	����Ľ��ܿ��Բ��� �ο��ֲ���� LTDC_LxBFCR �Ĵ����Ľ���
	pLayerCfg.BlendingFactor1 	= LTDC_BLENDING_FACTOR1_CA;		// ���ϵ��1
	pLayerCfg.BlendingFactor2 	= LTDC_BLENDING_FACTOR2_CA;      // ���ϵ��2
	
// layer0 ���Դ��ַ��������ʹ��750�ڲ���AXI SRAM��Ϊ�Դ棬��ʼ��ַ0x24000000����СΪ512K
// layer0 �Դ��С���� = LCD_Width * LCD_Width * BytesPerPixel_0��ÿ��������ռ�ֽڴ�С��
// ��Ϊ AXI SRAM �Ĵ�Сֻ��512K���û����õ�����һ�����ܳ������ֵ��	
// �ڱ������У�800*480����Ļ��ʹ��L8��ɫ��ʽ��LTDC_PIXEL_FORMAT_L8������Ҫ�Դ� 800*480*1 = 384000 �ֽ�
	pLayerCfg.FBStartAdress 	= LCD_MemoryAdd;                 // �Դ��ַ,ʹ��750�ڲ���AXI SRAM��Ϊ�Դ棬��ַ0x24000000
	
	
// ���� layer0 �ĳ�ʼĬ����ɫ������A,R,G,B ��ֵ ������д�� LTDC_LxDCCR �Ĵ��� 
	pLayerCfg.Alpha0 				= 0;			// ��ʼ��ɫ��A
	pLayerCfg.Backcolor.Blue 	= 0;        //	��ʼ��ɫ��R
	pLayerCfg.Backcolor.Green 	= 0;        //	��ʼ��ɫ��G
	pLayerCfg.Backcolor.Red 	= 0;			//	��ʼ��ɫ��B 
  
	HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);		// ����layer0

	for (Index = 0; Index < 256; Index++)
	{
		RGB332_CLUT[Index] = RGB332_ConvertTo_RGB888(Index);
	}

	HAL_LTDC_ConfigCLUT(&hltdc, (uint32_t *)RGB332_CLUT, 256, 0); 	// ���ò�ɫ��
	HAL_LTDC_EnableCLUT(&hltdc, 0);   // ʹ�ܲ�ɫ��


// LTDC�ڳ�ʼ��֮���ϵ��˲�����һ�����ݵİ�����˺��������Щ��Ļ��岻�ᣩ��
//	��ʹһ��ʼ�ͽ������������Ͳ�����Ļ���������õ����������ǻ����������
//	�����Ҫ����������󣬿����ڳ�ʼ�����֮�󣬽���һ�����ݵ���ʱ�ٴ򿪱���

//	HAL_Delay(200);	// ��ʱ200ms
	LCD_Backlight_ON;							// ��������	

}


/***************************************************************************************************************************************
*	�� �� ��: LCD_CopyBuffer
*
*	��ڲ���: x - ˮƽ����
*			 	 y - ��ֱ����
*			 	 width  - ͼƬ��ˮƽ���
*				 height - ͼƬ�Ĵ�ֱ���
*				 *color - Ҫ���ƵĻ�������ַ
*				
*	��������: ������ (x,y) ��ʼ�����ƻ���������ʾ��
*
*	˵    ��: 1. ʹ��DMA2Dʵ��
*				 2. Ҫ���Ƶ������ܳ�����Ļ����ʾ����
*				 3. �� lv_port_disp.c �ļ��У������� disp_flush() ���ã�����ˢ����ʾ����			 
*
*****************************************************************************************************************************************/

void LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color)
{

	DMA2D->CR	  &=	~(DMA2D_CR_START);				//	ֹͣDMA2D
	DMA2D->CR		=	DMA2D_M2M;							//	�洢�����洢��
	DMA2D->FGPFCCR	=	ColorMode_0;						//	������ɫ��ʽ
   DMA2D->FGOR    =  0;										// 
	DMA2D->OOR		=	LCD_Width - width;				//	������ƫ�� 	
	DMA2D->FGMAR   =  (uint32_t)color;		
	DMA2D->OMAR		=	LCD_MemoryAdd + BytesPerPixel_0*(LCD_Width * y + x);	// ��ַ;
	DMA2D->NLR		=	(width<<16)|(height);			//	�趨���ȺͿ��		
	DMA2D->CR	  |=	DMA2D_CR_START;					//	����DMA2D
		
	while (DMA2D->CR & DMA2D_CR_START) ;				//	�ȴ��������

}

/**************************************************************************************************************************************************************************************************************************************************************************FANKE***/
// ʵ��ƽ̨������ STM32H750���İ�
//

