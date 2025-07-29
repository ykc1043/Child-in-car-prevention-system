#include "stm32h7xx_hal.h"
#include "SysConfig.h"  
#include "stdio.h"
#define LCD_MemoryAdd   	0x24000000 		//	�Դ����ʼ��ַ  
/**
  * @brief �رղ���������ж�
  * @param None
  * @retval None
*/
void InitNVIC(void)
{
    uint32_t i = 0;
    __set_PRIMASK(1); // �ر�ȫ���ж�
 
    /* �رյδ�ʱ������λ��Ĭ��ֵ */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
 
    /* �ر������жϣ���������жϹ����־ */
    for(i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
 
    /* �����ж�������ƫ���� */
    SCB->VTOR = 0x90000000|0x00000000;
    
 
    __set_PRIMASK(0); // ��ȫ���ж�
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
static void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

/****************************************************************************************************/
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 480000000 (CPU Clock)
  *            HCLK(Hz)                       = 240000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  120MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  120MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  120MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  120MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 192
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
/****************************************************************************************************/  
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config1(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
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
  

//  /* ����LTDCʱ�ӣ���������Ϊ33MHz����ˢ������60֡���ң����߻��߹��Ͷ��������˸ */
//  /* LCD clock configuration */
//  /* PLL3_VCO Input = HSE_VALUE/PLL3M = 1 Mhz */
//  /* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 330 Mhz */
//  /* PLLLCDCLK = PLL3_VCO Output/PLL3R = 330/10 = 33 Mhz */
//  /* LTDC clock frequency = PLLLCDCLK = 33 Mhz */  
//  
//  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_USART1;
//  PeriphClkInitStruct.PLL3.PLL3M = 25;
//  PeriphClkInitStruct.PLL3.PLL3N = 330;
//  PeriphClkInitStruct.PLL3.PLL3P = 2;
//  PeriphClkInitStruct.PLL3.PLL3Q = 2;
//  PeriphClkInitStruct.PLL3.PLL3R = 10;
//  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
//  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;
//  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
//  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
}

void LtdcClock_Config(void)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* ����LTDCʱ�ӣ���������Ϊ33MHz����ˢ������60֡���ң����߻��߹��Ͷ��������˸ */
  /* LCD clock configuration */
  /* PLL3_VCO Input = HSE_VALUE/PLL3M = 1 Mhz */
  /* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 330 Mhz */
  /* PLLLCDCLK = PLL3_VCO Output/PLL3R = 330/10 = 33 Mhz */
  /* LTDC clock frequency = PLLLCDCLK = 33 Mhz */  
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_USART1;
  PeriphClkInitStruct.PLL3.PLL3M = 25;
  PeriphClkInitStruct.PLL3.PLL3N = 330;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 10;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

void MeasureLoopFrequency(void)
{
    static uint32_t loop_count = 0;        // ��¼ѭ������
    static uint32_t last_tick = 0;        // �ϴβ�����ʱ���

    loop_count++;                         // ����ѭ������
    uint32_t current_tick = HAL_GetTick(); // ��ȡ��ǰʱ�䣨��λ��ms��
    // ����Ƿ��ѹ�1��
    if ((current_tick - last_tick) >= 1000)
    {
        uint32_t frequency = loop_count;  // ����Ƶ��
        loop_count = 0;                   // ���¼���
        last_tick = current_tick;         // �����ϴβ���ʱ��
	printf("%d ",current_tick);

        // ��ӡƵ�ʣ����� USART1 �ѳ�ʼ����
        char message[50];
        snprintf(message, sizeof(message), "Loop Frequency: %lu Hz\r\n", frequency);
        
		printf("%s",message);
    }
}

