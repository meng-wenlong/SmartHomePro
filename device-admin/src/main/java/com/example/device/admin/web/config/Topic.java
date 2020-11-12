package com.example.device.admin.web.config;


import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

/**
 * User: Administrator
 * Date: 2018/9/20
 * Time: 14:28
 * Version:V1.0
 */
@Component
public class Topic {
    @Value("${iot.productKey}")
    private String productKey;


    // 设备状态上报  /as/mqtt/status/{productKey}/#
    public String getDeviceStatusTopic(){
        return new StringBuffer().append("/as/mqtt/status/").append(productKey).append("/#").toString();
    }

    //  高级版设备属性上报 sys/{productKey}/{deviceName}/thing/event/property/post
    public String getDevicePropertyPostTopic(){
        return new StringBuffer().append("/").append(productKey).append("/#").append("/thing/event/property/post").toString();
    }

    // 高级版设备事件上报 /sys/{productKey}/${deviceName}/thing/event/${tsl.event.identifer}/post
    public String getDeviceEventPostTopic(){
        return new StringBuffer().append("/").append(productKey).append("/#").append("/thing/event").append("/#").toString();
    }

    // 设备设备生命周期上报 /{productKey}/${deviceName}/thing/lifecycle
    public String getDeviceLifecyclePostTopic(){
        return new StringBuffer().append("/").append(productKey).append("/#").append("/thing/lifecycle").toString();
    }

    //基础版温湿度上报 /{productKey}/#/tempHumUpload
    public String getTempHumUploadTopic() {
        return new StringBuffer().append("/").append(productKey).append("/#").append("/user/tempHumUpload").toString();
    }
    //基础版温度报警上报  /{productKey}/#/tempAlarm
    public String getTempAlarmTopic() {
        return new StringBuffer().append("/").append(productKey).append("/#").append("/user/tempAlarm").toString();
    }


}
