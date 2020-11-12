/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "iot_flash_config.h"
#include "net.h"
#include "msg.h"
#include "version.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
enum {BP_NOT_PUSHED=0, BP_SINGLE_PUSH, BP_MULTIPLE_PUSH};

typedef enum
{
   TEMPERATURE_NORMAL =0,
   TEMPERATURE_HIGH_ALARM ,
   TEMPERATURE_HIGH_ALARMSTOP
}Temperature_StatusTypeDef;


typedef struct{
  int8_t Tem_value;       //温度
  uint8_t Hum_value;      //湿度
  Temperature_StatusTypeDef Tem_status;   //温度状态：正常、高于警报、高于警报但取消警报
  int8_t Tem_threshold;   //温度阈值
  uint16_t Light_value;     //光照强度      /*修改*/
  uint16_t Light_threshold;     //光照强度阈值的修改
}DeviceStatusTypeDef;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define LED_LIGHT_ON_DEFAULT      50//lux  光照强度阈值的修改
#define TEMPERATURE_ALARM_DEFAULT 28//℃
#define TEMPERATURE_ALARM_DELTA   1 //℃
#define REGULAR_TIMER_INTERVAL    5000//5S
#define ALARM_TIMER_INTERVAL      10000//10S
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern net_hnd_t         hnet;
#if defined(USE_WIFI)
#define NET_IF  NET_IF_WLAN
#elif defined(USE_LWIP)
#define NET_IF  NET_IF_ETH
#elif defined(USE_C2C)
#define NET_IF  NET_IF_C2C
#endif
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint8_t Button_WaitForPush(uint32_t timeout);
extern uint8_t GetHumValue(void);
extern int8_t GetTemperatureValue(void);
extern uint16_t GetLightValue(void);    /*修改*/
extern int8_t GetTempratureThreshold(void);     /*修改*/
extern uint16_t GetLightThreshold(void);        /*修改*/
extern void SetTempratureThreshold(int8_t value);
extern void SetLightThreshold(uint16_t value);  /*光照强度阈值的修改*/
extern void SetTemperatureStatus(Temperature_StatusTypeDef  status_value);
extern Temperature_StatusTypeDef  GetTemperatureStatus(void);
extern void SetTemperatureValue(int8_t value);
extern void SetHumValue(uint8_t value);
extern void SetLightValue(uint16_t value);      /*修改*/
/*custom mems processor functions*/
extern int32_t Sensor_Init(void);
extern void Temp_Sensor_Handler(void);
extern void Hum_Sensor_Handler(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define USER_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define LD_light_Pin GPIO_PIN_0    //调节光强的led GPIO的修改
#define LD_light_GPIO_Port GPIOC   //调节光强的led GPIO的修改
#define FAN_Pin GPIO_PIN_3         //风扇GPIO的修改
#define FAN_GPIO_Port GPIOC        //风扇GPIO的修改
#define LD1_Pin GPIO_PIN_7
#define LD1_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
