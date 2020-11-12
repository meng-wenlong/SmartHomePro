/**
  ******************************************************************************
  * @file    WiFi/EMW3080_IAP_Client/Src/EMW3080_io.c
  * @author  MCD Application Team
  * @brief   This file implments the IO operations to deal with the EMW3080 wifi
  *          module. It mainly Inits and Deinits the UART interface. Send and
  *          receive data over it.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "emw3080_io.h"
    
#include "oled.h"

/* Private define ------------------------------------------------------------*/
#define LENGTH 3
//
///* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RingBuffer_t WiFiRxBuffer;
extern DeviceStatusTypeDef  device_s;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;
extern uint8_t RxFlag;
extern uint8_t RxBuffer2[LENGTH];

uint8_t UartTxDone;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EMW3080 IO Initalization.
  *         This function inits the UART interface to deal with the EMW3080,
  *         then starts asynchronous listening on the RX port.
  * @param None
  * @retval 0 on success, -1 otherwise.
  */
int8_t EMW3080_IO_Init(void)
{
 
    /* Set the WiFi USART configuration parameters */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  /* Configure the USART IP */
  if(HAL_UART_Init(&huart3) != HAL_OK)
  {
    return -1;
  }
  /* Once the WiFi UART is intialized, start an asynchrounous recursive 
   listening. the HAL_UART_Receive_IT() call below will wait until one char is
   received to trigger the HAL_UART_RxCpltCallback(). The latter will recursively
   call the former to read another char.  */
  WiFiRxBuffer.head = 0;
  WiFiRxBuffer.tail = 0;
 
  HAL_UART_Receive_IT(&huart3, (uint8_t *)&WiFiRxBuffer.data[WiFiRxBuffer.tail], 1);

  return 0;
}

/**
  * @brief  EMW3080 IO Deinitialization.
  *         This function Deinits the UART interface of the EMW3080. When called
  *         the EMW3080 commands can't be executed anymore.
  * @param  None.
  * @retval None.
  */
void EMW3080_IO_DeInit(void)
{
  /* Reset USART configuration to default */
  HAL_UART_DeInit(&huart3);
}

/**
  * @brief  Send Data to the EMW3080 module over the UART interface.
  *         This function allows sending data to the  EMW3080 WiFi Module, the
  *          data can be either an AT command or raw data to send over 
             a pre-established WiFi connection.
  * @param pData: data to send.
  * @param Length: the data length.
  * @retval 0 on success, -1 otherwise.
  */
int8_t EMW3080_IO_Send(uint8_t* pData, uint32_t Length)
{
  uint32_t tickstart;

  UartTxDone = 0;
  if (HAL_UART_Transmit_IT(&huart3, (uint8_t*)pData, Length) != HAL_OK)
  {
     return -1;
  }
  tickstart = HAL_GetTick();
  while(UartTxDone!=1)
  {
    if((HAL_GetTick()-tickstart) > DEFAULT_TIME_OUT)
    {
      return -1;
    }
  }
  
  return 0;
}

/**
  * @brief  Receive Data from the EMW3080 module over the UART interface.
  *         This function receives data from the  EMW3080 WiFi module, the
  *         data is fetched from a ring buffer that is asynchonously and continuously
            filled with the received data.
  * @param  Buffer: a buffer inside which the data will be read.
  * @param  Length: the Maximum size of the data to receive.
  * @retval int32_t: the actual data size that has been received.
  */
int32_t EMW3080_IO_Receive(uint8_t* Buffer, uint32_t Length, uint32_t Timeout)
{
  uint32_t ReadData = 0;

  /* Loop until data received */
  while (Length--)
  {
    uint32_t tickStart = HAL_GetTick();
    do
    {
      if(WiFiRxBuffer.head != WiFiRxBuffer.tail)
      {
        /* serial data available, so return data to user */
        *Buffer++ = WiFiRxBuffer.data[WiFiRxBuffer.head++];
        ReadData++;
		
        /* check for ring buffer wrap */
        if (WiFiRxBuffer.head >= RING_BUFFER_SIZE)
        {
          /* Ring buffer wrap, so reset head pointer to start of buffer */
          WiFiRxBuffer.head = 0;
        }

        break;
      }
    }while((HAL_GetTick() - tickStart ) < Timeout);
  }


  return ReadData;
}

//整形转字符，stm32不支持标准的itoa,这里自己实现
char *myitoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;
 
    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
 
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
 
    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';
 
        /* Make the value positive. */
        value *= -1;
    }
 
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
 
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
 
    /* Null terminate the string. */
    *ptr = 0;
 
    return string;
}

