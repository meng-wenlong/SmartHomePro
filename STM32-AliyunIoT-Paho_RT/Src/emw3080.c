/**
  ******************************************************************************
  * @file    EMW3080.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the EMW3080
  *          WiFi devices.
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
#include "emw3080.h"
#include "emw3080_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t AtCmd[MAX_AT_CMD_SIZE];
uint8_t RxBuffer[MAX_RX_SIZE];
sock_buffer_ctx_t socks_buffer[5];
/* Private function prototypes -----------------------------------------------*/
static EMW3080_StatusTypeDef runAtCmd(uint8_t* cmd, uint32_t Length, const uint8_t* Token, uint32_t Timeout);
static EMW3080_StatusTypeDef getData(uint8_t socket,uint32_t* RetLength, uint32_t Timeout);
//EMW3080_StatusTypeDef EMW3080_GetVersionInfo(uint8_t* fwver_string);

EMW3080_StatusTypeDef EMW3080_FlashLockToggle(uint8_t flag);
EMW3080_StatusTypeDef EMW3080_IsDisconnectedFromServer(uint8_t channel_id);
void ParseNumberFromString(uint8_t* string, uint8_t* number_p, uint8_t* separate_mark, uint8_t* end_mark);

static EMW3080_StatusTypeDef pullSocketData(uint8_t socket, uint32_t timeout,uint32_t* RetLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Initialize the EMW3080 module.
  *          IT intitalize the IO to communicate between the MCU and the module, then
  *          test that the modules is working using some basic AT commands.
  *          in case of success the string "OK" is returned inside otherwise
  *	         it is an error.
  * @param   None
  * @retval  EMW3080_OK on sucess, EMW3080_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_Init(void)
{
  EMW3080_StatusTypeDef Ret = EMW3080_ERROR;
  
  if(EMW3080_IO_Init() == 0){
    Ret = EMW3080_OK;
  }
  
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_EchoOFF(void)
{
  EMW3080_StatusTypeDef Ret;
  /* Disable the Echo mode */
  /* Construct the command */
  memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+UARTE=OFF%c", '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_FlashLockToggle(uint8_t flag)
{
  EMW3080_StatusTypeDef Ret;

  /* Construct the command */
  memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
  if(flag==1)
    sprintf((char *)AtCmd, "AT+FLASHLOCK=ON%c", '\r');
  else
    sprintf((char *)AtCmd, "AT+FLASHLOCK=OFF%c", '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  return Ret;
}
/**
  * @brief   Deinitialize the EMW3080 module.
  * @details Restarts the module  and stop the IO. AT command can't be executed
             unless the module is reinitialized.
  * @param   None
  * @retval  EMW3080_OK on sucess, EMW3080_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_DeInit(void)
{
  EMW3080_StatusTypeDef Ret;

  /* Construct the command */
  sprintf((char *)AtCmd, "AT+REBOOT%c", '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  /* Free resources used by the module */
  EMW3080_IO_DeInit();
  
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_Reset2FactoryDefault(void)
{
    EMW3080_StatusTypeDef Ret;
    memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
    sprintf((char *)AtCmd, "AT+FACTORY%c", '\r');
  
    /* Send the command */
    Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
    
    return Ret;
}
/**
  * @brief  Restarts the EMW3080 module.
  * @param  None
  * @retval EMW3080_OK on sucess, EMW3080_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_Restart(void)
{
  EMW3080_StatusTypeDef Ret;

  /* Construct the command */
  memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+REBOOT%c", '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_WIFI_GetMACAddress(uint8_t  *mac)
{
  EMW3080_StatusTypeDef Ret;
  char *Token;
  
    /* Construct the command */
  memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+WMAC?%c", '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  if ( Ret == EMW3080_OK)
  {
    /* The MAC address is returned in the format
     ' +WMAC:<mac>,ok ', look for the token "+WMAC:",then get the mac value*/
    Token = strstr((char *)RxBuffer, "+WMAC:");
    Token+=6;
 
    /* Get the MAC address value */
    uint8_t i=0;
    uint32_t value=0;
    do{
      if((i%2==0)&&(i!=0))
      {
       *mac = value;
       value = 0;
       mac++;
      }
     if(*Token<='9')
    	 //convert char to number by *Token - '0'(ASCII code to number)
        value = value*16 + *Token-'0';//'0'=0x30
      else
        value = value*16 + *Token-'A'+10;//'A'=0x41
     Token++;
     i++;
    }while(i<=12);
  }
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_GetVersionInfo(uint8_t* fwver_string)
{
  EMW3080_StatusTypeDef Ret;
  char *Token,*temp;
  /* Construct the command */
  memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+FWVER?%c", '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  if ( Ret == EMW3080_OK)
  {
    /* ' +FWVER:<fwver>,ok'
      look for the token "STAIP," , then read the ip address located 
      between two double quotes */
    Token = strstr((char *)RxBuffer, "+FWVER:");
    Token+=7;
    
    temp = strstr(Token, ",");
    *temp = '\0';
 
    /* Get the IP address value */
    strcpy((char *)fwver_string, Token);
  }
 return Ret;
}
  
/**
  * @brief  Join an Access point.
  * @param  Ssid: the access point id.
  * @param  Password the Access point password.
  * @retval Returns EMW3080_AT_COMMAND_OK on success and EMW3080_AT_COMMAND_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_JoinAccessPoint(uint8_t* Ssid, uint8_t* Password)
{
  EMW3080_StatusTypeDef Ret;

    /* Setup the module in Station Mode*/
  
  /* Construct the command */
  memset (AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+WJAP=%s,%s%c",Ssid,Password, '\r');
  
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  if(Ret== EMW3080_OK)
  {
    if(EMW3080_WaitForEvent("STATION_UP\r\n")!=EMW3080_OK)
      return EMW3080_ERROR;
  }
  
  return Ret;
}

/**
  * @brief  Quit an Access point if any.
  * @param  None
  * @retval returns EMW3080_AT_COMMAND_OK on success and EMW3080_AT_COMMAND_ERROR otherwise.
  */
//EMW3080_StatusTypeDef EMW3080_QuitAccessPoint(void)
//{
//  EMW3080_StatusTypeDef Ret;
//  
//  /* Construct the command */
//  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
//  sprintf((char *)AtCmd, "AT+CWQAP%c%c", '\r', '\n');
//
//  /* Send the command */
//  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING);
//  
//  return Ret;
//}

EMW3080_StatusTypeDef EMW3080_CheckAPConnectionStatus(void)
{
  EMW3080_StatusTypeDef Ret;
  
  /* Construct the command */
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+WJAPS%c", '\r');

  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)"STATION_UP\r\nOK\r\n", DEFAULT_TIME_OUT);  
  
  return Ret; 
}

/**
  * @brief  Get the IP address for the EMW3080 in Station mode.
  * @param  Mode: a EMW3080_ModeTypeDef to choose the Station or AccessPoint mode.
                 only the Station Mode is supported.
  * @param  IpAddress buffer to contain the IP address.
  * @retval Returns EMW3080_OK on success and EMW3080_ERROR otherwise
  */
EMW3080_StatusTypeDef EMW3080_GetIPAddress(EMW3080_ModeTypeDef Mode, uint8_t* IpAddress)
{
  EMW3080_StatusTypeDef Ret = EMW3080_OK;
  char *Token; 
  
  /* Initialize the IP address and command fields */
  strcpy((char *)IpAddress, "0.0.0.0");
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  
  /* Construct the command */
  sprintf((char *)AtCmd, "AT+WJAPIP?%c", '\r');

  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  /* If EMW3080_OK is returned it means the IP Adress inside the RxBuffer 
     has already been read */
  if ( Ret == EMW3080_OK)
  {
    /* The IpAddress for the Station Mode is returned in the format
     ' STAIP,"ip_address" '
      look for the token "STAIP," , then read the ip address located 
      between two double quotes */
    Token = strstr((char *)RxBuffer, "+WJAPIP:");
    Token+=8;
 
    /* Get the IP address value */
    ParseNumberFromString((uint8_t*)Token,(uint8_t*)IpAddress,".", ",");

  }
  
  return Ret;
}
 
/**
  * @brief  Get IP address from URL using DNS.
  * @param  URL: Host URL.
  * @param  IpAddress buffer to contain the IP address.
  * @retval Returns EMW3080_OK on success and EMW3080_ERROR otherwise
  */
EMW3080_StatusTypeDef EMW3080_GetHostAddress(const char* url, uint8_t* IpAddress)
{
  EMW3080_StatusTypeDef Ret = EMW3080_OK;
  char *Token, *temp;
  
  /* Initialize the IP address and command fields */
  strcpy((char *)IpAddress, "0.0.0.0");
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  
  /* Construct the command */
  sprintf((char *)AtCmd, "AT+CIPDOMAIN=%s%c", url,'\r');

  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  /* If EMW3080_OK is returned it means the IP Adress inside the RxBuffer 
     has already been read */
  if ( Ret == EMW3080_OK)
  {
    /* The IpAddress for the Station Mode is returned in the format
    ' +CIPDOMAIN:"ip_address" '
      look for the token "+CIPDOMAIN:" , then read the ip address located 
      between two double quotes */
    Token = strstr((char *)RxBuffer, "+CIPDOMAIN:");
    Token+=14;
    
    temp = strstr(Token, "\r");
    *temp = '\0';
 
    /* Get the IP address value */
    strcpy((char *)IpAddress, Token);
  }
  
  return Ret;
}



/**
  * @brief  Establish a network connection.
  * @param  Connection_info a pointer to a EMW3080_ConnectionInfoTypeDef struct containing the connection info.
  * @retval Returns EMW3080_AT_COMMAND_OK on success and EMW3080_AT_COMMAND_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_EstablishConnection(const EMW3080_ConnectionInfoTypeDef* connection_info)
{
  EMW3080_StatusTypeDef Ret;
  
  /* Check the connection mode */
  if (connection_info->isServer)
  {
    /* Server mode not supported for this version yet */
    return EMW3080_ERROR;
  }
  
  /* Construct the CIPSTART command */
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+CIPSTART=%lu,tcp_client,%s,%lu%c", connection_info->connectionID,(char *)connection_info->ipAddress, connection_info->port,'\r');
  
  /* Send the command */  
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
  
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_IsConnectedWithServer(uint8_t channel_id)
{
  EMW3080_StatusTypeDef Ret;
  
  /* Construct the command */
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+CIPSTATUS=%lu%c",channel_id, '\r');

  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);  
  
  if(Ret== EMW3080_OK)
  {
    if (strstr((char *)RxBuffer, "connected") == NULL)
    {
      return EMW3080_ERROR;
    }
    
  }
  
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_IsDisconnectedFromServer(uint8_t channel_id)
{
  EMW3080_StatusTypeDef Ret;
  
  /* Construct the  command */
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+CIPSTATUS=%lu%c",channel_id, '\r');

  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);  
  
  if(Ret== EMW3080_OK)
  {
    if (strstr((char *)RxBuffer, "close") == NULL)
    {
      return EMW3080_ERROR;
    }
    
  }
  
  return Ret;
}
/**
  * @brief   Close a network connection.
  * @details Use the ALL_CONNECTION_ID to close all connections.
  * @param   Channel_id the channel ID of the connection to close.
  * @retval  Returns EMW3080_AT_COMMAND_OK on success and EMW3080_AT_COMMAND_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_CloseConnection(uint8_t channel_id)
{
  EMW3080_StatusTypeDef Ret;
  
  /* Construct the command */
  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)AtCmd, "AT+CIPSTOP=%lu%c",channel_id, '\r');
          
  /* Send the command */
  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);

  
  return Ret;
}

