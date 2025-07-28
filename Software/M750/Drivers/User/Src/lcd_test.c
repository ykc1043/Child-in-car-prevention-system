/***
	***************************************************************************************************************************
	*	@file  	lcd_test.c
	*	@version V1.0
	*  @date    2020-9-14
	*	@author  ���ͿƼ�
	*	@brief   LTDC���Ժ���	
   ***************************************************************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨��1. ����STM32H750VBT6���İ� ���ͺţ�FK750M3-VBT6�� + 4.3��RGBҺ����(��Ļ�ͺ�RGB043M2-800*480)
	*				 2. ����STM32H750VBT6���İ� ���ͺţ�FK750M3-VBT6�� + 5.0��RGBҺ����(��Ļ�ͺ�RGB050M1-800*480)
	*				 3. ����STM32H750VBT6���İ� ���ͺţ�FK750M3-VBT6�� + 7.0��RGBҺ����(��Ļ�ͺ�RGB070M1-800*480)
	*			
	*	�Ա���ַ��https://shop212360197.taobao.com
	*	QQ����Ⱥ��536665479
	*
>>>>> �ļ�˵����
	*
	*	1. �����ڲ��ԣ����ļ����Ǳ��룬�û���ֲ��ʱ���������
	*
	****************************************************************************************************************************
***/


#include "lcd_test.h"



/*************************************************************************************************
*	�� �� ��:	LCD_Test_Clear
*
*	��������:	��������
*
*	˵    ��:	��	
*************************************************************************************************/

void LCD_Test_Clear(void)
{
	uint16_t time = 1000;	// ��ʱʱ��
	uint8_t	i = 0;			// ��������
	
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>
	
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"ˢ������");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }	
	
// ˢ������>>>>>
		
	LCD_SetTextFont(&CH_Font32);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_BLACK);				// ���û�����ɫ

	for(i=0;i<8;i++)
	{
		switch (i)		// �л�����ɫ
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
		LCD_Clear();		// ����
		LCD_DisplayText(112, 84,"STM32H750 LTDC ˢ������");
		LCD_DisplayText(112,134,"���İ��ͺţ�FK750M3-VBT6");
		LCD_DisplayText(112,184,"��Ļ�ֱ��ʣ�800*480");	
		LCD_DisplayText(112,234,"��ɫ��ʽ��L8");	
		HAL_Delay(time);	// ��ʱ
	}
}

/*************************************************************************************************
*	�� �� ��:	LCD_Test_Text
*
*	��������:	�ı���ʾ����
*
*	˵    ��:	��	
*************************************************************************************************/

void LCD_Test_Text(void)
{
	uint16_t i;					// ��������
	uint16_t time = 80;	// ��ʱʱ��
	
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>
	
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"�ı���ʾ");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }	
	
// ��ʾ�ı����������������С�����ĺ�ASCII�ַ� >>>>>

	LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	
	LCD_SetColor(LCD_WHITE);	// ���û��ʣ���ɫ
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

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_YELLOW);				// ���û��ʣ���ɫ
	LCD_DisplayText(0, 230,"�ı���ʾ������ʾ���ĺ�ASCII�ַ���");
	LCD_DisplayText(0, 260,"�û��ɸ������󣬶��ֿ���������ɾ��");	

	
	LCD_SetTextFont(&CH_Font12);			// ����1212��������,ASCII�����ӦΪ1206
	LCD_SetColor(142);						// ���û���
	LCD_DisplayText(28, 290,"1212�������壺���ͿƼ�");	
	
	LCD_SetTextFont(&CH_Font16);			// ����1616��������,ASCII�����ӦΪ1608
	LCD_SetColor(135);						// ���û���
	LCD_DisplayText(28, 310,"1616�������壺���ͿƼ�");		
	
	LCD_SetTextFont(&CH_Font20);			// ����2020��������,ASCII�����ӦΪ2010
	LCD_SetColor(154);						// ���û���
	LCD_DisplayText(28, 335,"2020�������壺���ͿƼ�");		

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(171);						// ���û���
	LCD_DisplayText(28, 365,"2424�������壺���ͿƼ�");		

	LCD_SetTextFont(&CH_Font32);			// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(189);						// ���û���
	LCD_DisplayText(28, 405,"3232�������壺���ͿƼ�");		

	HAL_Delay(3000);	// ��ʱ�ȴ�
}

/*************************************************************************************************
*	�� �� ��:	LCD_Test_Variable
*
*	��������:	������ʾ������������С��
*
*	˵    ��:	��	
*************************************************************************************************/

