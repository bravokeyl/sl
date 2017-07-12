
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "metroTask.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
uint32_t uwPrescalerValue = 8;

TIM_HandleTypeDef htim2;
#ifdef UART_XFER_STPM3X /* UART MODE */
UART_HandleTypeDef huart6;
#endif

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void Timer2_Init();
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

#ifdef UART_XFER_STPM3X /* UART MODE */
static void MX_USART6_UART_Init(void);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
uint32_t tick=0;
int main(void)
{
  FRESULT res;                                          /* FatFs function common result code */
  uint32_t byteswritten, bytesread;                     /* File write/read counts */
  
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  //HAL_Init();
  
  /* Configure the system clock*/
  //SystemClock_Config();
  
  /* Configure LED3 and LED6 */
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED6);
  
  MX_GPIO_Init();
  MX_TIM2_Init();

#ifdef UART_XFER_STPM3X /* UART MODE */
  MX_USART6_UART_Init();
#endif

  /* Init timer for application measure */
    Timer2_Init();

  /* Init STPM with complete sequence and set the registers*/
  METRO_Init();
  /* Start Timer Channel1 in interrupt mode*/
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    // Starting Error
    Error_Handler();
  }

  /*##-1- Link the micro SD disk I/O driver ##################################*/
  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    /*##-2- Register the file system object to the FatFs module ##############*/
    if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
    {
      /* FatFs Initialization Error */
      Error_Handler();
    }
    else
    {
        FRESULT fr;
        FILINFO fno;
        uint8_t Header=1;
        char buf[256];

        fr = f_stat("log.csv", &fno);

        if(fr == FR_OK)//file exists
        {
          Header=0;
        }

        //open the file if it exists otherwise create a new one. with read write access
        if(f_open(&MyFile, "log.csv", FA_OPEN_ALWAYS | FA_WRITE | FA_READ) != FR_OK)
        {
          /* 'STM32.TXT' file Open for write Error */
          Error_Handler();
        }
        else
        {
          if(Header==1)//add the header line
          {
            sprintf(buf,"Time,channel,V RMS,C RMS,V MOM,C MOM,V FUND,C FUND,V PERIOD,"
                "C PHASE SH,V SAG TIME,V SWELL TIME,C SWELL TIME,EN_ACT,EN_REACT,"
                "EN_APP,POW_ACT,POW_REACT,POW_APP,AHACC\n");

            res = f_write(&MyFile, buf, strlen(buf), (void *)&byteswritten);
          }

          //move to end of file
          f_lseek(&MyFile,f_size(&MyFile));

          while (1)
          {
            uint32_t RMS_V1=0,RMS_V2=0;
            uint32_t RMS_I1=0,RMS_I2=0;
            int32_t MOM_V1=0,MOM_V2=0;
            int32_t MOM_I1=0,MOM_I2=0;
            int32_t FUND_V1=0,FUND_V2=0;
            int32_t FUND_I1=0,FUND_I2=0;
            int32_t Period1=0, Period2=0;
            int32_t Phase1=0, Phase2=0;
            int32_t SAG1=0,SAG2=0;
            int32_t VSWELL1=0,VSWELL2=0;
            int32_t CSWELL1=0,CSWELL2=0;
            int32_t POW_ACT1,POW_REACT1,POW_APP1;
            int32_t POW_ACT2,POW_REACT2,POW_APP2;
            int32_t EN_ACT1,EN_REACT1,EN_APP1;
            int32_t EN_ACT2,EN_REACT2,EN_APP2;
            int32_t AHACC1,AHACC2;
            /**************** Measure application ************************/
            if (metroData.metroInactiveTime >= METRO_TIMER)
            {

              metroData.metroInactiveTime = 0;
              HAL_GPIO_TogglePin(LED2_GPIO_type, LED2_GPIO_pin);
              METRO_Latch_Measures();
              HAL_Delay(20);
              METRO_Get_Measures();
              METRO_UpdateData();

              Metro_Read_RMS(1,&RMS_V1,&RMS_I1,1);
              Metro_Read_RMS(2,&RMS_V2,&RMS_I2,1);
              MOM_V1=Metro_Read_Momentary_Voltage(1,1);
              MOM_V2=Metro_Read_Momentary_Voltage(2,1);
              MOM_I1=Metro_Read_Momentary_Current(1,1);
              MOM_I2=Metro_Read_Momentary_Current(2,1);
              FUND_V1=Metro_Read_Momentary_Voltage(1,2);
              FUND_V2=Metro_Read_Momentary_Voltage(2,2);
              FUND_I1=Metro_Read_Momentary_Current(1,2);
              FUND_I2=Metro_Read_Momentary_Current(2,2);
              Phase1=Metro_Read_PHI(1);
              Phase2=Metro_Read_PHI(2);
              Period1=Metro_Read_Period(1);
              Period2=Metro_Read_Period(2);
              SAG1=Metro_Read_SAG_Time(1);
              SAG2=Metro_Read_SAG_Time(2);
              VSWELL1=Metro_Read_V_SWELL_Time(1);
              VSWELL2=Metro_Read_V_SWELL_Time(2);
              CSWELL1=Metro_Read_C_SWELL_Time(1);
              CSWELL2=Metro_Read_C_SWELL_Time(2);
              EN_ACT1=Metro_Read_energy(1,E_W_ACTIVE);
              EN_REACT1=Metro_Read_energy(1,E_REACTIVE);
              EN_APP1=Metro_Read_energy(1,E_APPARENT);
              POW_ACT1=Metro_Read_Power(1, W_ACTIVE);
              POW_REACT1=Metro_Read_Power(1, REACTIVE);
              POW_APP1=Metro_Read_Power(1, APPARENT_RMS);
              AHACC1 =Metro_Read_AH_Acc(1);

              EN_ACT2=Metro_Read_energy(2,E_W_ACTIVE);
              EN_REACT2=Metro_Read_energy(2,E_REACTIVE);
              EN_APP2=Metro_Read_energy(2,E_APPARENT);
              POW_ACT2=Metro_Read_Power(2, W_ACTIVE);
              POW_REACT2=Metro_Read_Power(2, REACTIVE);
              POW_APP2=Metro_Read_Power(2, APPARENT_RMS);
              AHACC1 =Metro_Read_AH_Acc(2);

              // Write data to the text file ################################
              sprintf(buf,"%d,1,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                  tick,RMS_V1,RMS_I1,MOM_V1,MOM_I1,FUND_V1,FUND_I1,Phase1,Period1,SAG1,
                  VSWELL1,CSWELL1,EN_ACT1,EN_REACT1,EN_APP1,POW_ACT1,POW_REACT1,POW_APP1,AHACC1);
              res = f_write(&MyFile, buf, strlen(buf), (void *)&byteswritten);

              sprintf(buf,"%d,2,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
              tick,RMS_V2,RMS_I2,MOM_V2,MOM_I2,FUND_V2,FUND_I2,Phase2,Period2,SAG2,
              VSWELL2,CSWELL2,EN_ACT2,EN_REACT2,EN_APP2,POW_ACT2,POW_REACT2,POW_APP2,AHACC2);
              res = f_write(&MyFile, buf, strlen(buf), (void *)&byteswritten);

              f_sync(&MyFile);
            }
            /*************************************************************/

          }
        }
    }
  }
  
  /*##-11- Unlink the RAM disk I/O driver ####################################*/
  FATFS_UnLinkDriver(SDPath);
  
  /* Infinite loop */
  while (1)
  {
  }
}