//EMW3080_StatusTypeDef EMW3080_CloseAutoConnection(uint8_t channel_id)
//{
//  EMW3080_StatusTypeDef Ret;
//
//  /* Construct the command */
//  memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
//  sprintf((char *)AtCmd, "AT+CIPAUTOCONN=%lu,%c",channel_id, '0');
//          
//  /* Send the command */
//  Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_OK_STRING, DEFAULT_TIME_OUT);
//  
//  return Ret;
//}

/**
  * @brief  Send data over the wifi connection.
  * @param  Buffer: the buffer to send
  * @param  Length: the Buffer's data size.
  * @retval Returns EMW3080_OK on success and EMW3080_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_SendData(uint8_t socket,uint8_t* Buffer, uint32_t Length, uint32_t Timeout)
{
  EMW3080_StatusTypeDef Ret = EMW3080_OK;
  
  if (Buffer != NULL)
  {
//    uint32_t tickStart;
	
    /* Construct the  command */
    memset(AtCmd, '\0', MAX_AT_CMD_SIZE);
    sprintf((char *)AtCmd, "AT+CIPSEND=%lu,%lu%c", socket,Length, '\r');
    
    /* The command doesn't have a return command
       until the data is actually sent. Thus we check here whether
       we got the '>' prompt or not. */
    Ret = runAtCmd(AtCmd, strlen((char *)AtCmd), (uint8_t*)AT_SEND_PROMPT_STRING, Timeout);
  
    /* Return Error */
    if (Ret != EMW3080_OK)
    {
      return EMW3080_ERROR;
    }
  
    /* Send the data */
    Ret = runAtCmd(Buffer, Length, (uint8_t*)AT_OK_STRING, Timeout);
  }
  
  return Ret;
}


