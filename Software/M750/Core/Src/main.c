/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
  /***********When modifying the CUBEMX configuration, ************************/
  /***********remember to modify the following code****************************/
  /***********1.Comment out the line before USER CODE *************************/
  /****************MX_LTDC_Init();*********************************************/
  /****************MX_DMA2D_Init();********************************************/
  /****************MX_SDMMC1_SD_Init();****************************************/
  
  /****************************************************************************/
  /***********2.delete main.c ???????????????????? function********************/
  
  /*Notice!!!Fatfs not in cubemx config*/
  
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma2d.h"
#include "ltdc.h"
#include "memorymap.h"
#include "rtc.h"
#include "sdmmc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SysConfig.h"  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "DataProc.h"
#if 1
	

	#include "led.h"
	#include "key.h"
	#include "myiic.h"
	#include "sgp30.h"
	#include "gsm.h"
//	#include "mqtt.h"
	#include "esp8266.h"
	#include "MqttKit.h"
	#include "onenet.h"
	#include "lcd_rgb.h"
	#include "lcd_pwm.h"
	#include "touch_800x480.h"
	

	#include "lvgl.h"
	#include "lv_port_disp_template.h"
	#include "lv_port_indev_template.h"
//	#include "lv_demo_benchmark.h"
	#include "lv_gui.h"

	#include "sdmmc_sd.h"//不使用cubemx生成的sdmmc.c
	#include "ff.h"
	#include "ff_gen_drv.h"
	#include "sd_diskio.h"
//Commom 
	#include "file_ops.h"
	
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t Uart_Buffer_1[1];
uint8_t Uart_Buffer_3[1];
extern uint8_t UART5_temp[REC_LENGTH];
//uint8_t Uart_Buffer_4[1];
uint8_t Uart_Buffer_7[1];
uint8_t Uart_Buffer_8[1];

GPS_Data gps;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MPU_Config(void);
void FatFs_Check(void);				// 判断FatFs是否挂载成功，若没有创建FatFs则格式化SD卡

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void print_system_config(const SystemConfig *config) {
    printf("System Configuration:\r\n");
    printf("WiFi SSID: %s\r\n", config->wifi_ssid);
    printf("WiFi Password: %s\r\n", config->wifi_password);
    printf("Phone Number: %s\r\n", config->phone_number);
    printf("Screen Brightness: %d\r\n", config->screen_brightness);
    printf("Volume: %d\r\n", config->volume);
    printf("Latitude: %.6f\r\n", config->latitude);
    printf("Longitude: %.6f\r\n", config->longitude);
}
void Calculate_UID_Checksum(void)
{
    uint32_t uid0 = READ_REG(*((uint32_t*)0x1FF1E800)); // UID[31:0]
    uint32_t uid1 = READ_REG(*((uint32_t*)0x1FF1E804)); // UID[63:32]
    uint32_t uid2 = READ_REG(*((uint32_t*)0x1FF1E808)); // UID[95:64]

    // 计算简单的 XOR 校验和
    uint32_t checksum = uid0 ^ uid1 ^ uid2;
//    printf("%x %x\r\n",checksum,HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR16));//33a6661
    if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR16)==checksum)
      return;
    else
    {
      lv_gui_activic_system(uid0, uid1, uid2);

      while(1)
      {
		lv_task_handler();	// LVGL进程
		Touch_Scan();			// 触摸扫描
		LED1_Toggle;			// LED指示
        HAL_Delay(10);
      }
    }
    

}
void net_init(void)
{

	if(ESP8266_Init()==1){
    printf("esp ok\r\n");
    for(int i=0,maxtry=3;i<maxtry;i++){
      if(!OneNet_DevLink()){
        sysData.network_status=1;
        break;
      }
      delay_ms(500);


    }

    if(sysData.network_status)printf("mqtt ok\r\n");
    else printf("mqtt false\r\n");
    
	const char *topics[] = {"TX"};

    OneNet_Subscribe(topics, 1);	
    // delay_ms(100);
    // OneNet_Publish("RX", "get/ts");
  } 
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	MPU_Config();
	SCB_EnableICache();		// 使能ICache
	SCB_EnableDCache();		// 使能DCache

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  LtdcClock_Config();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
//  MX_LTDC_Init();
//  MX_DMA2D_Init();
  MX_TIM4_Init();
  MX_UART7_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
//  MX_SDMMC1_SD_Init();
  MX_UART8_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim4);

  HAL_UART_Receive_IT(&huart1, (uint8_t *)Uart_Buffer_1, 1);//USB
