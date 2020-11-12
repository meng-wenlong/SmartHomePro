/**
  ******************************************************************************
  * @file    wifi.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    04-September-2017
  * @brief   WIFI interface file.
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
#include "wifi.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
EMW3080_ConnectionInfoTypeDef conn;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialiaze the WIFI core
  * @param  None
  * @retval Operation status
  */
WIFI_Status_t WIFI_Init(void)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR;
    Socks_Buffer_Init();
 
    if(EMW3080_Init() == EMW3080_OK)
    {
      ret = WIFI_STATUS_OK;
    }
    EMW3080_EchoOFF(); 
    //close all socket connections to avoid auto connection
    uint8_t i=0;
    for(i=0;i<5;i++)
    {
        if(EMW3080_CloseConnection(i)== EMW3080_OK)
        {
          ret = WIFI_STATUS_OK;
        }
    }
  
  return ret;
}

/**
  * @brief  Join an Access Point
  * @param  SSID : SSID string
  * @param  Password : Password string
  * @param  ecn : Encryption type
  * @param  IP_Addr : Got IP Address
  * @param  IP_Mask : Network IP mask
  * @param  Gateway_Addr : Gateway IP address
  * @param  MAC : pointer to MAC Address
  * @retval Operation status
  */
WIFI_Status_t WIFI_Connect(
                             const char* SSID, 
                             const char* Password,
                             WIFI_Ecn_t ecn)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR;  
 
  if(EMW3080_JoinAccessPoint((uint8_t*)SSID, (uint8_t*)Password) == EMW3080_OK)
  {
       ret = WIFI_STATUS_OK;
   
  }
  return ret;
}

/**
  * @brief  This function retrieves the WiFi interface's MAC address.
  * @retval Operation Status.
  */
WIFI_Status_t WIFI_GetMAC_Address(uint8_t  *mac)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR; 
  
  if(EMW3080_WIFI_GetMACAddress(mac) == EMW3080_OK)
  {
    ret = WIFI_STATUS_OK;
  }
  return ret;
}

/**
  * @brief  This function retrieves the WiFi interface's IP address.
  * @retval Operation Status.
  */
WIFI_Status_t WIFI_GetIP_Address (uint8_t  *ipaddr)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR; 
  
  if(EMW3080_GetIPAddress(EMW3080_STATION_MODE,ipaddr) == EMW3080_OK)
  {
    ret = WIFI_STATUS_OK;
  }
  return ret;
}

/**
  * @brief  Get IP address from URL using DNS
  * @param  location : Host URL
  * @param  ipaddr : array of the IP address
  * @retval Operation status
  */
WIFI_Status_t WIFI_GetHostAddress(char* location, uint8_t* ipaddr)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR;  
  
  if (EMW3080_GetHostAddress(location, ipaddr) == EMW3080_OK)
  {
    //some EMW3080 will return ip address=0
    if(*ipaddr==0)
      ret = WIFI_STATUS_ERROR;
    else
      ret = WIFI_STATUS_OK;
  }
  
  return ret;
}
/**
  * @brief  Configure and start a client connection
  * @param  type : Connection type TCP/UDP
  * @param  name : name of the connection
  * @param  ipaddr : Client IP address
  * @param  port : Remote port
  * @param  local_port : Local port
  * @retval Operation status
  */
