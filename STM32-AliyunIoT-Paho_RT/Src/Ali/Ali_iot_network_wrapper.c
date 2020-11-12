#include "main.h"
#include "Ali_iot_network_wrapper.h"

int mqtt_connect_network(Network* n, const char * host_address, int port)
{
  int rc =0;
 
  if( (rc = net_sock_create(hnet, (net_sockhnd_t *)&n->my_socket, NET_PROTO_TCP)) != NET_OK )
  {
    msg_error(" failed to create a TCP socket  ! net_sock_create %d\n", rc);
    return rc;
  }
  

  if( (rc = net_sock_setopt((void*)n->my_socket, "sock_blocking", NULL, 0)) != NET_OK )//@sz change to "sock_blocking"
   {
     msg_error(" failed to set the TCP socket noblocking ! net_sock_setopt %d\n", rc);
     goto exit;
   }
  
  if((rc = net_sock_open((void*)n->my_socket, host_address, port)) != NET_OK)
  {
     msg_error(" failed to open the TCP socket ! net_sock_open %d\n", rc);
   }

exit:
   if(rc!=NET_OK)
   {
     net_sock_destroy((void*)n->my_socket);//@sz,free net_sock_ctxt_t which malloced in net_sock_create_tcp_wifi
   }
   return rc;
}

int mqtt_socket_send(Network *network, unsigned char *buf, int len, int timeout)
{
  int rc;
  int socket = (int) (network->my_socket);
  
  rc = net_sock_send((void*)socket,buf,len);
  if(NET_TIMEOUT==rc) //@sz ignore timeout error
    rc= NET_OK;
  
  return rc;
}

int mqtt_socket_recv(Network *network, unsigned char *buf, int len, int timeout)
{
    int rc;
  int socket = (int) (network->my_socket);
  
  rc = net_sock_recv((void*)socket,buf,len);
  if(NET_TIMEOUT==rc) //@sz ignore timeout error
    rc= NET_OK;
  return rc;
}


int mqtt_socket_disconnect(Network *network)
{
   int rc = NET_OK;
   
   if(network->my_socket!=NULL)
   {
      rc = net_sock_close((void*)network->my_socket);
      rc = net_sock_destroy((void*)network->my_socket);
   }
   if (rc != NET_OK)
   {
      msg_error("net_sock_close() or net_sock_destroy() failed.\n");
   }
   return rc;
}


//@sz, new a MQTT network interface with WIFI module
//must called before baidu_connect_network_tls/baidu_connect_network
int mqtt_network_new(Network *network)
{
  network->my_socket = 0;
  network->mqttread = mqtt_socket_recv;
  network->mqttwrite = mqtt_socket_send;
  network->disconnect = mqtt_socket_disconnect;

  return SUCCESS;
}