/**
  * @brief  receive data over the wifi connection.
  * @param  pData the buffer to fill will the received data.
  * @param  Length the maximum data size to receive.
  * @param RetLength the actual data received.
  * @retval returns EMW3080_OK on success and EMW3080_ERROR otherwise.
  */
EMW3080_StatusTypeDef EMW3080_ReceiveData(uint8_t socket,uint8_t* pData, uint32_t Length, uint32_t* RetLength, uint32_t Timeout)
{
  EMW3080_StatusTypeDef Ret;
  uint32_t already_read_length = 0;//count how many data has been read out from socks buffer
  Ret = EMW3080_OK;
  uint32_t unread_length,len=0;  
  sock_buffer_ctx_t* socks;
  
  unread_length = Length;
  already_read_length = 0;
  
  //get the corresponding socks struct,which will store the data read from WiFiRxBuffer
  socks = Get_Socks_Buffer(socket);
  
   while(unread_length!=0)
  {
    //try to get data from the socket buffer first
    //if no data in socket buffer,then get data from WiFiRxBuffer
    if(socks->left != 0 && socks->pdata !=NULL)
    {
      //calculate the length that can be read from socket buffer
      len = (socks->left > unread_length)? unread_length: socks->left;
      
      memcpy((pData+already_read_length),(socks->pdata+socks->offset),len);
      socks->left -=len; //update how many data still left in socks buffer
      socks->offset+=len;//update the socks buffer's offset
      already_read_length+=len; //update how many data is already read
      unread_length-=len;//update unread length

      if(socks->left==0)
      {
        socks->offset = 0;
      }
    }
    else
    {
      //read more data from WiFiRxBuffer
        Ret = getData(socket,RetLength, Timeout);
        if(*RetLength ==0)
        {
          break;
          //return EMW3080_ERROR; //@sz MQTTYield will try to read data continually,so ignore this situation
        }
    }

  }
    
  *RetLength = already_read_length;
  return Ret;
}