void LCD_Test_Variable (void)
{

	uint16_t i;					// ��������

	int32_t	a = 0;			// �����������������ڲ���
	int32_t	b = 0;			// �����������������ڲ���
	int32_t	c = 0;			// �����������������ڲ���

	double p = 3.1415926;	// ���帡�������������ڲ���
	double f = -1234.1234;	// ���帡�������������ڲ���
	
	
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>	
		
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"������ʾ");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }		
	
// ��ʾ����������������С��>>>>>	
	
	LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	
	LCD_SetTextFont(&CH_Font32);					// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_WHITE);						// ���û���,��ɫ
	
	LCD_DisplayText(28, 20,"����λ���ո�");	// ��ʾ�ı�	
	LCD_DisplayText(400,20,"����λ���0");		// ��ʾ�ı�		
	
	LCD_SetColor(LIGHT_CYAN);					// ���û��ʣ�����ɫ
	LCD_DisplayText(28, 60,"��������");				
	LCD_DisplayText(28,100,"��������");				
	LCD_DisplayText(28,140,"��������");					
				
	LCD_SetColor(LIGHT_YELLOW);				// ���û��ʣ�����ɫ		
	LCD_DisplayText(400, 60,"��������");	
	LCD_DisplayText(400,100,"��������");	
	LCD_DisplayText(400,140,"��������");	
			
	LCD_SetColor(LIGHT_RED);					// ���û���	������ɫ		
	LCD_DisplayText(28, 200,"��С����");	
	LCD_DisplayText(28, 240,"��С����");			
			
	for(i=0;i<100;i++)
   {
		LCD_SetColor(LIGHT_CYAN);									// ���û���	������ɫ	
		LCD_ShowNumMode(Fill_Space);								// ����λ���ո�
		LCD_DisplayNumber( 160, 60, a+i*125, 8) ;				// ��ʾ����		
		LCD_DisplayNumber( 160,100, b+i, 	 6) ;				// ��ʾ����			
		LCD_DisplayNumber( 160,140, c-i,     6) ;				// ��ʾ����			
		
		LCD_SetColor(LIGHT_YELLOW);								// ���û��ʣ�����ɫ	
		LCD_ShowNumMode(Fill_Zero);								// ����λ���0
		LCD_DisplayNumber( 560, 60, a+i*125, 8) ;				// ��ʾ����		
		LCD_DisplayNumber( 560,100, b+i, 	 6) ;				// ��ʾ����			
		LCD_DisplayNumber( 560,140, c-i,     6) ;				// ��ʾ����				
		
		LCD_SetColor(LIGHT_RED);									// ���û��ʣ�����ɫ			
		LCD_ShowNumMode(Fill_Space);								// ����λ���ո�		
		LCD_DisplayDecimals( 160, 200, p+i*0.1,  6,3);		// ��ʾС��	
		LCD_DisplayDecimals( 160, 240, f+i*0.01, 11,4);		// ��ʾС��		
		
		HAL_Delay(30);				
   }
	
	HAL_Delay(2500);		
	
}



/*************************************************************************************************
*	�� �� ��:	LCD_Test_FillRect
*
*	��������:	����������
*
*	˵    ��:	��	
*************************************************************************************************/

void LCD_Test_FillRect(void)
{
	uint16_t i;					// ��������
	
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>

	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"���λ���");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }			
	
// �������>>>>>	
	
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ

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
*	�� �� ��:	LCD_Test_Color
*
*	��������:	��ɫ����
*
*	˵    ��:	����ɫ��256����ɫ��ʾ����
*************************************************************************************************/

void LCD_Test_Color(void)
{
	uint16_t i;
	uint8_t x,y,color=0;
	
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>
	
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"��ɫ����");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }		
	
// ��ɫ����>>>>>
		
	LCD_SetTextFont(&CH_Font32);			// ����2424��������,ASCII�����ӦΪ2412

	for( i=0; i<2; i++)
   {
		LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
		LCD_Clear(); //������ˢ����ɫ		
		LCD_SetColor(LCD_WHITE);				// ���û�����ɫ
		LCD_DisplayText(221,420,"L8��ɫ��ʽ���ܼ�256����ɫ");	
		
		for( y=0; y<8; y++)
		{
			for( x=0; x<16; x++)
			{
				LCD_SetColor(color);		// ������ɫ
				LCD_FillRect( 5+(x*50), 6+(y*50), 40, 40);	// ������
				color++;
			}
		}
		HAL_Delay(3000);	
	}

	
}

/*************************************************************************************************
*	�� �� ��:	LCD_Test_GrahicTest
*
*	��������:	2Dͼ�λ���
*
*	˵    ��:	��	
*************************************************************************************************/

void LCD_Test_GrahicTest(void)
{
	uint16_t time = 80;		// ��ʱʱ��
	uint16_t i;					// ��������
		
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>
	
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"��ͼ����");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }					
	
