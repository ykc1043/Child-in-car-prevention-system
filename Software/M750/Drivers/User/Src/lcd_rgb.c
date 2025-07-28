/***
	************************************************************************************************************************************************************************************************
	*	@file  	lcd_rgb.c
	*	@version V1.0
	*  @date    2020-9-23 
	*	@author  反客科技
	*	@brief   驱动RGB显示屏进行显示	
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	实验平台：1. 反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 4.3寸RGB液晶屏(屏幕型号RGB043M2-800*480)
	*				 2. 反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 5.0寸RGB液晶屏(屏幕型号RGB050M1-800*480)
	*				 3. 反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 7.0寸RGB液晶屏(屏幕型号RGB070M1-800*480)
	*			
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
>>>>> 重要说明：
	*
	*	1. FK750M3-VBT6 核心板 使用的是内部AXI SRAM作为显存，起始地址0x24000000，大小为512K
	*	2. 不管是单层显示还是双层显示，都不能超过 AXI SRAM 的大小
	*	3. 在刚下载完程序时，屏幕有轻微抖动和闪烁属于正常现象，等待片刻或者重新上电即可恢复正常
	* 	4. LTDC时钟在 main.c 文件里的 SystemClock_Config()函数设置，配置为33MHz，即刷新率在60帧左右，过高或者过低都会造成闪烁
	*	5. 受限于显存大小，驱动800*480分辨率的屏幕只能使用 L8颜色格式，即只能显示256种颜色
	* 
>>>>> 关于L8颜色格式：
	*
	*	1. 使用的是间接颜色，即一个颜色索引值对应查色表的实际的颜色值，只能显示256中颜色
	*	2. 在 lcd_clut.h 文件中， L8_CLUT[256] 查色表定义了256种RGB888格式的颜色
	*	3. 用户可自由定义需要使用的颜色，只需将查色表对应的颜色替换掉即可
	*	4. 查色表的颜色色板以及索引可以查看例程附带的图片
	*	5. 使用 lcd_test.c 文件中的 LCD_Test_Color()	可以将整个查色表显示到屏幕上
	*	6. 使用 LCD_SetBackColor() 或者 LCD_SetColor() 输入查色表的颜色索引即可设置对应的颜色，例如 LCD_SetColor(128) 表示设置查色表第128号的颜色
	*	7. 虽然L8颜色格式只能拥有256种颜色值，但是实际输出的还是24位RGB888的颜色
	*	8. FK750M3-VBT6 核心板使用的是RGB666 18位色的颜色接口，实际项目，用户也可以只用RGB565的接口	
	*	
>>>>> 其他说明：
	*
	*	1. 中文字库使用的是小字库，即用到了对应的汉字再去取模，用户可以根据需求自行增添或删减
	*	2. 各个函数的功能和使用可以参考函数的说明以及 lcd_test.c 文件里的测试函数
	*
	*********************************************************************************************************************************************************************************************FANKE*****
***/

#include "lcd_rgb.h"


DMA2D_HandleTypeDef hdma2d;	// DMA2D句柄
LTDC_HandleTypeDef hltdc;		// LTDC句柄



