/***
	***************************************************************************************************************************
	*	@file  	lcd_test.c
	*	@version V1.0
	*  @date    2020-9-14
	*	@author  反客科技
	*	@brief   LTDC测试函数	
   ***************************************************************************************************************************
   *  @description
	*
	*	实验平台：1. 反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 4.3寸RGB液晶屏(屏幕型号RGB043M2-800*480)
	*				 2. 反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 5.0寸RGB液晶屏(屏幕型号RGB050M1-800*480)
	*				 3. 反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 7.0寸RGB液晶屏(屏幕型号RGB070M1-800*480)
	*			
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
>>>>> 文件说明：
	*
	*	1. 仅用于测试，该文件不是必须，用户移植的时候可以舍弃
	*
	****************************************************************************************************************************
***/


#include "lcd_test.h"



/*************************************************************************************************
*	函 数 名:	LCD_Test_Clear
*
*	函数功能:	清屏测试
*
*	说    明:	无	
*************************************************************************************************/

void LCD_Test_Clear(void)
{
	uint16_t time = 1000;	// 延时时间
	uint8_t	i = 0;			// 计数变量
	
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>
	
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"刷屏测试");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }	
	
// 刷屏测试>>>>>
		
	LCD_SetTextFont(&CH_Font32);			// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(LCD_BLACK);				// 设置画笔颜色

	for(i=0;i<8;i++)
	{
		switch (i)		// 切换背景色
		{
			case 0: LCD_SetBackColor(LIGHT_RED); 		break;	
			case 1: LCD_SetBackColor(LIGHT_GREEN); 	break;				
			case 2: LCD_SetBackColor(LIGHT_BLUE); 		break;
			case 3: LCD_SetBackColor(LIGHT_YELLOW); 	break;
			case 4: LCD_SetBackColor(LIGHT_CYAN);		break;
			case 5: LCD_SetBackColor(LIGHT_GREY); 		break;
			case 6: LCD_SetBackColor(LIGHT_MAGENTA); 	break;
			case 7: LCD_SetBackColor(LCD_WHITE); 		break;			
			default:	break;			
		}
		LCD_Clear();		// 清屏
		LCD_DisplayText(112, 84,"STM32H750 LTDC 刷屏测试");
		LCD_DisplayText(112,134,"核心板型号：FK750M3-VBT6");
		LCD_DisplayText(112,184,"屏幕分辨率：800*480");	
		LCD_DisplayText(112,234,"颜色格式：L8");	
		HAL_Delay(time);	// 延时
	}
}

/*************************************************************************************************
*	函 数 名:	LCD_Test_Text
*
*	函数功能:	文本显示测试
*
*	说    明:	无	
*************************************************************************************************/

void LCD_Test_Text(void)
{
	uint16_t i;					// 计数变量
	uint16_t time = 80;	// 延时时间
	
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>
	
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"文本显示");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }	
	