EMW3080_StatusTypeDef EMW3080_WaitForEvent(const uint8_t* Token1)
{
  EMW3080_StatusTypeDef Ret;
  uint32_t idx = 0;
  uint8_t RxChar;
  
  Ret = EMW3080_ERROR;
  
  /* Reset the Rx buffer to make sure no previous data exist */
  memset(RxBuffer, '\0', MAX_RX_SIZE);
  
  while (1)
  {
	/* Wait to recieve data */
    if (EMW3080_IO_Receive(&RxChar, 1, DEFAULT_TIME_OUT) != 0)
    {
      RxBuffer[idx++] = RxChar; 

      //printf("Received %s", (const char *)RxChar);
    }
    else
    {
      break;
    }
    
	/* Check that max buffer size has not been reached */
    if (idx == MAX_RX_SIZE)
    {
      break;
    }

	/* Extract the Token */
    if (strstr((char *)RxBuffer, (char *)Token1) != NULL)
    {
        return EMW3080_OK;      
    }
  }  
  return Ret;
}

/**
  * @brief  Run the AT command
  * @param  cmd the buffer to fill will the received data.
  * @param  Length the maximum data size to receive.
  * @param  Token the expected output if command runs successfully
  * @retval Returns EMW3080_OK on success and EMW3080_ERROR otherwise.
  */
