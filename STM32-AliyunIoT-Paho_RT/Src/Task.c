#include "Task.h"
#include "Ali_iotclient.h"
#include <rtthread.h>

static rt_thread_t MQTT_Receive_thread;
void MQTT_Receive_entry(void *parameter);

void TaskInit(void)
{
    MQTT_Receive_thread = rt_thread_create("MQTT_Receive",
                         MQTT_Receive_entry,
                         RT_NULL,
                         1024,
                         4,
                         10);
    if(MQTT_Receive_thread != RT_NULL)
    {
        rt_thread_startup(MQTT_Receive_thread);
    }

//    rt_thread_t rt_thread_create(const char *name,
//                                 void (*entry)(void *parameter),
//                                 void       *parameter,
//                                 rt_uint32_t stack_size,
//                                 rt_uint8_t  priority,
//                                 rt_uint32_t tick);
}

void MQTT_Receive_entry(void *parameter)
{
    while (1)
    {
        doMqttYield();      //200ms（尝试接收数据）
        //rt_thread_mdelay(2000);
    }
}
