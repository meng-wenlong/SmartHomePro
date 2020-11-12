
#ifndef __ALI_IOTCLIENT_H
#define __ALI_IOTCLIENT_H

extern int doMqttYield(void);
extern int connect2Aliiothub(void);
extern int deviceSubscribe(void);
extern int deviceStatusPub(void);
extern int rebuildMQTTConnection(void);
extern void mqttDisconnect(void);
extern int deviceAlarmPub(void);
extern int build_mqtt_topic(void);
#endif //__Ali_IOTCLIENT_H