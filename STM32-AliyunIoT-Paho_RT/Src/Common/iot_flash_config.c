/**
  ******************************************************************************
  * @file    iot_flash_config.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   configuration in flash memory.
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "flash.h"
#include "iot_flash_config.h"
#include "msg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/** Do not zero-initialize the static user configuration. 
 *  Otherwise, it must be entered manually each time the device FW is updated by STLink. 
 */

#ifdef __ICCARM__  /* IAR */
__no_init const user_config_t lUserConfig @ "UNINIT_FIXED_LOC";
#elif defined ( __CC_ARM   )/* Keil / armcc */
user_config_t lUserConfig __attribute__((section("UNINIT_FIXED_LOC"), zero_init));
#elif defined ( __GNUC__ )      /*GNU Compiler */
user_config_t lUserConfig __attribute__((section("UNINIT_FIXED_LOC")));
#endif

/* Private function prototypes -----------------------------------------------*/
static int getInputString(char *inputString, size_t len);

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Get a line from the console (user input).
  * @param  Out:  inputString   Pointer to buffer for input line.
  * @param  In:   len           Max length for line.
  * @retval Number of bytes read from the terminal.
  */
static int getInputString(char *inputString, size_t len)
{
  size_t currLen = 0;
  int c = 0;
  
  c = getchar();
  
  while ((c != EOF) && ((currLen + 1) < len) && (c != '\r') && (c != '\n') )
  {
    if (c == '\b')
    {
      if (currLen != 0)
      {
        --currLen;
        inputString[currLen] = 0;
        msg_info(" \b");
      }
    }
    else
    {
      if (currLen < (len-1))
      {
        inputString[currLen] = c;
      }
      
      ++currLen;
    }
    c = getchar();
  }
  if (currLen != 0)
  { /* Close the string in the input buffer... only if a string was written to it. */
    inputString[currLen] = '\0';
  }
  if (c == '\r')
  {
    c = getchar(); /* assume there is '\n' after '\r'. Just discard it. */
  }
  
  return currLen;
}


/**
  * @brief  Check whether the Wifi parameters are present in FLASH memory.
  *         Returns the parameters if present.
  * @param  Out:  ssid              Wifi SSID.
  * @param  Out:  psk               Wifi security password.
  * @param  Out:  security_mode     See @ref wifi_network_security_t definition.
  * @retval   0 if the parameters are present in FLASH.
  *          -1 if the parameters are not present in FLASH.
  */
int checkWiFiCredentials(const char ** const ssid, const char ** const psk, uint8_t * const security_mode)
{
  bool is_ssid_present = 0;
  
  if (lUserConfig.wifi_config.magic == USER_CONF_MAGIC)
  {
    is_ssid_present = true;
    if ((ssid == NULL) ||(psk == NULL) || (security_mode == NULL))
    {
      return -2;
    }
    *ssid = lUserConfig.wifi_config.ssid;
    *psk = lUserConfig.wifi_config.psk;
    *security_mode = lUserConfig.wifi_config.security_mode;
  }
 
  return (is_ssid_present) ? 0 : -1;
}


/**
  * @brief  Write the Wifi parameters to the FLASH memory.
  * @param  In:   ssid            Wifi SSID.
  * @param  In:   psk             Wifi security password.
  * @param  In:   security_mode   See @ref wifi_network_security_t definition.
  * @retval	Error code
  * 			0	Success
  * 			<0	Unrecoverable error
  */
int updateWiFiCredentials(const char ** const ssid, const char ** const psk, uint8_t * const security_mode)
{
  wifi_config_t wifi_config;
  int ret = 0;

  if ((ssid == NULL) ||(psk == NULL) || (security_mode == NULL))
  {
    return -1;
  }
  
  memset(&wifi_config, 0, sizeof(wifi_config_t));
    
  msg_info("\nEnter SSID: ");
  
  getInputString(wifi_config.ssid, USER_CONF_WIFI_SSID_MAX_LENGTH);
  msg_info("You have entered %s as the ssid.\n", wifi_config.ssid);
  
  
  msg_info("\n");
  char c;
  do
  {
      msg_info("\rEnter Security Mode (0 - Open, 1 - WEP, 2 - WPA, 3 - WPA2): \b");
      c = getchar();
  }
  while ( (c < '0')  || (c > '3'));
  wifi_config.security_mode = c - '0';
  msg_info("\nYou have entered %d as the security mode.\n", wifi_config.security_mode);
  
  
  if (wifi_config.security_mode != 0)
  {
    msg_info("\nEnter password: "); 
    getInputString(wifi_config.psk, sizeof(wifi_config.psk));
  }
  
  wifi_config.magic = USER_CONF_MAGIC;


  ret = FLASH_update((uint32_t)&lUserConfig.wifi_config, &wifi_config, sizeof(wifi_config_t));  

  if (ret < 0)
  {
    msg_error("Failed updating the wifi configuration in FLASH.\n");
  }

  msg_info("\n");
  return ret;
}


/**
  * @brief  Ask user and write in FLASH memory the Product Key,
  *          device name and the device secret.
  * @retval	Error code
  * 			0	Success
  * 			<0	Unrecoverable error
  */