/**
  * @brief  Rx Callback when new data is received on the UART.
  * @param  UartHandle: Uart handle receiving the data.
  * @retval None.
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if(UartHandle ->Instance == USART2)
  {
    //RxFlag = 1;
    HAL_UART_Receive_IT(&huart2, (uint8_t *)RxBuffer2, LENGTH - 1);    //使能接收中断
    msg_info("Receive Success!\r\n");
    msg_info("%s\r\n", RxBuffer2);
    if (RxBuffer2[0] == '0' && RxBuffer2[1] == '0')     //我在！
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      OLED_ShowCHinese(0, 0, 4);
      OLED_ShowCHinese(16, 0, 5);
      OLED_ShowCHinese(32, 0, 6);
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '1')    //降低阈值
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      device_s.Tem_threshold = device_s.Tem_threshold - 1;
      OLED_ShowCHinese(0, 0, 7);        //当
      OLED_ShowCHinese(16, 0, 8);       //前
      OLED_ShowCHinese(32, 0, 9);       //阈
      OLED_ShowCHinese(48, 0, 10);      //值
      OLED_ShowCHinese(64, 0, 11);      //为
      char Tem_string[6] = {0};
      myitoa(device_s.Tem_threshold, Tem_string, 10);
      OLED_ShowString(80, 0, (uint8_t* )Tem_string, 16);
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '2')    //提高阈值
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      device_s.Tem_threshold = device_s.Tem_threshold + 1;
      OLED_ShowCHinese(0, 0, 7);        //当
      OLED_ShowCHinese(16, 0, 8);       //前
      OLED_ShowCHinese(32, 0, 9);       //阈
      OLED_ShowCHinese(48, 0, 10);      //值
      OLED_ShowCHinese(64, 0, 11);      //为
      char Tem_string[6] = {0}; 
      myitoa(device_s.Tem_threshold, Tem_string, 10);
      OLED_ShowString(80, 0, (uint8_t* )Tem_string, 16);
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '3')    //显示温度
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      OLED_ShowCHinese(0, 0, 7);        //当
      OLED_ShowCHinese(16, 0, 8);       //前
      OLED_ShowCHinese(32, 0, 13);      //温
      OLED_ShowCHinese(48, 0, 14);      //度
      OLED_ShowCHinese(64, 0, 11);      //为
      OLED_ShowCHinese(80, 0, 12);      //：
      char Tem_string[6] = {0};
      myitoa(device_s.Tem_value, Tem_string, 10);
      OLED_ShowString(0, 3, (uint8_t* )Tem_string, 16);
      OLED_ShowCHinese(32, 3, 19);      //℃
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '4')    //显示湿度
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      OLED_ShowCHinese(0, 0, 7);        //当
      OLED_ShowCHinese(16, 0, 8);       //前
      OLED_ShowCHinese(32, 0, 15);      //湿
      OLED_ShowCHinese(48, 0, 16);      //度
      OLED_ShowCHinese(64, 0, 11);      //为
      OLED_ShowCHinese(80, 0, 12);      //：
      char Tem_string[6] = {0};
      myitoa(device_s.Hum_value, Tem_string, 10);
      OLED_ShowString(0, 3, (uint8_t* )Tem_string, 16);
      OLED_ShowChar(32, 3, '%', 16);
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '5')    //显示光强
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      OLED_ShowCHinese(0, 0, 7);        //当
      OLED_ShowCHinese(16, 0, 8);       //前
      OLED_ShowCHinese(32, 0, 17);      //光
      OLED_ShowCHinese(48, 0, 18);      //强
      OLED_ShowCHinese(64, 0, 11);      //为
      OLED_ShowCHinese(80, 0, 12);      //：
      char Tem_string[6] = {0};
      myitoa(device_s.Light_value, Tem_string, 10);
      OLED_ShowString(0, 3, (uint8_t* )Tem_string, 16);
      OLED_ShowString(32, 3, "lux", 16);
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '6')    //全部显示
    {
      RxFlag = 1;
      OLED_Clear();         //OLED清屏
      char Tem_string[6] = {0};
      OLED_ShowString(0, 0, "T:", 16);
      myitoa(device_s.Tem_value, Tem_string, 10);
      OLED_ShowString(32, 0, (uint8_t* )Tem_string, 16);
      OLED_ShowCHinese(64, 0, 19);      //℃
      OLED_ShowString(0, 3, "H:", 16);
      myitoa(device_s.Hum_value, Tem_string, 10);
      OLED_ShowString(32, 3, (uint8_t* )Tem_string, 16);
      OLED_ShowChar(64, 3, '%', 16);    //%
      OLED_ShowString(0, 6, "L:", 16);
      myitoa(device_s.Light_value, Tem_string, 10);
      OLED_ShowString(32, 6, (uint8_t* )Tem_string, 16);
      OLED_ShowString(64, 6, "lux", 16);//lux
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '7')    //息屏
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
    }
    else if(RxBuffer2[0] == '0' && RxBuffer2[1] == '8')    //歪比巴卜
    {
      RxFlag = 0;
      OLED_Clear();         //OLED清屏
      device_s.Tem_threshold = device_s.Tem_threshold - 5;
      OLED_ShowCHinese(0, 0, 7);        //当
      OLED_ShowCHinese(16, 0, 8);       //前
      OLED_ShowCHinese(32, 0, 9);       //阈
      OLED_ShowCHinese(48, 0, 10);      //值
      OLED_ShowCHinese(64, 0, 11);      //为
      char Tem_string[6] = {0};
      myitoa(device_s.Tem_threshold, Tem_string, 10);
      OLED_ShowString(80, 0, (uint8_t* )Tem_string, 16);
    }
  }
  else 
  {
    static uint32_t count_test1 =0;
    count_test1++;
    
    /* If ring buffer end is reached reset tail pointer to start of buffer */
    if(++WiFiRxBuffer.tail >= RING_BUFFER_SIZE)
    {
      WiFiRxBuffer.tail = 0;   
    }
    
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&WiFiRxBuffer.data[WiFiRxBuffer.tail], 1);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartTxDone = 1;  
}
/**
  * @brief  Function called when error happens on the UART.
  * @param  UartHandle: Uart handle receiving the data.
  * @retval None.
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Add user code here*/
}

/**
  * @brief  Handler to deinialize the EMW3080 UART interface in case of errors.
  * @param  None
  * @retval None.
  */
//static void WIFI_Handler(void)
//{
//  HAL_UART_DeInit(&WiFiUartHandle);
//  
//  while(1)
//  {
//  }
//}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
