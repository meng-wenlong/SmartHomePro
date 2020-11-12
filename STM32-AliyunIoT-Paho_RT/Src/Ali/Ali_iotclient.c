/*
This file implement the Ali IoT Client Activities
*/
#include <string.h>
#include "main.h"
#include "Ali_iotclient.h"
#include "msg.h"
#include "Ali_iot_network_wrapper.h"
#include "MQTTClient.h"
#include "iot_flash_config.h"
#include "mbedtls/md.h"

//MQTT configue
#define MQTT_3_1_1                      4
#define MQTT_BUFFER_SIZE                512
#define MQTT_YIELD_DELAY                200//ms
#define MQTT_CLIENT_INFO_SIZE           100

#define COMMAND_TIMEOUT_MS 5000 //ms，it will used to wait for suback，puback
#define CONNECT_MAX_ATTEMPT_COUNT 3

#define HOST_PORT  1883//without tls

#define APP_PAYLOAD_LENGTH  7   /*修改*/

/*这两代码可以完美地将printf()重定向到rt_kprintf()*/
extern void rt_kprintf(const char *fmt, ...);
#define printf(...) rt_kprintf(__VA_ARGS__)

Network sNetwork;
MQTTClient Client;
unsigned char MQTT_read_buf[MQTT_BUFFER_SIZE];
unsigned char MQTT_write_buf[MQTT_BUFFER_SIZE];
int8_t payload_buf[APP_PAYLOAD_LENGTH];//one byte for temperature, one byte for humidity,one byte for temperature threshold
/*上面原本是int8*//*修改*/
char mqtt_host_address[MQTT_CLIENT_INFO_SIZE];
char mqtt_client_id[MQTT_CLIENT_INFO_SIZE];
char mqtt_client_username[MQTT_CLIENT_INFO_SIZE];
char mqtt_client_password[MQTT_CLIENT_INFO_SIZE];
char threshold_topic[MQTT_CLIENT_INFO_SIZE];
char light_threshold_topic[MQTT_CLIENT_INFO_SIZE];      /*接收光强阈值的修改*/
char clearAlarm_topic[MQTT_CLIENT_INFO_SIZE];
char tempAlarm_topic[MQTT_CLIENT_INFO_SIZE];
char temp_hum_topic[MQTT_CLIENT_INFO_SIZE];

int build_mqtt_topic(void);
int connect2MQTTServer(void);
int get_mqtt_password(char* password,char* product_key, char* device_name, uint32_t timestamp, uint8_t* clientid);
int get_mqtt_username(char* user_name, char* device_name, char* product_key);
int get_mqtt_client_id(char* mqtt_client_id, uint8_t* clientid, uint32_t timestamp);
int get_mqtt_server_addr(char* host_addr, char* region_id, char* product_key );
int32_t Mbedtls_SHA1_HMAC_Compute(uint8_t* InputMessage,
                                uint32_t InputMessageLength,
                                uint8_t *HMAC_key,
                                uint32_t HMAC_keyLength,
                                uint8_t *MessageDigest,
                                int32_t* MessageDigestLength);
extern int PrepareMqttPayload(char * PayloadBuffer, int PayloadSize);
extern void Parameters_message_handler(MessageData * data);
extern void Parameters_message_handler_2(MessageData * data);
extern void Service_message_handler(MessageData * data);
extern uint8_t dat[2];
void calpassword(void);
int get_mqtt_server_addr(char* host_addr, char* region_id, char* product_key )
{
  //${YourProductKey}.iot-as-mqtt.${YourRegionId}.aliyuncs.com:1883
  uint32_t return_len=0;
  

  return_len = snprintf(host_addr,MQTT_CLIENT_INFO_SIZE,"%s.iot-as-mqtt.%s.aliyuncs.com",product_key,region_id);
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for host address\n");
    return -1;
  }
  msg_info("MQTT server address is :%s\n",host_addr);
  return 0;
}