static EMW3080_StatusTypeDef runAtCmd(uint8_t* cmd, uint32_t Length, const uint8_t* Token, uint32_t Timeout)
{
  uint32_t idx = 0;
  uint8_t RxChar;
  uint32_t RetLength ;
  
  /* Reset the Rx buffer to make sure no previous data exist */
  memset(RxBuffer, '\0', MAX_RX_SIZE);

  /* Send the command */
  if (EMW3080_IO_Send(cmd, Length) < 0)
  {

    return EMW3080_ERROR;
  }

  /* Wait for reception */
  while (1)
  {	
	/* Wait to recieve data */
    if (EMW3080_IO_Receive(&RxChar, 1, Timeout) != 0)
    {
      RxBuffer[idx++] = RxChar; 
      
      	/* Check that max buffer size has not been reached */
      if (idx == MAX_RX_SIZE)
      {
        printf("reached the end of RX Buffer\n");
        break;
      }
          /* Extract the Token */
      if (strstr((char *)RxBuffer, (char *)Token) != NULL)
      {        
        return EMW3080_OK;
      }
      
//          /* Extract the Token */
//      if (strstr((char *)RxBuffer, (char *)AT_OK_STRING) != NULL)
//      {
//        return EMW3080_OK;
//      }
      
          /* Check if the message contains error code */
      if (strstr((char *)RxBuffer, AT_ERROR_STRING) != NULL)
      {
        return EMW3080_ERROR;
      }
      
      if (strstr((char *)RxBuffer, EVENT_DATA_IN) != NULL) 
      {
        if(EMW3080_OK == pullSocketData(0xff, Timeout, &RetLength))/*0xff indicates socket number is from stream*/
        {
          memset(RxBuffer, '\0', MAX_RX_SIZE);
          idx = 0;
          
          continue;
        }
        else
        {
          printf("Error in reading event,only get %d data\n",RetLength);        
          //can't readout enough data
          break;
        }
      }
    }
    else
    {
     break;
    }   
  }
  return EMW3080_ERROR;
}

static EMW3080_StatusTypeDef pullSocketData(uint8_t socket, uint32_t timeout,uint32_t* RetLength)
{
  uint32_t   i = 0;
  sock_buffer_ctx_t* socks;
  uint8_t RxChar;
#if defined __GNUC__
  uint8_t LengthString[5];
#else
  /* need one more char for End of String indication */	
  uint8_t LengthString[5];
#endif
  uint32_t LengthValue;

   *RetLength  = 0;
#if defined __GNUC__
   memset(LengthString, '\0', 5);
#else
   /* initialize with one more End of String char */
   memset(LengthString, '\0', 5);
#endif
  
  //check the socket number
  uint8_t number=0;
  EMW3080_IO_Receive(&RxChar, 1, timeout);
  while(RxChar != ',')
  {
    number = RxChar-'0';
    EMW3080_IO_Receive(&RxChar, 1, timeout);
  }

  //get the corresponding socks struct,which will store the data read from WiFiRxBuffer
  socks = Get_Socks_Buffer(number);

  if(socket != 0xff && number != socket)
  {
    printf("socket number is not match\n");
    return EMW3080_ERROR;
  }  
      
  //read the length string
  EMW3080_IO_Receive(&RxChar, 1, timeout);
  while(RxChar != ',')
  {
    LengthString[i++] = RxChar;
    EMW3080_IO_Receive(&RxChar, 1, timeout);
  }

  /* Get the buffer length */
#if defined __GNUC__
  LengthValue = atoi((char *)LengthString);;
#else
  LengthValue = atoi((char *)LengthString);
#endif
  if(LengthValue > (MAX_SOCKET_SIZE-socks->left))
  {
    printf("no enough socket memory buffer to store data\n");
    return EMW3080_ERROR;
  }
 
  //start to read data
  if(socks->pdata==NULL)
  {
    printf("fail to malloc memory to socks\n");
    return EMW3080_ERROR;
  }      
  
  *RetLength = EMW3080_IO_Receive(socks->pdata + socks->offset + socks->left,LengthValue, timeout);  
  socks->left += *RetLength;  
  if(*RetLength != LengthValue)
  {
    printf("can't get enough data,only get %d data\n",socks->left);        
    //can't readout enough data
    return EMW3080_ERROR;
  }

  EMW3080_IO_Receive(&RxChar, 1, timeout); /*Disarcd 0x0D*/
  EMW3080_IO_Receive(&RxChar, 1, timeout); /*Discard 0x0A*/
  
  return EMW3080_OK;
   
}

/**
  * @brief  Receive data from the WiFi module
  * @param  Buffer The buffer where to fill the received data
  * @param  Length the maximum data size to receive.
  * @param  RetLength Length of received data
  * @retval Returns EMW3080_OK on success and EMW3080_ERROR otherwise.
  */