// 显示文本，包括各种字体大小的中文和ASCII字符 >>>>>

	LCD_SetBackColor(LCD_BLACK); 			//	设置背景色
	LCD_Clear(); 								// 清屏
	
	LCD_SetColor(LCD_WHITE);	// 设置画笔，白色
	LCD_SetFont(&Font32); LCD_DisplayString(0,  0,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);		
	LCD_SetFont(&Font24); LCD_DisplayString(0, 32,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);
	LCD_SetFont(&Font20); LCD_DisplayString(0, 56,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);	
	LCD_SetFont(&Font16); LCD_DisplayString(0, 76,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);	
	LCD_SetFont(&Font12); LCD_DisplayString(0, 92,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);	
																																		  
	LCD_SetColor(LCD_CYAN);                                                                              
	LCD_SetFont(&Font12); LCD_DisplayString(0,104,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);	
	LCD_SetFont(&Font16); LCD_DisplayString(0,116,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);	
	LCD_SetFont(&Font20); LCD_DisplayString(0,132,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);		
	LCD_SetFont(&Font24); LCD_DisplayString(0,152,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);		
	LCD_SetFont(&Font32); LCD_DisplayString(0,176,"!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRST"); HAL_Delay(time);	

	LCD_SetTextFont(&CH_Font24);			// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(LCD_YELLOW);				// 设置画笔，黄色
	LCD_DisplayText(0, 230,"文本显示，可显示中文和ASCII字符集");
	LCD_DisplayText(0, 260,"用户可根据需求，对字库进行增添和删减");	

	
	LCD_SetTextFont(&CH_Font12);			// 设置1212中文字体,ASCII字体对应为1206
	LCD_SetColor(142);						// 设置画笔
	LCD_DisplayText(28, 290,"1212中文字体：反客科技");	
	
	LCD_SetTextFont(&CH_Font16);			// 设置1616中文字体,ASCII字体对应为1608
	LCD_SetColor(135);						// 设置画笔
	LCD_DisplayText(28, 310,"1616中文字体：反客科技");		
	
	LCD_SetTextFont(&CH_Font20);			// 设置2020中文字体,ASCII字体对应为2010
	LCD_SetColor(154);						// 设置画笔
	LCD_DisplayText(28, 335,"2020中文字体：反客科技");		

	LCD_SetTextFont(&CH_Font24);			// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(171);						// 设置画笔
	LCD_DisplayText(28, 365,"2424中文字体：反客科技");		

	LCD_SetTextFont(&CH_Font32);			// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(189);						// 设置画笔
	LCD_DisplayText(28, 405,"3232中文字体：反客科技");		

	HAL_Delay(3000);	// 延时等待
}

/*************************************************************************************************
*	函 数 名:	LCD_Test_Variable
*
*	函数功能:	变量显示，包括整数和小数
*
*	说    明:	无	
*************************************************************************************************/

void LCD_Test_Variable (void)
{

	uint16_t i;					// 计数变量

	int32_t	a = 0;			// 定义整数变量，用于测试
	int32_t	b = 0;			// 定义整数变量，用于测试
	int32_t	c = 0;			// 定义整数变量，用于测试

	double p = 3.1415926;	// 定义浮点数变量，用于测试
	double f = -1234.1234;	// 定义浮点数变量，用于测试
	
	
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>	
		
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"变量显示");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }		
	
// 显示正整数、负整数、小数>>>>>	
	
	LCD_SetBackColor(LCD_BLACK); 			//	设置背景色
	LCD_Clear(); 								// 清屏
	
	LCD_SetTextFont(&CH_Font32);					// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(LCD_WHITE);						// 设置画笔,白色
	
	LCD_DisplayText(28, 20,"多余位填充空格");	// 显示文本	
	LCD_DisplayText(400,20,"多余位填充0");		// 显示文本		
	
	LCD_SetColor(LIGHT_CYAN);					// 设置画笔，蓝绿色
	LCD_DisplayText(28, 60,"正整数：");				
	LCD_DisplayText(28,100,"正整数：");				
	LCD_DisplayText(28,140,"负整数：");					
				
	LCD_SetColor(LIGHT_YELLOW);				// 设置画笔，亮黄色		
	LCD_DisplayText(400, 60,"正整数：");	
	LCD_DisplayText(400,100,"正整数：");	
	LCD_DisplayText(400,140,"负整数：");	
			
	LCD_SetColor(LIGHT_RED);					// 设置画笔	，亮红色		
	LCD_DisplayText(28, 200,"正小数：");	
	LCD_DisplayText(28, 240,"负小数：");			
			
	for(i=0;i<100;i++)
   {
		LCD_SetColor(LIGHT_CYAN);									// 设置画笔	，蓝绿色	
		LCD_ShowNumMode(Fill_Space);								// 多余位填充空格
		LCD_DisplayNumber( 160, 60, a+i*125, 8) ;				// 显示变量		
		LCD_DisplayNumber( 160,100, b+i, 	 6) ;				// 显示变量			
		LCD_DisplayNumber( 160,140, c-i,     6) ;				// 显示变量			
		
		LCD_SetColor(LIGHT_YELLOW);								// 设置画笔，亮黄色	
		LCD_ShowNumMode(Fill_Zero);								// 多余位填充0
		LCD_DisplayNumber( 560, 60, a+i*125, 8) ;				// 显示变量		
		LCD_DisplayNumber( 560,100, b+i, 	 6) ;				// 显示变量			
		LCD_DisplayNumber( 560,140, c-i,     6) ;				// 显示变量				
		
		LCD_SetColor(LIGHT_RED);									// 设置画笔，亮红色			
		LCD_ShowNumMode(Fill_Space);								// 多余位填充空格		
		LCD_DisplayDecimals( 160, 200, p+i*0.1,  6,3);		// 显示小数	
		LCD_DisplayDecimals( 160, 240, f+i*0.01, 11,4);		// 显示小数		
		
		HAL_Delay(30);				
   }
	
	HAL_Delay(2500);		
	
}