int build_mqtt_topic(void)
{
  uint32_t return_len=0;
  char* product_key_temp;
  char* device_name_temp;
  
  if(getProductKey(&product_key_temp)==HAL_ERROR)
  {
    msg_info("getProductKey failed\n");
    return -1;
  }
  if(getDeviceName(&device_name_temp)==HAL_ERROR)
  {
    msg_info("getDeviceName failed\n");
    return -1;
  }
  
  memset(tempAlarm_topic,0,sizeof(tempAlarm_topic));
  return_len = snprintf(tempAlarm_topic,MQTT_CLIENT_INFO_SIZE,"/%s/%s/user/tempAlarm",product_key_temp,device_name_temp);  
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for tempAlarm_topic\n");
    return -1;
  }
  
  memset(clearAlarm_topic,0,sizeof(clearAlarm_topic));
  return_len = snprintf(clearAlarm_topic,MQTT_CLIENT_INFO_SIZE,"/%s/%s/user/clearAlarm",product_key_temp,device_name_temp);  
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for clearAlarm_topic\n");
    return -1;
  }
  
  memset(threshold_topic,0,sizeof(threshold_topic));
  return_len = snprintf(threshold_topic,MQTT_CLIENT_INFO_SIZE,"/%s/%s/user/tempThresholdSet",product_key_temp,device_name_temp);  
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for threshold_topic\n");
    return -1;
  }
  
  /*接收光强阈值的修改*/
  memset(light_threshold_topic,0,sizeof(light_threshold_topic));
  return_len = snprintf(light_threshold_topic,MQTT_CLIENT_INFO_SIZE,"/%s/%s/user/lightThresholdSet",product_key_temp,device_name_temp);  
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for light_threshold_topic\n");
    return -1;
  }
  
  memset(temp_hum_topic,0,sizeof(temp_hum_topic));
  return_len = snprintf(temp_hum_topic,MQTT_CLIENT_INFO_SIZE,"/%s/%s/user/tempHumUpload",product_key_temp,device_name_temp);  
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for temp_hum_topic\n");
    return -1;
  }
  return 0;
}

int get_mqtt_client_id(char* mqtt_client_id, uint8_t* clientid, uint32_t timestamp)
{
  //mqttClientId: clientId+"|securemode=3,signmethod=hmacsha1,timestamp=132323232|"
  uint32_t return_len=0;
  
  return_len = snprintf(mqtt_client_id,MQTT_CLIENT_INFO_SIZE,"%s|securemode=3,signmethod=hmacsha1,timestamp=%d|",clientid,timestamp);
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for Client ID\n");
    return -1;
  }
  msg_info("MQTT Client ID is :%s\n",mqtt_client_id);
  return 0;
}

int get_mqtt_username(char* user_name, char* device_name, char* product_key)
{
  //mqttUsername: deviceName+"&"+productKey
    uint32_t return_len=0;
  
  return_len = snprintf(user_name,MQTT_CLIENT_INFO_SIZE,"%s&%s",device_name,product_key);
  if(return_len >= MQTT_CLIENT_INFO_SIZE)
  {
    msg_info("no enough space for Username\n");
    return -1;
  }
  msg_info("MQTT Username is :%s\n",user_name);
  return 0;
}

int get_mqtt_password(char* password,char* product_key, char* device_name, uint32_t timestamp, uint8_t* clientid)
{
  //mqttPassword: sign_hmac(deviceSecret,content)

  char content[256];
  char digest[20];
  char sig_hmac[44]= {'\0'};
  char * tempsecret;
  int32_t password_len;
  uint32_t return_len=0;
  uint32_t i=0;
  
  memset(content,0,sizeof(content));
  memset(digest,0,sizeof(digest));
  return_len = snprintf(content,sizeof(content),"clientId%sdeviceName%sproductKey%stimestamp%d",
                        clientid,device_name,product_key,timestamp);
  if(return_len >= sizeof(content))
  {
    msg_info("no enough space for content\n");
    return -1;
  }
  if(getDeviceSecret(&tempsecret)==HAL_ERROR)
  {
    msg_info("getDeviceSecret failed\n");
    return -1;
  }  
  msg_info("content:%s\n",content);
  msg_info("key: %s\n",tempsecret);
  //HMAC SHA1
  //transform to HEX string
  Mbedtls_SHA1_HMAC_Compute((uint8_t*)content,strlen(content),(uint8_t*)tempsecret,strlen(tempsecret),(uint8_t*)digest,&password_len);
  for(i=0;i<20;i++)
    sprintf(&sig_hmac[i*2],"%02x",(unsigned int)digest[i]);

  memcpy(password,sig_hmac,44);
  msg_info("MQTT password generated successfully:%s\n",&password[0]);
  return 0;
}