//  HAL_UART_Receive_IT(&huart7, (uint8_t *)Uart_Buffer_7, 1);//摄像头
	HAL_UART_Receive_IT(&huart3, (uint8_t *)Uart_Buffer_3, 1);//wifi模块
	HAL_UART_Receive_IT(&huart4, (uint8_t *)UART4_temp, 1);//GPS
	HAL_UART_Receive_IT(&huart8, (uint8_t *)Uart_Buffer_8, 1);//GMS
	FatFs_Check();//sd卡初始化
	DataProc_Init();//系统配置加载，从sd卡
	LED_Init();					// 初始化LED引脚
	KEY_Init();					// 初始化按键引脚
	IIC_Init();//SHT40温湿度监测模块
	sgp30_init();//sgp30空气质量模块
	MX_LTDC_Init();			// LTDC以及层初始化 
//	LCD_PWMinit(100);			// 背光引脚PWM初始化，占空比40%
	Touch_Init();				// 触摸屏初始化		

	
	lv_init();					//	LVGL初始化
	lv_port_disp_init();		//	LVGL显示接口初始化
	lv_port_indev_init();	// LVGL触摸接口初始化				
	
//	lv_demo_benchmark();    // 运行官方例程
	lv_gui_start();
	Calculate_UID_Checksum();
//	DP_UpdateWiFiConfig("PDCN_6C74","1234567890");
//	DP_UpdateWiFiConfig("k40","88888888");
//	DP_UpdateLocation(25.036921,118.481931);
//	DP_UpdatePhoneNumber("17758751170");
//	DP_UpdateVolume(6);
//	
	DP_ConfigSave();//保存系统配置
	file_cat("sysConfig.json");
//	file_list("/");
//	file_remove("sysConfig.json");
  unsigned char *dataPtr = NULL;
  net_init();
	sysData.init_status=true;
	printf("\r\ninit done, use %d ms.\r\n",HAL_GetTick());
		lv_gui();//创建lvgl界面

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
uint32_t last_touch_time = 0; // 记录最后一次触摸时间
#define SCREEN_TIMEOUT 30000   // 30秒，单位 ms
  while (1)
  {
//		MeasureLoopFrequency();

        if (touchInfo.flag) {  // 检测是否触摸
            last_touch_time = HAL_GetTick(); // 更新触摸时间
            LCD_PWMinit(100);
        }

        // 判断是否超过30秒未触摸
        if ((HAL_GetTick() - last_touch_time) > SCREEN_TIMEOUT) {
            LCD_PWMinit(1);
        }
		lv_task_handler();	// LVGL进程
		Touch_Scan();			// 触摸扫描
		LED1_Toggle;			// LED指示
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		HAL_Delay(10);			// GT911触摸屏扫描间隔不能小于10ms
  // check_and_print_gps_data(&gps);
	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 48;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//	MPU配置 
//
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  HAL_MPU_Disable(); // 设置之前先禁止MPU

  MPU_InitStruct.Enable 				= MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress 			= LCD_MemoryAdd;
  MPU_InitStruct.Size 					= MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission 	= MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable 		= MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable 			= MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable 			= MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number 				= MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField 		= MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable 	= 0x00;
  MPU_InitStruct.DisableExec 			= MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);	// 使能MCU
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
// void print_gps_data(const GPS_Data *gps) {
// 	static uint16_t Usefulcount=0;
// 	if(gps->UsefulFlag=='V')
// 	{
// 		printf("wait gps %s...\r\n",gps->UTCTime);
// 		return;
// 	}
// 	else if(gps->UsefulFlag=='A')
// 		printf("gps OK\r\n");
		
// 	Usefulcount++;
// 	if(Usefulcount==30)
// 	{
// 		DP_UpdateLocation(gps->latitude_d,gps->longitude_d);
// 		DP_ConfigSave();
// 	}
//     printf("UTC Time: %s\r\n", gps->UTCTime);
//     printf("UTC Time (int): %u\r\n", gps->UTCTime_int);
//     printf("Useful Flag: %u\r\n", gps->UsefulFlag);
//     printf("Latitude: %s %c\r\n", gps->latitude, gps->N_S);
//     printf("Latitude (float): %.6f\r\n", gps->latitude_d);
//     printf("Longitude: %s %c\r\n", gps->longitude, gps->E_W);
//     printf("Longitude (float): %.6f\r\n", gps->longitude_d);
//     printf("Speed: %s\r\n", gps->speed);
//     printf("Speed (float): %.2f\r\n", gps->speed_d);
// }
// void check_and_print_gps_data(const GPS_Data *gps) {
//     // 静态变量保存上一次的 UTCTime
//     static uint8_t last_UTCTime[15] = {0};

//     // 检查 UTCTime 是否变化
//     if (strncmp((const char *)last_UTCTime, (const char *)gps->UTCTime, sizeof(gps->UTCTime)) != 0) {
//         print_gps_data(gps); // 打印 GPS 数据
//         strncpy((char *)last_UTCTime, (const char *)gps->UTCTime, sizeof(gps->UTCTime)); // 更新 last_UTCTime
//     }
// }