/*************************************************************************************************
*	函 数 名:	LCD_Test_FillRect
*
*	函数功能:	矩形填充测试
*
*	说    明:	无	
*************************************************************************************************/

void LCD_Test_FillRect(void)
{
	uint16_t i;					// 计数变量
	
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>

	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"矩形绘制");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }			
	
// 矩形填充>>>>>	
	
	LCD_SetBackColor(LCD_BLACK); //设置背景色
	LCD_Clear(); //清屏，刷背景色

	LCD_SetFont(&Font16);
	LCD_SetColor(LCD_BLUE);	  	  LCD_FillRect(200,  3,500,16);	LCD_DisplayString(0,  3,"LCD_BLUE");		
	LCD_SetColor(LCD_GREEN);  	  LCD_FillRect(200, 25,500,16);  LCD_DisplayString(0, 25,"LCD_GREEN");		
	LCD_SetColor(LCD_RED);    	  LCD_FillRect(200, 47,500,16);  LCD_DisplayString(0, 47,"LCD_RED");			
	LCD_SetColor(LCD_CYAN);   	  LCD_FillRect(200, 69,500,16);  LCD_DisplayString(0, 69,"LCD_CYAN");		
	LCD_SetColor(LCD_MAGENTA);	  LCD_FillRect(200, 91,500,16);  LCD_DisplayString(0, 91,"LCD_MAGENTA");	
	LCD_SetColor(LCD_YELLOW); 	  LCD_FillRect(200,113,500,16);  LCD_DisplayString(0,113,"LCD_YELLOW");		
	LCD_SetColor(LCD_GREY);   	  LCD_FillRect(200,135,500,16);	LCD_DisplayString(0,135,"LCD_GREY");		

	LCD_SetColor(LIGHT_BLUE);	  LCD_FillRect(200,157,500,16);  LCD_DisplayString(0,157,"LIGHT_BLUE");		
	LCD_SetColor(LIGHT_GREEN);   LCD_FillRect(200,179,500,16);  LCD_DisplayString(0,179,"LIGHT_GREEN");	
	LCD_SetColor(LIGHT_RED);     LCD_FillRect(200,201,500,16);  LCD_DisplayString(0,201,"LIGHT_RED");	   
	LCD_SetColor(LIGHT_CYAN);    LCD_FillRect(200,223,500,16);  LCD_DisplayString(0,223,"LIGHT_CYAN");	   
	LCD_SetColor(LIGHT_MAGENTA); LCD_FillRect(200,245,500,16);  LCD_DisplayString(0,245,"LIGHT_MAGENTA");	
	LCD_SetColor(LIGHT_YELLOW);  LCD_FillRect(200,267,500,16);  LCD_DisplayString(0,267,"LIGHT_YELLOW");	
	LCD_SetColor(LIGHT_GREY);    LCD_FillRect(200,289,500,16);	LCD_DisplayString(0,289,"LIGHT_GREY");  	

	LCD_SetColor(DARK_BLUE);	  LCD_FillRect(200,311,500,16);  LCD_DisplayString(0,311,"DARK_BLUE");		
	LCD_SetColor(DARK_GREEN);    LCD_FillRect(200,333,500,16);  LCD_DisplayString(0,333,"DARK_GREEN");		
	LCD_SetColor(DARK_RED);      LCD_FillRect(200,355,500,16);  LCD_DisplayString(0,355,"DARK_RED");		
	LCD_SetColor(DARK_CYAN);     LCD_FillRect(200,377,500,16);  LCD_DisplayString(0,377,"DARK_CYAN");		
	LCD_SetColor(DARK_MAGENTA);  LCD_FillRect(200,399,500,16);  LCD_DisplayString(0,399,"DARK_MAGENTA");	
	LCD_SetColor(DARK_YELLOW);   LCD_FillRect(200,421,500,16);  LCD_DisplayString(0,421,"DARK_YELLOW");	
	LCD_SetColor(DARK_GREY);     LCD_FillRect(200,443,500,16);	LCD_DisplayString(0,443,"DARK_GREY");	

	HAL_Delay(2500);
}