int connect2Aliiothub(void)
{
  int ret;
  uint8_t connectCounter = 0;
  char * tempRegionIdString;
  char * tempProductKeyString;
  
  /*get host address*/
  ret = getRegionId(&tempRegionIdString);
  if(ret==HAL_ERROR)
  {
    msg_info("getRegionId failed\n");
    return -1;
  }
    
  if(getProductKey(&tempProductKeyString)==HAL_ERROR)
  {
    msg_info("getProductKey failed\n");
    return -1;
  }
  if(get_mqtt_server_addr(mqtt_host_address,tempRegionIdString,tempProductKeyString)== -1)
  {
    msg_info("get_mqtt_server_addr failed\n");
    return -1;    
  }
  msg_info("\n*** Start connecting to MQTT server ***\n");
  msg_info("Server address: %s : %d\n\n",mqtt_host_address,HOST_PORT);
  
  /*process connect*/
  do
  {
    connectCounter ++;
    msg_info("TCP Connection in progress:   Attempt %d/%d ...\n",connectCounter,CONNECT_MAX_ATTEMPT_COUNT);
    
    //new a MQTT network interface
    mqtt_network_new(&sNetwork); //@sz
    
    //process TCP Connect

     if((ret = mqtt_connect_network(&sNetwork,(const char *)mqtt_host_address,HOST_PORT))== 0)
      {
        msg_info("\nconnected to server \n\n");
        msg_info("Processing MQTT Connection\n\n");    
        //process mqtt connect        
        if((ret = connect2MQTTServer()) != 0)
        {
          msg_error("MQTT Connection failed!\n");
          mqtt_socket_disconnect(&sNetwork); //@sz,20180228
        }
      }
      else
        msg_error(" failed to build TCP connection  ! socket_connect returned -0x%x\n",  -ret);
    
  }while((ret != 0)&&(connectCounter < CONNECT_MAX_ATTEMPT_COUNT));
  
  return ret;
}

int connect2MQTTServer(void)
{
  int ret;
  uint8_t count=0;
  net_macaddr_t mac_adr;
  uint8_t clientid[13];
  uint32_t timestamp;
  char* device_name_temp;
  char* product_key_temp;
  
  MQTTPacket_connectData Connect_para = MQTTPacket_connectData_initializer;
  
  ret = 0;
  MQTTClientInit(&Client,&sNetwork,COMMAND_TIMEOUT_MS,MQTT_write_buf,sizeof(MQTT_write_buf),MQTT_read_buf,sizeof(MQTT_read_buf));
  
  if(getProductKey(&product_key_temp)==HAL_ERROR)
  {
    msg_info("getProductKey failed\n");
    return -1;
  }
  if(getDeviceName(&device_name_temp)==HAL_ERROR)
  {
    msg_info("getDeviceName failed\n");
    return -1;
  }
  if(net_get_mac_address(hnet,&mac_adr)!=NET_OK)
  {
    msg_info("getDeviceName failed\n");
    return -1;
  }
  memset(clientid,0,sizeof(clientid));
  sprintf(clientid,"%x%x%x%x%x%x",mac_adr.mac[0],mac_adr.mac[1],mac_adr.mac[2],mac_adr.mac[3],mac_adr.mac[4],mac_adr.mac[5]);
  timestamp = HAL_GetTick();
  get_mqtt_client_id(mqtt_client_id,clientid,timestamp);
  get_mqtt_username(mqtt_client_username,device_name_temp, product_key_temp);
  get_mqtt_password(mqtt_client_password,product_key_temp,device_name_temp,timestamp,clientid);

  //Connect_para.MQTTVersion = MQTT_3_1_1;
  Connect_para.clientID.cstring = mqtt_client_id;
  Connect_para.username.cstring =  mqtt_client_username;
  Connect_para.password.cstring =  mqtt_client_password;

  //MQTT connect,will option is set in Connect_para
  do
  {
    count++;
    msg_info("MQTT Connection Attempt %d/%d ...\n",count,CONNECT_MAX_ATTEMPT_COUNT);
    if((ret = MQTTConnect(&Client, &Connect_para)) != 0)
    {
       msg_error("Client connection with Baidu MQTT Broker failed with error code: %d\n",ret);
    }
  }while((ret != 0)&&(count < CONNECT_MAX_ATTEMPT_COUNT));
  
  return ret;
}

int deviceStatusPub(void)
{
  int ret;
  MQTTMessage MQTT_msg;
  ret =0;
  
  MQTT_msg.qos = QOS1;//QoS1;
  MQTT_msg.dup = 0;//The DUP flag MUST be set to 1 by the Client or Server when it attempts to re-deliver a PUBLISH Packet
  //The DUP flag MUST be set to 0 for all QoS 0 messages
  MQTT_msg.retained = 1; //the Server MUST store 757 the Application Message and its QoS
  MQTT_msg.payload = payload_buf;
  
  //TODO:prepare pub payload,@sz
  payload_buf[0]= GetTemperatureValue();
  payload_buf[1]= GetHumValue(); 
  payload_buf[2]= GetTempratureThreshold();
  payload_buf[3]= dat[0];               /*修改*/
  payload_buf[4]= dat[1];               /*修改*/
  payload_buf[5]= GetLightThreshold()/100;      /*修改：光强阈值*/
  payload_buf[6]= GetLightThreshold()%100;      /*修改：光强阈值*/
  MQTT_msg.payloadlen = 7;              /*修改*/
  

  if((ret=MQTTPublish(&Client, temp_hum_topic,&MQTT_msg)) != 0)
  {
    msg_error("Failed to publish data. %d ",ret);
    msg_info(": temprature = %d, humidity = %d, light = %d\n\n",\
      GetTemperatureValue(),GetHumValue(),GetLightValue());       /*修改*/
  }
  else
  {

    msg_info("publish device status successfully ");
    msg_info(": temprature = %d, humidity = %d, light = %d\n\n",\
      GetTemperatureValue(),GetHumValue(),GetLightValue());     /*修改*/
  }
  return ret;
}

