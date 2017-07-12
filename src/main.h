/**
 ******************************************************************************
 * @file    FatFs/FatFs_uSD/Inc/main.h
 * @author  MCD Application Team
 * @version V1.2.1
 * @date    13-March-2015
 * @brief   Header for main.c module
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* EVAL includes component */
//#include "stm324xg_eval.h"
#include "stm32f4_discovery.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"

/* Private define ------------------------------------------------------------*/

#define SCS_B_Pin GPIO_PIN_0
#define SCS_B_GPIO_Port GPIOA

#define SYN_B_Pin GPIO_PIN_1
#define SYN_B_GPIO_Port GPIOA

#define EN_B_Pin GPIO_PIN_3
#define EN_B_GPIO_Port GPIOC

#define EN_C_Pin GPIO_PIN_13
#define EN_C_GPIO_Port GPIOE

#define SCS_C_Pin GPIO_PIN_14
#define SCS_C_GPIO_Port GPIOE

#define SYN_C_Pin GPIO_PIN_15
#define SYN_C_GPIO_Port GPIOE

#define LED5_Pin GPIO_PIN_14
#define LED5_GPIO_Port GPIOD

#define LED6_Pin GPIO_PIN_15
#define LED6_GPIO_Port GPIOD

#define EN_A_Pin GPIO_PIN_0
#define EN_A_GPIO_Port GPIOB

#define SCS_A_Pin GPIO_PIN_1
#define SCS_A_GPIO_Port GPIOB

#define SYN_A_Pin GPIO_PIN_2
#define SYN_A_GPIO_Port GPIOB

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