/*************************************************************************************************
*	函 数 名:	LCD_Test_Color
*
*	函数功能:	颜色测试
*
*	说    明:	将查色表256种颜色显示出来
*************************************************************************************************/

void LCD_Test_Color(void)
{
	uint16_t i;
	uint8_t x,y,color=0;
	
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>
	
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"颜色绘制");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }		
	
// 颜色测试>>>>>
		
	LCD_SetTextFont(&CH_Font32);			// 设置2424中文字体,ASCII字体对应为2412

	for( i=0; i<2; i++)
   {
		LCD_SetBackColor(LCD_BLACK); //设置背景色
		LCD_Clear(); //清屏，刷背景色		
		LCD_SetColor(LCD_WHITE);				// 设置画笔颜色
		LCD_DisplayText(221,420,"L8颜色格式，总计256种颜色");	
		
		for( y=0; y<8; y++)
		{
			for( x=0; x<16; x++)
			{
				LCD_SetColor(color);		// 设置颜色
				LCD_FillRect( 5+(x*50), 6+(y*50), 40, 40);	// 填充矩形
				color++;
			}
		}
		HAL_Delay(3000);	
	}

	
}

/*************************************************************************************************
*	函 数 名:	LCD_Test_GrahicTest
*
*	函数功能:	2D图形绘制
*
*	说    明:	无	
*************************************************************************************************/

void LCD_Test_GrahicTest(void)
{
	uint16_t time = 80;		// 延时时间
	uint16_t i;					// 计数变量
		
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>
	
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"绘图测试");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }					
	
// 2D图形绘制>>>>>>>
	
	LCD_SetBackColor(LCD_BLACK); //设置背景色
	LCD_Clear(); //清屏，刷背景色	

	LCD_SetColor(LCD_RED);    LCD_FillCircle(120,350,80);		//填充圆形
	LCD_SetColor(LCD_GREEN);  LCD_FillCircle(170,350,80); 	
	LCD_SetColor(LCD_BLUE);   LCD_FillCircle(220,350,80);  	
	
	LCD_SetColor(LIGHT_GREY);
	LCD_DrawLine(5,5,400,5);	HAL_Delay(time);			//画直线
	LCD_DrawLine(5,10,300,10);	HAL_Delay(time);
	LCD_DrawLine(5,15,200,15); HAL_Delay(time);
	LCD_DrawLine(5,20,100,20);	HAL_Delay(time);	

	LCD_SetColor(LIGHT_CYAN);
	LCD_DrawCircle(600,120,100);	HAL_Delay(time);		//绘制圆形
	LCD_DrawCircle(600,120,80);   HAL_Delay(time);
	LCD_DrawCircle(600,120,60);   HAL_Delay(time);
	LCD_DrawCircle(600,120,40);   HAL_Delay(time);
	
	LCD_SetColor(LCD_RED);	
	LCD_DrawRect(5,35,400,150);  HAL_Delay(time);			//绘制矩形
	LCD_DrawRect(30,50,350,120); HAL_Delay(time);
	LCD_DrawRect(55,65,300,90);  HAL_Delay(time);
	LCD_DrawRect(80,80,250,60);  HAL_Delay(time);

	LCD_SetColor(LIGHT_MAGENTA);	
	LCD_DrawEllipse(590,350,200,100); HAL_Delay(time);	//绘制椭圆
	LCD_DrawEllipse(590,350,170,80);  HAL_Delay(time);
	LCD_DrawEllipse(590,350,140,60);  HAL_Delay(time);
	LCD_DrawEllipse(590,350,110,40);  HAL_Delay(time);

	HAL_Delay(2000);	
}
/*************************************************************************************************
*	函 数 名:	LCD_Test_Image
*
*	函数功能:	图片显示测试
*
*	说    明:	无	
*************************************************************************************************/