/*************************************************************************************************
*	函 数 名:	HAL_LTDC_MspInit
*	入口参数:	无
*	返 回 值:	无
*	函数功能:	初始化LTDC引脚的IO口
*	说    明:	会被 HAL_LTDC_Init() 函数调用			
*************************************************************************************************/

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hltdc->Instance==LTDC)
	{
		__HAL_RCC_LTDC_CLK_ENABLE();		// 使能LTDC时钟

		__HAL_RCC_GPIOC_CLK_ENABLE();		// 使能IO口时钟
		__HAL_RCC_GPIOA_CLK_ENABLE();		// 使能IO口时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();		// 使能IO口时钟
		__HAL_RCC_GPIOE_CLK_ENABLE();		// 使能IO口时钟
		__HAL_RCC_GPIOD_CLK_ENABLE();		// 使能IO口时钟
		GPIO_LDC_Backlight_CLK_ENABLE;	// 使能背光引脚IO口时钟

/*------------------------------LTDC GPIO Configuration--------------------------
		
FK750M3-VBT6 核心板使用18位接口，RGB666
受限于750内部SRAM的大小，如果不是显示高质量图片或者有大量渐变色彩需要平滑过渡的场合，
使用L8颜色格式即可，可以最大程度减小资源的占用
		
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
		
		GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;			// 复用输出模式
		GPIO_InitStruct.Pull 		= GPIO_NOPULL;					// 无上下拉
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_MEDIUM;	// 速度等级中
		GPIO_InitStruct.Alternate 	= GPIO_AF14_LTDC;				// 复用到AF14的LTDC
		
		GPIO_InitStruct.Pin 			= GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7;		
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	// 初始化 GPIOC 对应的LTDC引脚

		GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;								
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	// 初始化 GPIOA 对应的LTDC引脚

		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;								  
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	// 初始化 GPIOE 对应的LTDC引脚

		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_8|GPIO_PIN_9;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	// 初始化 GPIOB 对应的LTDC引脚

		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_6;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	// 初始化 GPIOD 对应的LTDC引脚


		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;	// 需要注意，PB0和PB1对应的LTDC复用线是在AF9
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	

		LCD_Backlight_OFF;	// 先关闭背光引脚
		
		// 初始化背光引脚
		GPIO_InitStruct.Pin 		= LCD_Backlight_PIN;				// 背光引脚
		GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;			// 推挽输出模式
		GPIO_InitStruct.Pull 	= GPIO_NOPULL;						// 无上下拉
		GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;			// 速度等级低
		HAL_GPIO_Init(LCD_Backlight_PORT, &GPIO_InitStruct);	// 初始化

	}
}


/*************************************************************************************************
*	函    数 :	RGB332_ConvertTo_RGB888
*	入口参数:	Color332 - RGB332颜色
*	返 回 值:	RGB888 颜色
*	函数功能:	颜色转换，将RGB332颜色转换成RGB888
*	说    明:	无			
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
*	函 数 名:	MX_LTDC_Init
*	入口参数:	无
*	返 回 值:	无
*	函数功能:	初始化LTDC引脚的IO口、全局参数、层设置等
*	说    明:	无			
*************************************************************************************************/

void MX_LTDC_Init(void)
{
	uint16_t Index = 0;
	uint32_t RGB332_CLUT[256];
	
	LTDC_LayerCfgTypeDef pLayerCfg = {0};		// layer0 相关参数
	__HAL_RCC_DMA2D_CLK_ENABLE();					// 使能DMA2D时钟

	hltdc.Instance 		 = LTDC;
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;		// 低电平有效
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;     // 低电平有效
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;     // 低电平有效，要注意的是，很多面板都是高电平有效，但是750需要设置成低电平才能正常显示
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;    // 正常时钟信号
  
	hltdc.Init.HorizontalSync 		= HSW - 1;									// 根据屏幕设置参数即可
	hltdc.Init.VerticalSync 		= VSW	-1 ;
	hltdc.Init.AccumulatedHBP		= HBP + HSW -1;
	hltdc.Init.AccumulatedVBP 		= VBP + VSW -1;
	hltdc.Init.AccumulatedActiveW = LCD_Width  + HSW + HBP -1;
	hltdc.Init.AccumulatedActiveH = LCD_Height + VSW + VBP -1;
	hltdc.Init.TotalWidth 			= LCD_Width  + HSW + HBP + HFP - 1; 
	hltdc.Init.TotalHeigh 			= LCD_Height + VSW + VBP + VFP - 1;
	
	hltdc.Init.Backcolor.Red 		= 0;	// 初始背景色，R	
	hltdc.Init.Backcolor.Green 	= 0;  // 初始背景色，G	
	hltdc.Init.Backcolor.Blue 		= 0;	// 初始背景色，B
	
	HAL_LTDC_Init(&hltdc);	// 初始化LTDC参数


/*---------------------------------- layer0 显示配置 --------------------------------*/

	pLayerCfg.WindowX0 			= 0;										// 水平起点
	pLayerCfg.WindowX1 			= LCD_Width;							// 水平终点
	pLayerCfg.WindowY0 			= 0;										// 垂直起点
	pLayerCfg.WindowY1 			= LCD_Height;							// 垂直终点
	pLayerCfg.ImageWidth 		= LCD_Width;                     // 显示区域宽度
	pLayerCfg.ImageHeight 		= LCD_Height;                    // 显示区域高度	
	pLayerCfg.PixelFormat 		= ColorMode_0;							// 颜色格式	

// 配置 layer0 的恒定透明度，最终写入 LTDC_LxCACR 寄存器 
//	需要注意的是，这个参数是直接配置整个 layer0 的透明度，这里设置为255即不透明 
	pLayerCfg.Alpha 				= 255;									// 取值范围0~255，255表示不透明，0表示完全透明

// 设置 layer0 的层混合系数，最终写入 LTDC_LxBFCR 寄存器 
// 该参数用于设置 layer0 和 底层背景 之间的颜色混合系数，计算公式为 ：
// 混合后的颜色 =  BF1 * layer0的颜色 + BF2 * 底层背景的颜色
// 如果 layer0 使用了透明色，则必须配置成 LTDC_BLENDING_FACTOR1_PAxCA 和 LTDC_BLENDING_FACTOR2_PAxCA，否则ARGB中的A通道不起作用
//	更多的介绍可以查阅 参考手册关于 LTDC_LxBFCR 寄存器的介绍
	pLayerCfg.BlendingFactor1 	= LTDC_BLENDING_FACTOR1_CA;		// 混合系数1
	pLayerCfg.BlendingFactor2 	= LTDC_BLENDING_FACTOR2_CA;      // 混合系数2
	
// layer0 的显存地址，本例程使用750内部的AXI SRAM作为显存，起始地址0x24000000，大小为512K
// layer0 显存大小等于 = LCD_Width * LCD_Width * BytesPerPixel_0（每个像素所占字节大小）
// 因为 AXI SRAM 的大小只有512K，用户设置的区域一定不能超过这个值！	
// 在本例程中，800*480的屏幕，使用L8颜色格式（LTDC_PIXEL_FORMAT_L8），需要显存 800*480*1 = 384000 字节
	pLayerCfg.FBStartAdress 	= LCD_MemoryAdd;                 // 显存地址,使用750内部的AXI SRAM作为显存，地址0x24000000
	
	
// 配置 layer0 的初始默认颜色，包括A,R,G,B 的值 ，最终写入 LTDC_LxDCCR 寄存器 
	pLayerCfg.Alpha0 				= 0;			// 初始颜色，A
	pLayerCfg.Backcolor.Blue 	= 0;        //	初始颜色，R
	pLayerCfg.Backcolor.Green 	= 0;        //	初始颜色，G
	pLayerCfg.Backcolor.Red 	= 0;			//	初始颜色，B 
  
	HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);		// 配置layer0

	for (Index = 0; Index < 256; Index++)
	{
		RGB332_CLUT[Index] = RGB332_ConvertTo_RGB888(Index);
	}

	HAL_LTDC_ConfigCLUT(&hltdc, (uint32_t *)RGB332_CLUT, 256, 0); 	// 配置查色表
	HAL_LTDC_EnableCLUT(&hltdc, 0);   // 使能查色表


// LTDC在初始化之后，上电的瞬间会有一个短暂的白屏或撕裂现象（有些屏幕面板不会），
//	即使一开始就将背光引脚拉低并且屏幕背光引脚用电阻下拉还是会有这个现象，
//	如果需要消除这个现象，可以在初始化完毕之后，进行一个短暂的延时再打开背光

//	HAL_Delay(200);	// 延时200ms
	LCD_Backlight_ON;							// 开启背光	

}