int updateDeviceConfig(void)
{
  iot_config_t iot_config;
  int ret = 0;
  msg_info("\nStart to enter Ali Device Parameters: Region ID,Product Key,Device Name and Device Secret\n");

  memset(&iot_config, 0, sizeof(iot_config_t));
  
  msg_info("\nEnter Region ID: (example: cn-shanghai) \n");
  getInputString(iot_config.region_id, USER_CONF_REGION_ID_LENGTH);
  msg_info("read: --->\n%s\n<---\n", iot_config.region_id);
    
  msg_info("\nEnter Product Key: (example: a1b05Uexxxx) \n");
  getInputString(iot_config.product_key, USER_CONF_PRODUCT_KEY_LENGTH);
  msg_info("read: --->\n%s\n<---\n", iot_config.product_key);
  
  msg_info("\nEnter device name: (example: mydevicename) \n");
  getInputString(iot_config.device_name, USER_CONF_DEVICE_NAME_LENGTH);
  msg_info("read: --->\n%s\n<---\n", iot_config.device_name);

  msg_info("\nEnter device secret: (example: 7o7GJ3odUE7pPnie07dzxxxxxxxxxxxx) \n");
  getInputString(iot_config.device_secret, USER_CONF_DEVICE_SECRET_LENGTH);
  msg_info("read: --->\n%s\n<---\n", iot_config.device_secret);
  
  iot_config.magic = USER_CONF_MAGIC;
   
  ret = FLASH_update((uint32_t)&lUserConfig.iot_config, &iot_config, sizeof(iot_config_t));
  
  if (ret < 0)
  {
    msg_error("Failed programming the IOT server / device config into Flash.\n");
  }
  return ret;
}


int getRegionId(const char ** const address)
{
  int ret = HAL_ERROR;
  
  if (address != NULL)
  {
    if (lUserConfig.iot_config.magic == USER_CONF_MAGIC)
    {
      *address = lUserConfig.iot_config.region_id;
      //ret = HAL_OK;
      return HAL_OK;
    } else {
      *address = NULL;
    }
  }
  return ret;
}

/**
  * @brief  Get the Product Key from FLASH memory.
  * @param  Out:  address   Pointer to location of the ProductKey.
  * @retval HAL_OK      The ProductKey is configured and returned to the caller. 
  *         HAL_ERROR   No ProductKey is configured.
  */
int getProductKey(const char ** const address)
{
  int ret = HAL_ERROR;
  
  if (address != NULL)
  {
    if (lUserConfig.iot_config.magic == USER_CONF_MAGIC)
    {
      *address = lUserConfig.iot_config.product_key;
      ret = HAL_OK;
    } else {
      *address = NULL;
    }
  }
  return ret;
}


/**
  * @brief  Get the device name from FLASH memory.
  * @param  Out:  name    Pointer to location of the device name.
  * @retval HAL_OK      The device name is configured and returned to the caller. 
  *         HAL_ERROR   No device name is configured.
  
  */
int getDeviceName(const char ** const name)
{
  int ret = HAL_ERROR;

  if (name != NULL)
  {
    if (lUserConfig.iot_config.magic == USER_CONF_MAGIC)
    {
      *name = lUserConfig.iot_config.device_name;
      ret = HAL_OK;
    } else {
      *name = NULL;
    }
  }
  return ret;
}


/**
  * @brief  Get the device Secret from FLASH memory.
  * @param  Out:     Pointer to location of the device Secret.
  * @retval HAL_OK      The device Secret is configured and returned to the caller. 
  *         HAL_ERROR   No device Secret is configured.
  
  */
int getDeviceSecret(const char ** const name)
{
  int ret = HAL_ERROR;

  if (name != NULL)
  {
    if (lUserConfig.iot_config.magic == USER_CONF_MAGIC)
    {
      *name = lUserConfig.iot_config.device_secret;
      ret = HAL_OK;
    } else {
      *name = NULL;
    }
  }
  return ret;
}


/**
  * @brief  Check if the Product Key, the device name and device secret are present in FLASH memory.
  * @retval 0:  The device certificate are configured.
  *        -1:  The device certificate are not configured.
  */
int checkDeviceConfig()
{
  return (lUserConfig.iot_config.magic == USER_CONF_MAGIC) ? 0 : -1;
}

int getTempThresholdInFlash(int8_t * threshold_p)
{
  int ret = -1;
  
  if(threshold_p!=NULL)
  {
    if (lUserConfig.app_config.magic == USER_CONF_MAGIC)
    {
      *threshold_p = lUserConfig.app_config.temprature_threshold;
      ret = 0;
    }
    else
    {
      *threshold_p = NULL;
    }
  }
  return ret;
}


int updateTempThreshold(int8_t value)
{
  app_config_t app_config;
  int ret = 0;

  memset(&app_config, 0, sizeof(app_config_t));
  
  app_config.magic = USER_CONF_MAGIC;
  app_config.temprature_threshold = value;
  
  ret = FLASH_update((uint32_t)&lUserConfig.app_config, &app_config, sizeof(app_config_t));
  
  if (ret < 0)
  {
    msg_error("Failed programming the temprature threshold config into Flash.\n");
  }
  return ret;}

/*接收光强阈值的修改*/
int updateLightThreshold(int8_t value)    /*这是一个大胆的尝试*/
{
  app_config_t app_config;
  int ret = 0;
  
  memset(&app_config, 0, sizeof(app_config_t));
  
  app_config.magic = USER_CONF_MAGIC;
  app_config.light_threshold = value;
  
  ret = FLASH_update((uint32_t)&lUserConfig.app_config, &app_config, sizeof(app_config_t));
  
  if (ret < 0)
  {
    msg_error("Failed programming the temprature threshold config into Flash.\n");
  }
  return ret;
}

/*接收光强阈值的修改*/
int getLightThresholdInFlash(uint16_t * threshold_p)
{
  int ret = -1;
  
  if(threshold_p!=NULL)
  {
    if (lUserConfig.app_config.magic == USER_CONF_MAGIC)
    {
      *threshold_p = lUserConfig.app_config.light_threshold;
      ret = 0;
    }
    else
    {
      *threshold_p = NULL;
    }
  }
  return ret;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