int deviceAlarmPub(void)
{
  //pub_device_status();
  int ret;
  MQTTMessage MQTT_msg;
  ret =0;
  
  MQTT_msg.qos = QOS1;//QoS1;
  MQTT_msg.dup = 0;//The DUP flag MUST be set to 1 by the Client or Server when it attempts to re-deliver a PUBLISH Packet
  //The DUP flag MUST be set to 0 for all QoS 0 messages
  MQTT_msg.retained = 1; //the Server MUST store 757 the Application Message and its QoS
  MQTT_msg.payload = payload_buf;
  
  //TODO:prepare pub payload,@sz      
  payload_buf[0]= 0x01;
  MQTT_msg.payloadlen = 1;
  
 
  if((ret=MQTTPublish(&Client, tempAlarm_topic,&MQTT_msg)) != 0)
  {
    msg_error("\nFailed to publish data. %d ",ret);
    msg_info(": temprature = %d\n\n",GetTemperatureValue());
  }
  else
  {

    msg_info("\npublish temperature alarm successfully ");
    msg_info(": temprature = %d\n\n",GetTemperatureValue());
  }
  return ret;
}


int deviceSubscribe(void)
{
  int ret;
  ret = 0;
  
  if((ret=MQTTSubscribe(&Client,threshold_topic,QOS0,Parameters_message_handler))!=0)
    printf("fail to subscribe to topic:%s\n",threshold_topic);
  else
    printf("subscribe to topic: %s\n",threshold_topic);
  
  if((ret=MQTTSubscribe(&Client,clearAlarm_topic,QOS0,Service_message_handler))!=0)
    printf("fail to subscribe to topic:%s\n",clearAlarm_topic);
  else
    printf("subscribe to topic: %s\n",clearAlarm_topic);
  
  /*接收光强阈值的修改*/
  if((ret=MQTTSubscribe(&Client,light_threshold_topic,QOS0,Parameters_message_handler_2))!=0)
    printf("fail to subscribe to topic:%s\n",light_threshold_topic);
  else
    printf("subscribe to topic: %s\n",light_threshold_topic);
  
  return ret;
}

int doMqttYield(void)
{
  return MQTTYield(&Client,MQTT_YIELD_DELAY);
}

int rebuildMQTTConnection(void)
{
  int ret=0;
     if(!MQTTIsConnected(&Client))
     {
       mqttDisconnect();
       printf("rebuild MQTT connection...\n");
       if(connect2Aliiothub()==0)
       {
         //subscribe to topic
         deviceSubscribe();  
       }
       else
         ret=-1;
     }
return ret;
}

void mqttDisconnect(void)
{
  if(Client.ipstack->my_socket!=0)
    MQTTDisconnect(&Client);
  mqtt_socket_disconnect(Client.ipstack);
}

int32_t Mbedtls_SHA1_HMAC_Compute(uint8_t* InputMessage,
                                uint32_t InputMessageLength,
                                uint8_t *HMAC_key,
                                uint32_t HMAC_keyLength,
                                uint8_t *MessageDigest,
                                int32_t* MessageDigestLength)
{
  mbedtls_md_context_t sha1_ctx;
  uint32_t ret = 0;

  mbedtls_md_init(&sha1_ctx);

  ret = mbedtls_md_setup(&sha1_ctx,mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),1);

  /* check for initialization errors */
  if (ret == 0)
  {
    /* Add data to be hashed */
    if((ret=mbedtls_md_hmac_starts(&sha1_ctx,HMAC_key,HMAC_keyLength))!=0)
      goto exit;
    if((ret=mbedtls_md_hmac_update(&sha1_ctx,InputMessage,InputMessageLength))!=0)
      goto exit;
    if((ret=mbedtls_md_hmac_finish(&sha1_ctx,MessageDigest))!=0)
      goto exit;
    *MessageDigestLength = strlen(MessageDigest);
  }

exit:
  mbedtls_md_free(&sha1_ctx);
  return ret;
}