/** System Clock Configuration



/* TIM2 init function */
void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED3);
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}


/* USART6 init function */
void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_8;
  HAL_UART_Init(&huart6);

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SCS_B_Pin|SYN_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, SCS_A_Pin|SYN_A_Pin|EN_A_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, SCS_C_Pin|SYN_C_Pin|EN_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED5_Pin|LED6_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EN_B_Pin, GPIO_PIN_RESET);


  /*Configure GPIO pins : SCS_B_Pin SYN_B_Pin PA8 */
  GPIO_InitStruct.Pin = SCS_B_Pin|SYN_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins :  */
  GPIO_InitStruct.Pin = SCS_A_Pin|SYN_A_Pin|EN_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SCS_C_Pin SYN_C_Pin */
  GPIO_InitStruct.Pin = SCS_C_Pin|SYN_C_Pin|EN_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LED5_Pin LED6_Pin */
  GPIO_InitStruct.Pin = LED5_Pin|LED6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : SCS_A_Pin SYN_A_Pin */
  GPIO_InitStruct.Pin = EN_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/**
  * @brief  Configure timer with 0.5s period
  * @param  None
  * @retval None
  */

static void Timer2_Init()
{

  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;

  /* Set TIMx instance */
  htim2.Instance = TIM2;

  htim2.Init.Period            = 2500 - 1;
  htim2.Init.Prescaler         = uwPrescalerValue;
  htim2.Init.ClockDivision     = 0;
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim2.Init.RepetitionCounter = 0;

  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim==&htim2)
  {
    metroData.metroInactiveTime += 1;
    tick++;
  }


}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