static EMW3080_StatusTypeDef getData(uint8_t socket,uint32_t* RetLength, uint32_t Timeout)
{
  uint8_t RxChar;
  uint32_t idx = 0;
  uint32_t LengthValue;
 
  EMW3080_Boolean newChunk  = EMW3080_FALSE;
  sock_buffer_ctx_t* socks;
  
  //get the corresponding socks struct,which will store the data read from WiFiRxBuffer
  socks = Get_Socks_Buffer(socket);
  
  /* Reset the reception data length */
  *RetLength = 0;

  /* Reset the reception buffer */  
  memset(RxBuffer, '\0', MAX_RX_SIZE);
  
  /* When reading data over a wifi connection the EMW3080*/
  while (1)
  {
    if (EMW3080_IO_Receive(&RxChar, 1, Timeout) != 0)
    {
      /* Check the data chunk starts with +CIPEVENT:SOCKET, */
      RxBuffer[idx++] = RxChar;
    }
    else
    {
     /* Errors while reading return an error. */
      if ((newChunk == EMW3080_TRUE) && (LengthValue != 0))
      {
        return EMW3080_ERROR;
      }
      else
      {
        break;
      }
    }

	
    /* When a new chunk is met, extact the socket number and its size */
    if ((strstr((char *)RxBuffer, EVENT_DATA_IN) != NULL) && (newChunk == EMW3080_FALSE)) 
    {
      if(EMW3080_OK == pullSocketData(socket, Timeout, RetLength))
      {
        newChunk = EMW3080_FALSE;
        memset(RxBuffer, '\0', MAX_RX_SIZE);
        idx = 0;
        break;
      }
      else
      {
        printf("can't get enough data,only get %d data\n",socks->left);        
        //can't readout enough data
        return EMW3080_ERROR;
      }
    }

	/* Check if message contains error code */
    if (strstr((char *)RxBuffer, AT_ERROR_STRING) != NULL)
    {
      return EMW3080_ERROR;
    }

  }
  return EMW3080_OK;
}

uint32_t GetAvailableDataNum(RingBuffer_t buffer)
{
  uint32_t len=0;
  
  if(buffer.tail >= buffer.head)
  {
    len = buffer.tail - buffer.head;
  }
  else
  {
    len = RING_BUFFER_SIZE - buffer.head + buffer.tail;
  }
  
  return len;
}

void ParseNumberFromString(uint8_t* string, uint8_t* number_p, uint8_t* separate_mark, uint8_t* end_mark)
{
     static uint32_t value=0;
    while((*string)!=*end_mark)
    {
       value = value*10 + (*string-'0');
       string++;
     if(*string==*separate_mark)
     {
       *(number_p) = value;
       number_p++;
       string++;
       value=0;
     }
    };
    *(number_p) = value;
}
void Socks_Buffer_Init(void)
{
  uint8_t i=0;
  
  for(i=0;i<5;i++)
  {
    socks_buffer[i].socket = i;
#if defined __GNUC__
    socks_buffer[i].pdata = (uint8_t *)NULL;
#else
    socks_buffer[i].pdata = NULL;
#endif
    socks_buffer[i].offset = 0;
    socks_buffer[i].left = 0;
  }
}

void Free_Socks_Buffer(uint8_t socket)
{
  if(socks_buffer[socket].pdata!=NULL)
  {
    free(socks_buffer[socket].pdata);
    socks_buffer[socket].pdata = NULL;
    socks_buffer[socket].offset = 0;
    socks_buffer[socket].left = 0;    
  }
}

int Malloc_Socks_Buffer(uint8_t socket)
{
  int ret=0;
  if(socks_buffer[socket].pdata==NULL)
  {
    socks_buffer[socket].pdata = (uint8_t*)malloc(MAX_SOCKET_SIZE); /*allocate once; not free in the whole power cycle*/
    if(socks_buffer[socket].pdata==NULL)
    {
      printf("fail to malloc memory to socks\n");
      ret=-1;
    }
  }
  return ret;
}

sock_buffer_ctx_t* Get_Socks_Buffer(uint8_t socket)
{
   if(socket >=4)
#if defined __GNUC__
     return (sock_buffer_ctx_t *)0;
#else
   	 return NULL;
#endif
 
  return &(socks_buffer[socket]);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
