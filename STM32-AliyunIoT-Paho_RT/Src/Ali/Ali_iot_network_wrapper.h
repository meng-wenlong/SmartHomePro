#ifndef __ALI_IOT_NET_WRAPPER_C_
#define __ALI_IOT_NET_WRAPPER_C_

#include "net.h"
#include "paho_mqtt_platform.h"

extern int mqtt_connect_network(Network* n, const char * host_address, int port);
extern int mqtt_socket_send(Network *network, unsigned char *buf, int len, int timeout);
extern int mqtt_socket_recv(Network *network, unsigned char *buf, int len, int timeout);
//extern int mqtt_socket_close(Network *network);
extern int mqtt_socket_disconnect(Network *network);
extern int mqtt_network_new(Network *network);


#endif //__ALI_IOT_NET_WRAPPER_C_