void LCD_Test_Image(void)
{
	uint16_t i;					// 计数变量
		
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>
	
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(334, 160,"图片绘制");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }		
	
	
// 图片绘制>>>>>>>	

	LCD_SetBackColor(LCD_BLACK); 			//	设置背景色
	LCD_Clear(); 								// 清屏
	
	LCD_SetColor( 72);
	LCD_DrawImage( 185, 116, 83, 83, Image_Android_83x83) ;	// 显示图片
	
	LCD_SetColor( 183);
	LCD_DrawImage( 359, 124, 83, 83, Image_Cloud_83x83) ;		// 显示图片
	
	LCD_SetColor( 142);
	LCD_DrawImage( 533, 124, 83, 83, Image_Folder_83x83) ;	// 显示图片

	LCD_SetColor( 76);
	LCD_DrawImage( 185, 270, 83, 83, Image_Message_83x83) ;	// 显示图片
	
	LCD_SetColor( 184);
	LCD_DrawImage( 359, 270, 83, 83, Image_Toys_83x83) ;		// 显示图片
	
	LCD_SetColor( 239);
	LCD_DrawImage( 533, 270, 83, 83, Image_Video_83x83) ;		// 显示图片

	HAL_Delay(2000);
	
	LCD_SetBackColor(LCD_WHITE); 			//	设置背景色
	LCD_Clear(); 								// 清屏
	LCD_SetColor( LCD_BLACK);				// 设置画笔
	LCD_DrawImage( 159, 120, 480, 239, Image_FANKE_480x239) ;	// 显示图片
	HAL_Delay(2000);
	
}


/*************************************************************************************************
*	函 数 名:	LCD_Test_Vertical
*
*	函数功能:	竖屏测试
*
*	说    明:	无	
*************************************************************************************************/