WIFI_Status_t WIFI_OpenClientConnection(uint32_t socket, WIFI_Protocol_t type, const char* name, uint8_t* ipaddr, uint16_t port, uint16_t local_port)
{
  uint32_t ret = WIFI_STATUS_ERROR;
  uint8_t count=0;
  
  memset(&conn, '\0', sizeof (EMW3080_ConnectionInfoTypeDef));
  
  conn.connectionID = socket;
  conn.port = port;
  conn.localPort = local_port;
  conn.connectionType = (type == WIFI_TCP_PROTOCOL)? EMW3080_TCP_CONNECTION : EMW3080_UDP_CONNECTION;//!!UDP connection not tested
  conn.ipAddress = ipaddr;

  //check if this socket id is occupied
  //!!if use multi-connection, it will have problem here - TODO
  //this code need change to support multi-connection - TODO
  if(EMW3080_IsDisconnectedFromServer(socket)!=EMW3080_OK)
  {
    count=0;
    do
    {
      EMW3080_CloseConnection(socket);
      ret = EMW3080_WaitForEvent("CLOSED\r\n");
      count++;
    }while(ret!=EMW3080_OK && count<3);
  }
  if(EMW3080_EstablishConnection((EMW3080_ConnectionInfoTypeDef*)&conn)== EMW3080_OK)
  {
      if(EMW3080_WaitForEvent("CONNECTED\r\n")==EMW3080_OK)
      {
        //connected to server
        //malloc socket buffer this connection service
        if((ret=Malloc_Socks_Buffer(socket))==0)
          ret = WIFI_STATUS_OK;
        else
          ret = WIFI_STATUS_ERROR;
      }
      else
        ret = WIFI_STATUS_ERROR;
  }
  
  return (WIFI_Status_t)ret;
}

/**
  * @brief  Close client connection
  * @param  type : Connection type TCP/UDP
  * @param  name : name of the connection
  * @param  location : Client address
  * @param  port : Remote port
  * @param  local_port : Local port
  * @retval Operation status
  */
WIFI_Status_t WIFI_CloseClientConnection(uint32_t socket)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR;  
  
  if(EMW3080_CloseConnection(socket)== EMW3080_OK)
  {
    ret = WIFI_STATUS_OK;
  }
  //free socks buffer
  Free_Socks_Buffer(socket);
  return ret; 
}

/**
  * @brief  Send Data on a socket
  * @param  pdata : pointer to data to be sent
  * @param  len : length of data to be sent
  * @retval Operation status
  */
WIFI_Status_t WIFI_SendData(uint8_t socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *SentDatalen, uint32_t Timeout)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR;
  *SentDatalen = Reqlen;
    if(EMW3080_SendData(socket,pdata, (uint32_t)Reqlen, Timeout) == EMW3080_OK)
    {
      ret = WIFI_STATUS_OK;
    }
    else
      *SentDatalen = 0;
  return ret;
}

/**
  * @brief  Receive Data from a socket
  * @param  pdata : pointer to Rx buffer
  * @param  *len :  pointer to length of data
  * @retval Operation status
  */
WIFI_Status_t WIFI_ReceiveData(uint8_t socket, uint8_t *pdata, uint16_t Reqlen, uint16_t* RcvDatalen, uint32_t Timeout)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR; 
  uint32_t Len = 0;

 
  if(EMW3080_ReceiveData(socket,pdata, (uint32_t)Reqlen, &Len, Timeout) == EMW3080_OK)
  {
    *RcvDatalen = Len;
    ret = WIFI_STATUS_OK; 
  }
  
  return ret;
}

/**
  * @brief  Reset the WIFI module
  * @retval Operation status
  */
WIFI_Status_t WIFI_ResetModule(void)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR; 
  
  if(EMW3080_Restart() == EMW3080_OK)
  {
      ret = WIFI_STATUS_OK;
  }
  return ret;
}

/**
  * @brief  Restore module default configuration
  * @retval Operation status
  */
WIFI_Status_t WIFI_SetModuleDefault(void)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR; 
  
  if(EMW3080_Reset2FactoryDefault() == EMW3080_OK)
  {
      ret = WIFI_STATUS_OK;
  }
  return ret;
}

/**
  * @brief  Return Module firmware revision
  * @param  rev : revision string
  * @retval Operation status
  */
WIFI_Status_t WIFI_GetModuleFwRevision(char *rev)
{
  WIFI_Status_t ret = WIFI_STATUS_ERROR; 
    
  if(EMW3080_GetVersionInfo((uint8_t*)rev) == EMW3080_OK)
  {
      ret = WIFI_STATUS_OK;
  }
  return ret;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
