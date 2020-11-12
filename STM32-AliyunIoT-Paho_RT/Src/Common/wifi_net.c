/**
  ******************************************************************************
  * @file    wifi_net.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    04-September-2017
  * @brief   Wifi-specific NET initialization.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
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
#include "wifi.h"
#include "iot_flash_config.h"

/* Private defines -----------------------------------------------------------*/
#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

#ifdef ES_WIFI_MAX_SSID_NAME_SIZE
#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE
#define WIFI_PAYLOAD_SIZE                           ES_WIFI_PAYLOAD_SIZE
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int net_if_init(void * if_ctxt);
int net_if_deinit(void * if_ctxt);

/* Functions Definition ------------------------------------------------------*/
int net_if_init(void * if_ctxt)
{
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  char moduleinfo[WIFI_MAX_MODULE_NAME];
  uint8_t  MAC_Addr[6]; 
  WIFI_Status_t wifiRes;
  int wifiConnectCounter = 0;
  bool skip_reconf = false;
  
  msg_info("\n*** WIFI connection ***\n\n");

  skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK)? 1: 0;

  if (skip_reconf == true)
  {
    msg_info("Push the User button (Blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  
  if (skip_reconf == false)
  {
    msg_info("Your WiFi parameters need to be entered to proceed.\n");
    do
    {
      updateWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode);
    } while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
  }
  
  /*  Wifi Module initialization */
  msg_info("Initializing the WiFi module\n");
  
  wifiRes = WIFI_Init();
  if ( WIFI_STATUS_OK != wifiRes )
  {
    msg_info("Failed to initialize WIFI module\n");
    return -1;
  }
    
  WIFI_GetModuleFwRevision(moduleinfo);
  msg_info("firmware version is : %s\n",moduleinfo);
  
  if(WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK)
  {
      printf("> WiFi module MAC address is: %X:%X:%X:%X:%X:%X\n\n",     
             MAC_Addr[0],
             MAC_Addr[1],
             MAC_Addr[2],
             MAC_Addr[3],
             MAC_Addr[4],
             MAC_Addr[5]);   
  }

  /* Connect to the specified SSID. */

  msg_info("\n");
  do 
  {
    msg_info("\rConnecting to AP: %s  Attempt %d/%d ...",ssid, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
    wifiRes = WIFI_Connect(ssid, psk, security_mode);
    if (wifiRes == WIFI_STATUS_OK) break;
  } 
  while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);
  
  if (wifiRes == WIFI_STATUS_OK)
  {
    msg_info("\nConnected to AP %s\n",ssid);
  }
  else
  {
    msg_info("\nFailed to connect to AP %s\n",ssid);
  }
  
  return (wifiRes == WIFI_STATUS_OK)?0:-1;
}


int net_if_deinit(void * if_ctxt)
{
  return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