/***************************************************************************************************************************************
*	函 数 名: LCD_CopyBuffer
*
*	入口参数: x - 水平坐标
*			 	 y - 垂直坐标
*			 	 width  - 图片的水平宽度
*				 height - 图片的垂直宽度
*				 *color - 要复制的缓冲区地址
*				
*	函数功能: 在坐标 (x,y) 起始处复制缓冲区到显示区
*
*	说    明: 1. 使用DMA2D实现
*				 2. 要绘制的区域不能超过屏幕的显示区域
*				 3. 在 lv_port_disp.c 文件中，被函数 disp_flush() 调用，用以刷新显示区域			 
*
*****************************************************************************************************************************************/

void LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color)
{

	DMA2D->CR	  &=	~(DMA2D_CR_START);				//	停止DMA2D
	DMA2D->CR		=	DMA2D_M2M;							//	存储器到存储器
	DMA2D->FGPFCCR	=	ColorMode_0;						//	设置颜色格式
   DMA2D->FGOR    =  0;										// 
	DMA2D->OOR		=	LCD_Width - width;				//	设置行偏移 	
	DMA2D->FGMAR   =  (uint32_t)color;		
	DMA2D->OMAR		=	LCD_MemoryAdd + BytesPerPixel_0*(LCD_Width * y + x);	// 地址;
	DMA2D->NLR		=	(width<<16)|(height);			//	设定长度和宽度		
	DMA2D->CR	  |=	DMA2D_CR_START;					//	启动DMA2D
		
	while (DMA2D->CR & DMA2D_CR_START) ;				//	等待传输完成

}

/**************************************************************************************************************************************************************************************************************************************************************************FANKE***/
// 实验平台：反客 STM32H750核心板
//