void  LCD_Test_Vertical(void)
{
	uint16_t i;
	uint16_t time = 100;	
	
	LCD_DisplayDirection(Direction_V); // 切换到竖屏显示
	
// 绘制初始界面，包括标题、LOGO以及进度条>>>>>
	
	LCD_SetBackColor(142); 			//	设置背景色
	LCD_Clear(); 						//	清屏，刷背景色
	
	LCD_SetTextFont(&CH_Font32);				// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(LCD_BLACK);					//	设置画笔色
	LCD_DisplayText(180, 260,"竖屏显示");	// 显示文本
	
	LCD_SetColor(221);					//	设置画笔色
	LCD_DrawImage( 120, 320, 240, 83, Image_FANKE_240x83) ;		// 显示LOGO图片

	LCD_SetColor(LIGHT_YELLOW);		//	设置画笔色
	for(i=0;i<130;i++)
   {
		LCD_FillRect(45,450,3*i,6);	// 绘制矩形，实现简易进度条的效果
		HAL_Delay(10);	
   }

	
// 绘制其它内容>>>>>>>		
	
	LCD_SetBackColor(LCD_BLACK); //设置背景色
	LCD_Clear(); //清屏，刷背景色		

	LCD_SetColor(LCD_WHITE);
	LCD_SetFont(&Font32);LCD_DisplayString(0, 0,"!#$%&'()*+,-.0123456789:;<=>"); HAL_Delay(time);		
	LCD_SetFont(&Font24);LCD_DisplayString(0,32,"!#$%&'()*+,-.0123456789:;<=>"); HAL_Delay(time);
	LCD_SetFont(&Font20);LCD_DisplayString(0,56,"!#$%&'()*+,-.0123456789:;<=>"); HAL_Delay(time);	
	LCD_SetFont(&Font16);LCD_DisplayString(0,76,"!#$%&'()*+,-.0123456789:;<=>"); HAL_Delay(time);	
	LCD_SetFont(&Font12);LCD_DisplayString(0,92,"!#$%&'()*+,-.0123456789:;<=>"); HAL_Delay(time);	
                                                                                                      
	LCD_SetColor(LCD_CYAN);                                                                            
	LCD_SetFont(&Font12);LCD_DisplayString(0,104,"!#$%&'()*+,-.0123456789:;<=>");HAL_Delay(time);	
	LCD_SetFont(&Font16);LCD_DisplayString(0,116,"!#$%&'()*+,-.0123456789:;<=>");HAL_Delay(time);	
	LCD_SetFont(&Font20);LCD_DisplayString(0,132,"!#$%&'()*+,-.0123456789:;<=>");HAL_Delay(time);		
	LCD_SetFont(&Font24);LCD_DisplayString(0,152,"!#$%&'()*+,-.0123456789:;<=>");HAL_Delay(time);		
	LCD_SetFont(&Font32);LCD_DisplayString(0,176,"!#$%&'()*+,-.0123456789:;<=>");HAL_Delay(time);	

	LCD_SetTextFont(&CH_Font24);			// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(LCD_YELLOW);				// 设置画笔，黄色
	LCD_DisplayText(0, 230,"文本显示，可显示中文和ASCII字符集");
	LCD_DisplayText(0, 260,"用户可根据需求，对字库进行增添和删减");	

	
	LCD_SetTextFont(&CH_Font12);			// 设置1212中文字体,ASCII字体对应为1206
	LCD_SetColor(142);						// 设置画笔
	LCD_DisplayText(28, 310,"1212中文字体：反客科技");	
	
	LCD_SetTextFont(&CH_Font16);			// 设置1616中文字体,ASCII字体对应为1608
	LCD_SetColor(135);						// 设置画笔
	LCD_DisplayText(28, 330,"1616中文字体：反客科技");		
	
	LCD_SetTextFont(&CH_Font20);			// 设置2020中文字体,ASCII字体对应为2010
	LCD_SetColor(154);						// 设置画笔
	LCD_DisplayText(28, 355,"2020中文字体：反客科技");		

	LCD_SetTextFont(&CH_Font24);			// 设置2424中文字体,ASCII字体对应为2412
	LCD_SetColor(171);						// 设置画笔
	LCD_DisplayText(28, 385,"2424中文字体：反客科技");		

	LCD_SetTextFont(&CH_Font32);			// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(189);						// 设置画笔
	LCD_DisplayText(28, 425,"3232中文字体：反客科技");		
	
	LCD_SetTextFont(&CH_Font32);			// 设置3232中文字体,ASCII字体对应为3216
	LCD_SetColor(183);
	LCD_DisplayText(40,500,   "STM32H750 LTDC 测试");	
	LCD_DisplayText(40,500+40,"核心板型号：FK750M3-VBT6");	
	LCD_DisplayText(40,500+80,"屏幕分辨率：800*480");	
	LCD_DisplayText(40,500+120,"颜色格式：L8");		
	
	HAL_Delay(2000);

	time = 80;
	
	LCD_SetBackColor(LCD_BLACK); //设置背景色
	LCD_Clear(); //清屏，刷背景色	
	
	LCD_SetColor(LCD_RED);    LCD_FillCircle(120,285,80);		// 填充圆
	LCD_SetColor(LCD_GREEN);  LCD_FillCircle(170,285,80); 
	LCD_SetColor(LCD_BLUE);   LCD_FillCircle(220,285,80);  	
	
   LCD_SetColor(LCD_RED);	
	LCD_DrawRect(5,35,350,150);  HAL_Delay(time);		// 画矩形
	LCD_DrawRect(30,50,300,120); HAL_Delay(time);
	LCD_DrawRect(55,65,250,90);  HAL_Delay(time);
	LCD_DrawRect(80,80,200,60);  HAL_Delay(time);
	
	LCD_SetColor(LIGHT_GREY);
	LCD_DrawLine(5,5,400,5);	HAL_Delay(time);	// 画线
	LCD_DrawLine(5,10,300,10);	HAL_Delay(time);
	LCD_DrawLine(5,15,200,15); HAL_Delay(time);
	LCD_DrawLine(5,20,100,20);	HAL_Delay(time);		
	
   
   LCD_SetColor(LCD_YELLOW);	LCD_DrawCircle(100,480,80);   HAL_Delay(time);	// 画圆
	LCD_SetColor(LCD_CYAN);		LCD_DrawCircle(150,480,80);   HAL_Delay(time);
   LCD_SetColor(LCD_MAGENTA); LCD_DrawCircle(200,480,80);   HAL_Delay(time);
   LCD_SetColor(LCD_RED);		LCD_DrawCircle(250,480,80);   HAL_Delay(time);

   LCD_SetColor(LIGHT_MAGENTA);	
	LCD_DrawEllipse(200,680,200,100); HAL_Delay(time);		// 画椭圆
	LCD_DrawEllipse(200,680,170,80);  HAL_Delay(time);
	LCD_DrawEllipse(200,680,140,60);  HAL_Delay(time);
	LCD_DrawEllipse(200,680,110,40);  HAL_Delay(time);
                                                        
	HAL_Delay(2000);	
	
	LCD_SetBackColor(LCD_BLACK); //设置背景色
	LCD_Clear(); //清屏，刷背景色

	LCD_SetFont(&Font16);
	LCD_SetColor(LCD_BLUE);	  	  LCD_FillRect(150,  3,250,16);	LCD_DisplayString(0,  3,"LCD_BLUE"); 			
	LCD_SetColor(LCD_GREEN);  	  LCD_FillRect(150, 25,250,16);  LCD_DisplayString(0, 25,"LCD_GREEN");	
	LCD_SetColor(LCD_RED);    	  LCD_FillRect(150, 47,250,16);  LCD_DisplayString(0, 47,"LCD_RED");	 	
	LCD_SetColor(LCD_CYAN);   	  LCD_FillRect(150, 69,250,16);  LCD_DisplayString(0, 69,"LCD_CYAN");		
	LCD_SetColor(LCD_MAGENTA);	  LCD_FillRect(150, 91,250,16);  LCD_DisplayString(0, 91,"LCD_MAGENTA");	
	LCD_SetColor(LCD_YELLOW); 	  LCD_FillRect(150,113,250,16);  LCD_DisplayString(0,113,"LCD_YELLOW");		
	LCD_SetColor(LCD_GREY);   	  LCD_FillRect(150,135,250,16);	LCD_DisplayString(0,135,"LCD_GREY");		
                                                                                          
	LCD_SetColor(LIGHT_BLUE);	  LCD_FillRect(150,157,250,16);  LCD_DisplayString(0,157,"LIGHT_BLUE");		
	LCD_SetColor(LIGHT_GREEN);   LCD_FillRect(150,179,250,16);  LCD_DisplayString(0,179,"LIGHT_GREEN");	
	LCD_SetColor(LIGHT_RED);     LCD_FillRect(150,201,250,16);  LCD_DisplayString(0,201,"LIGHT_RED");	   
	LCD_SetColor(LIGHT_CYAN);    LCD_FillRect(150,223,250,16);  LCD_DisplayString(0,223,"LIGHT_CYAN");	   
	LCD_SetColor(LIGHT_MAGENTA); LCD_FillRect(150,245,250,16);  LCD_DisplayString(0,245,"LIGHT_MAGENTA");	
	LCD_SetColor(LIGHT_YELLOW);  LCD_FillRect(150,267,250,16);  LCD_DisplayString(0,267,"LIGHT_YELLOW");	
	LCD_SetColor(LIGHT_GREY);    LCD_FillRect(150,289,250,16);	LCD_DisplayString(0,289,"LIGHT_GREY");  	
	                                                                                       
	LCD_SetColor(DARK_BLUE);	  LCD_FillRect(150,311,250,16);  LCD_DisplayString(0,311,"DARK_BLUE");		
	LCD_SetColor(DARK_GREEN);    LCD_FillRect(150,333,250,16);  LCD_DisplayString(0,333,"DARK_GREEN");		
	LCD_SetColor(DARK_RED);      LCD_FillRect(150,355,250,16);  LCD_DisplayString(0,355,"DARK_RED");		
	LCD_SetColor(DARK_CYAN);     LCD_FillRect(150,377,250,16);  LCD_DisplayString(0,377,"DARK_CYAN");		
	LCD_SetColor(DARK_MAGENTA);  LCD_FillRect(150,399,250,16);  LCD_DisplayString(0,399,"DARK_MAGENTA");	
	LCD_SetColor(DARK_YELLOW);   LCD_FillRect(150,421,250,16);  LCD_DisplayString(0,421,"DARK_YELLOW");	
	LCD_SetColor(DARK_GREY);     LCD_FillRect(150,443,250,16);	LCD_DisplayString(0,443,"DARK_GREY");	
	
	for(i=16;i<24;i++)  {LCD_SetColor(i);  LCD_FillRect(150+(i-16)*31,465,31,16);}  LCD_DisplayString(0,465,"Index:16~23");	
	for(i=32;i<40;i++)  {LCD_SetColor(i);  LCD_FillRect(150+(i-32)*31,487,31,16);}  LCD_DisplayString(0,487,"Index:32~39");		
	for(i=64;i<72;i++)  {LCD_SetColor(i);  LCD_FillRect(150+(i-64)*31,509,31,16);}  LCD_DisplayString(0,509,"Index:64~71");		
	for(i=80;i<88;i++)  {LCD_SetColor(i);  LCD_FillRect(150+(i-80)*31,531,31,16);}  LCD_DisplayString(0,531,"Index:80~87");		
	for(i=96;i<104;i++) {LCD_SetColor(i);  LCD_FillRect(150+(i-96)*31,553,31,16);}  LCD_DisplayString(0,553,"Index:96~104");		
	for(i=112;i<120;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-112)*31,578,31,16);} LCD_DisplayString(0,578,"Index:112~119");		
	for(i=128;i<136;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-128)*31,600,31,16);} LCD_DisplayString(0,600,"Index:128~135");		
	for(i=144;i<152;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-144)*31,622,31,16);} LCD_DisplayString(0,622,"Index:144~151");		
	for(i=160;i<168;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-160)*31,644,31,16);} LCD_DisplayString(0,644,"Index:160~167");		
	for(i=176;i<184;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-176)*31,666,31,16);} LCD_DisplayString(0,666,"Index:176~183");		
	for(i=192;i<200;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-192)*31,688,31,16);} LCD_DisplayString(0,688,"Index:192~199");		
	for(i=208;i<216;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-208)*31,710,31,16);} LCD_DisplayString(0,710,"Index:208~215");		
	for(i=224;i<232;i++){LCD_SetColor(i);  LCD_FillRect(150+(i-224)*31,732,31,16);} LCD_DisplayString(0,732,"Index:224~231");		

	HAL_Delay(3000);		
	LCD_DisplayDirection(Direction_H); // 切换回横屏显示
}