// 2Dͼ�λ���>>>>>>>
	
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ	

	LCD_SetColor(LCD_RED);    LCD_FillCircle(120,350,80);		//���Բ��
	LCD_SetColor(LCD_GREEN);  LCD_FillCircle(170,350,80); 	
	LCD_SetColor(LCD_BLUE);   LCD_FillCircle(220,350,80);  	
	
	LCD_SetColor(LIGHT_GREY);
	LCD_DrawLine(5,5,400,5);	HAL_Delay(time);			//��ֱ��
	LCD_DrawLine(5,10,300,10);	HAL_Delay(time);
	LCD_DrawLine(5,15,200,15); HAL_Delay(time);
	LCD_DrawLine(5,20,100,20);	HAL_Delay(time);	

	LCD_SetColor(LIGHT_CYAN);
	LCD_DrawCircle(600,120,100);	HAL_Delay(time);		//����Բ��
	LCD_DrawCircle(600,120,80);   HAL_Delay(time);
	LCD_DrawCircle(600,120,60);   HAL_Delay(time);
	LCD_DrawCircle(600,120,40);   HAL_Delay(time);
	
	LCD_SetColor(LCD_RED);	
	LCD_DrawRect(5,35,400,150);  HAL_Delay(time);			//���ƾ���
	LCD_DrawRect(30,50,350,120); HAL_Delay(time);
	LCD_DrawRect(55,65,300,90);  HAL_Delay(time);
	LCD_DrawRect(80,80,250,60);  HAL_Delay(time);

	LCD_SetColor(LIGHT_MAGENTA);	
	LCD_DrawEllipse(590,350,200,100); HAL_Delay(time);	//������Բ
	LCD_DrawEllipse(590,350,170,80);  HAL_Delay(time);
	LCD_DrawEllipse(590,350,140,60);  HAL_Delay(time);
	LCD_DrawEllipse(590,350,110,40);  HAL_Delay(time);

	HAL_Delay(2000);	
}
/*************************************************************************************************
*	�� �� ��:	LCD_Test_Image
*
*	��������:	ͼƬ��ʾ����
*
*	˵    ��:	��	
*************************************************************************************************/

