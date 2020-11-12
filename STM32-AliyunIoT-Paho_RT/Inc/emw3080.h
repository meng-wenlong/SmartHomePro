/**
  ******************************************************************************
  * @file    Applications\WiFi\EMW3080_IAP_Client\Inc\EMW3080.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the 
  *          EMW3080 WiFi driver.   
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

#ifndef __EMW3080_H
#define __EMW3080_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32l4xx_hal.h"
/* Private define ------------------------------------------------------------*/
#define MAX_SOCKET_SIZE 512  //set the buffer size according to real application requirement
#define MAX_RX_SIZE             1500
#define MAX_AT_CMD_SIZE         256
#define AT_OK_STRING            "\r\nOK\r\n"
#define AT_SEND_PROMPT_STRING   ">"
#define AT_ERROR_STRING         "\r\nERROR\r\n"
#define EVENT_DATA_IN           "\r\n+CIPEVENT:SOCKET,"

#define AT_STATION_UP			"STATION_UP\r\n"
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  EMW3080_FALSE         = 0,
  EMW3080_TRUE          = 1
} EMW3080_Boolean;

typedef enum
{ 
  EMW3080_OK                            = 0,
  EMW3080_ERROR                         = 1,
  EMW3080_BUSY                          = 2,
  EMW3080_ALREADY_CONNECTED             = 3,
  EMW3080_CONNECTION_CLOSED             = 4,
  EMW3080_TIMEOUT                       = 5,
  EMW3080_IO_ERROR                      = 6,
} EMW3080_StatusTypeDef;

typedef enum
{
  EMW3080_ENCRYPTION_OPEN         = 0,
  EMW3080_ENCRYPTION_WEP          = 1,
  EMW3080_ENCRYPTION_WPA_PSK      = 2,
  EMW3080_ENCRYPTION_WPA2_PSK     = 3,
  EMW3080_ENCRYPTION_WPA_WPA2_PSK = 4,
} EMW3080_EncryptionTypeDef;

typedef enum
{
  EMW3080_STATION_MODE      = 0,
  EMW3080_ACCESSPPOINT_MODE = 1,
  EMW3080_MIXED_MODE        = 2,
} EMW3080_ModeTypeDef;

typedef enum
{
  NORMAL_MODE      = 0,
  UNVARNISHED_MODE = 1
} EMW3080_TransferModeTypeDef;

typedef enum
{
  EMW3080_GOT_IP_STATUS       = 1,
  EMW3080_CONNECTED_STATUS    = 2,
  EMW3080_DISCONNECTED_STATUS = 3,
} EMW3080_ConnectionStatusTypeDef;

typedef enum
{
  EMW3080_TCP_CONNECTION = 0,
  EMW3080_UDP_CONNECTION = 1,
} EMW3080_ConnectionModeTypeDef;

typedef enum
{
  UDP_PEER_NO_CHANGE   = 0,
  UPD_PEER_CHANGE_ONCE = 1,
  UDP_PEER_CHANGE_ALLOWED = 2,
  UDP_PEER_CHANGE_INVALID = -1,
} EMW3080_ConnectionPolicyTypeDef;

typedef struct
{
  EMW3080_ConnectionStatusTypeDef  connectionStatus;
  EMW3080_ConnectionModeTypeDef    connectionType;
  EMW3080_ConnectionPolicyTypeDef  connectionMode;  /* For UDP connections only */
  uint8_t                          connectionID;
  uint8_t*                         ipAddress;
  uint32_t                         port;
  uint32_t                         localPort;       /* for UDP connection only */
  EMW3080_Boolean                  isServer;  
} EMW3080_ConnectionInfoTypeDef;

typedef struct
{
  uint8_t*                   ssid;
  uint8_t*                   password;
  uint16_t                   channedID;
  EMW3080_EncryptionTypeDef  encryptionMode;
} EMW3080_APConfigTypeDef;

typedef struct
{
  uint32_t socket;
  uint8_t* pdata;
  uint16_t offset;
  uint16_t left;  
}sock_buffer_ctx_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
EMW3080_StatusTypeDef EMW3080_Init(void);
EMW3080_StatusTypeDef EMW3080_DeInit(void);
EMW3080_StatusTypeDef EMW3080_Restart(void);
EMW3080_StatusTypeDef EMW3080_EchoOFF(void);
EMW3080_StatusTypeDef EMW3080_QuitAccessPoint(void);
EMW3080_StatusTypeDef EMW3080_JoinAccessPoint(uint8_t* ssid, uint8_t* password);
EMW3080_StatusTypeDef EMW3080_GetIPAddress(EMW3080_ModeTypeDef mode, uint8_t* ip_address);
EMW3080_StatusTypeDef EMW3080_EstablishConnection(const EMW3080_ConnectionInfoTypeDef* connection_info);
EMW3080_StatusTypeDef EMW3080_CloseConnection(uint8_t channel_id);
//EMW3080_StatusTypeDef EMW3080_CloseAutoConnection(uint8_t channel_id);

EMW3080_StatusTypeDef EMW3080_SendData(uint8_t socket,uint8_t* pData, uint32_t Length, uint32_t Timeout);
EMW3080_StatusTypeDef EMW3080_ReceiveData(uint8_t socket,uint8_t* pData, uint32_t Length, uint32_t* retLength, uint32_t Timeout);

EMW3080_StatusTypeDef EMW3080_GetHostAddress(const char* url, uint8_t* IpAddress);
EMW3080_StatusTypeDef EMW3080_CheckAPConnectionStatus(void);//@sz
EMW3080_StatusTypeDef EMW3080_IsConnectedWithServer(uint8_t channel_id);
EMW3080_StatusTypeDef EMW3080_IsDisconnectedFromServer(uint8_t channel_id);
EMW3080_StatusTypeDef EMW3080_WaitForEvent(const uint8_t* Token1);
EMW3080_StatusTypeDef EMW3080_Reset2FactoryDefault(void);
EMW3080_StatusTypeDef EMW3080_Restart(void);
EMW3080_StatusTypeDef EMW3080_GetVersionInfo(uint8_t* fwver_string);
EMW3080_StatusTypeDef EMW3080_WIFI_GetMACAddress(uint8_t  *mac);

void Socks_Buffer_Init(void);
sock_buffer_ctx_t* Get_Socks_Buffer(uint8_t socket);
int Malloc_Socks_Buffer(uint8_t socket);
void Free_Socks_Buffer(uint8_t socket);
#ifdef __cplusplus
}
#endif

#endif /* __EMW3080_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