void LCD_Test_Image(void)
{
	uint16_t i;					// ��������
		
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>
	
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(334, 160,"ͼƬ����");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage(  280, 218, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<150;i++)
   {
		LCD_FillRect(100,330,4*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }		
	
	
// ͼƬ����>>>>>>>	

	LCD_SetBackColor(LCD_BLACK); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	
	LCD_SetColor( 72);
	LCD_DrawImage( 185, 116, 83, 83, Image_Android_83x83) ;	// ��ʾͼƬ
	
	LCD_SetColor( 183);
	LCD_DrawImage( 359, 124, 83, 83, Image_Cloud_83x83) ;		// ��ʾͼƬ
	
	LCD_SetColor( 142);
	LCD_DrawImage( 533, 124, 83, 83, Image_Folder_83x83) ;	// ��ʾͼƬ

	LCD_SetColor( 76);
	LCD_DrawImage( 185, 270, 83, 83, Image_Message_83x83) ;	// ��ʾͼƬ
	
	LCD_SetColor( 184);
	LCD_DrawImage( 359, 270, 83, 83, Image_Toys_83x83) ;		// ��ʾͼƬ
	
	LCD_SetColor( 239);
	LCD_DrawImage( 533, 270, 83, 83, Image_Video_83x83) ;		// ��ʾͼƬ

	HAL_Delay(2000);
	
	LCD_SetBackColor(LCD_WHITE); 			//	���ñ���ɫ
	LCD_Clear(); 								// ����
	LCD_SetColor( LCD_BLACK);				// ���û���
	LCD_DrawImage( 159, 120, 480, 239, Image_FANKE_480x239) ;	// ��ʾͼƬ
	HAL_Delay(2000);
	
}


/*************************************************************************************************
*	�� �� ��:	LCD_Test_Vertical
*
*	��������:	��������
*
*	˵    ��:	��	
*************************************************************************************************/

void  LCD_Test_Vertical(void)
{
	uint16_t i;
	uint16_t time = 100;	
	
	LCD_DisplayDirection(Direction_V); // �л���������ʾ
	
// ���Ƴ�ʼ���棬�������⡢LOGO�Լ�������>>>>>
	
	LCD_SetBackColor(142); 			//	���ñ���ɫ
	LCD_Clear(); 						//	������ˢ����ɫ
	
	LCD_SetTextFont(&CH_Font32);				// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(LCD_BLACK);					//	���û���ɫ
	LCD_DisplayText(180, 260,"������ʾ");	// ��ʾ�ı�
	
	LCD_SetColor(221);					//	���û���ɫ
	LCD_DrawImage( 120, 320, 240, 83, Image_FANKE_240x83) ;		// ��ʾLOGOͼƬ

	LCD_SetColor(LIGHT_YELLOW);		//	���û���ɫ
	for(i=0;i<130;i++)
   {
		LCD_FillRect(45,450,3*i,6);	// ���ƾ��Σ�ʵ�ּ��׽�������Ч��
		HAL_Delay(10);	
   }

	
// ������������>>>>>>>		
	
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ		

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

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(LCD_YELLOW);				// ���û��ʣ���ɫ
	LCD_DisplayText(0, 230,"�ı���ʾ������ʾ���ĺ�ASCII�ַ���");
	LCD_DisplayText(0, 260,"�û��ɸ������󣬶��ֿ���������ɾ��");	

	
	LCD_SetTextFont(&CH_Font12);			// ����1212��������,ASCII�����ӦΪ1206
	LCD_SetColor(142);						// ���û���
	LCD_DisplayText(28, 310,"1212�������壺���ͿƼ�");	
	
	LCD_SetTextFont(&CH_Font16);			// ����1616��������,ASCII�����ӦΪ1608
	LCD_SetColor(135);						// ���û���
	LCD_DisplayText(28, 330,"1616�������壺���ͿƼ�");		
	
	LCD_SetTextFont(&CH_Font20);			// ����2020��������,ASCII�����ӦΪ2010
	LCD_SetColor(154);						// ���û���
	LCD_DisplayText(28, 355,"2020�������壺���ͿƼ�");		

	LCD_SetTextFont(&CH_Font24);			// ����2424��������,ASCII�����ӦΪ2412
	LCD_SetColor(171);						// ���û���
	LCD_DisplayText(28, 385,"2424�������壺���ͿƼ�");		

	LCD_SetTextFont(&CH_Font32);			// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(189);						// ���û���
	LCD_DisplayText(28, 425,"3232�������壺���ͿƼ�");		
	
	LCD_SetTextFont(&CH_Font32);			// ����3232��������,ASCII�����ӦΪ3216
	LCD_SetColor(183);
	LCD_DisplayText(40,500,   "STM32H750 LTDC ����");	
	LCD_DisplayText(40,500+40,"���İ��ͺţ�FK750M3-VBT6");	
	LCD_DisplayText(40,500+80,"��Ļ�ֱ��ʣ�800*480");	
	LCD_DisplayText(40,500+120,"��ɫ��ʽ��L8");		
	
	HAL_Delay(2000);

	time = 80;
	
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ	
	
	LCD_SetColor(LCD_RED);    LCD_FillCircle(120,285,80);		// ���Բ
	LCD_SetColor(LCD_GREEN);  LCD_FillCircle(170,285,80); 
	LCD_SetColor(LCD_BLUE);   LCD_FillCircle(220,285,80);  	
	
   LCD_SetColor(LCD_RED);	
	LCD_DrawRect(5,35,350,150);  HAL_Delay(time);		// ������
	LCD_DrawRect(30,50,300,120); HAL_Delay(time);
	LCD_DrawRect(55,65,250,90);  HAL_Delay(time);
	LCD_DrawRect(80,80,200,60);  HAL_Delay(time);
	
	LCD_SetColor(LIGHT_GREY);
	LCD_DrawLine(5,5,400,5);	HAL_Delay(time);	// ����
	LCD_DrawLine(5,10,300,10);	HAL_Delay(time);
	LCD_DrawLine(5,15,200,15); HAL_Delay(time);
	LCD_DrawLine(5,20,100,20);	HAL_Delay(time);		
	
   
   LCD_SetColor(LCD_YELLOW);	LCD_DrawCircle(100,480,80);   HAL_Delay(time);	// ��Բ
	LCD_SetColor(LCD_CYAN);		LCD_DrawCircle(150,480,80);   HAL_Delay(time);
   LCD_SetColor(LCD_MAGENTA); LCD_DrawCircle(200,480,80);   HAL_Delay(time);
   LCD_SetColor(LCD_RED);		LCD_DrawCircle(250,480,80);   HAL_Delay(time);

   LCD_SetColor(LIGHT_MAGENTA);	
	LCD_DrawEllipse(200,680,200,100); HAL_Delay(time);		// ����Բ
	LCD_DrawEllipse(200,680,170,80);  HAL_Delay(time);
	LCD_DrawEllipse(200,680,140,60);  HAL_Delay(time);
	LCD_DrawEllipse(200,680,110,40);  HAL_Delay(time);
                                                        
	HAL_Delay(2000);	
	
	LCD_SetBackColor(LCD_BLACK); //���ñ���ɫ
	LCD_Clear(); //������ˢ����ɫ

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
	LCD_DisplayDirection(Direction_H); // �л��غ�����ʾ
}







