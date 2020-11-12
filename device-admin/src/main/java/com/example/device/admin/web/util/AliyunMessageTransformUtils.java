package com.example.device.admin.web.util;

import com.aliyun.openservices.iot.api.message.entity.Message;
import com.aliyuncs.iot.model.v20180120.BatchAddDeviceGroupRelationsRequest;
import com.example.device.admin.dao.entity.bo.DeviceEvent;
import com.example.device.admin.dao.entity.bo.DevicePropertyPost;
import org.apache.commons.lang3.StringUtils;

import java.util.Date;


public class AliyunMessageTransformUtils {

    public static DevicePropertyPost getDevicePropertyPost(Message m, String productKey) {
        DevicePropertyPost devicePropertyPost=new DevicePropertyPost();
        devicePropertyPost.setDeviceName(getDeviceName(m.getTopic(),productKey));

        byte[] payload = m.getPayload();
        //收到数据为byte数组，数组第一位为当前温度值
        DevicePropertyPost.Data data=new DevicePropertyPost.Data();
        DevicePropertyPost.CurrentTemperaturePropertyInfo currentTemperaturePropertyInfo=new  DevicePropertyPost.CurrentTemperaturePropertyInfo();
        currentTemperaturePropertyInfo.setValue((float)payload[0]);
        data.setCurrentTemperature(currentTemperaturePropertyInfo);

        //收到数据为byte数组，数组第二位为当前湿度值
        DevicePropertyPost.CurrentHumidityPropertyInfo currentHumidityPropertyInfo=new DevicePropertyPost.CurrentHumidityPropertyInfo();
        currentHumidityPropertyInfo.setValue((float)payload[1]);
        data.setCurrentHumidity(currentHumidityPropertyInfo);

        //收到数据为byte数组，数组第三位为温度阈值
        DevicePropertyPost.TempThresholdPropertyInfo tempThresholdPropertyInfo=new  DevicePropertyPost.TempThresholdPropertyInfo();
        tempThresholdPropertyInfo.setValue((float)payload[2]);
        data.setTempThreshold(tempThresholdPropertyInfo);

        /*修改：收到数据为byte数组，数组第四位、第五位为光照强度*/
        DevicePropertyPost.CurrentLightPropertyInfo currentLightPropertyInfo=new DevicePropertyPost.CurrentLightPropertyInfo();
        //组合光强数据
        int high = (payload[3] & 0x7f) * 256 + ((payload[3]>>>7) & 0x01) * 256 * 128;//高8位
        int low = (payload[4] & 0x7f) + ((payload[4]>>>7) & 0x01) * 128;//低8位
        int light = (int)((high + low) / 1.2);
        currentLightPropertyInfo.setValue((float)light);
        data.setCurrentLight(currentLightPropertyInfo);

        /*修改：收到数据为byte数组，数组第六位、第七位为光照强度阈值*/
        DevicePropertyPost.LightThresholdPropertyInfo lightThresholdPropertyInfo=new DevicePropertyPost.LightThresholdPropertyInfo();
        //组合光强阈值
        int light_threshold = payload[5]*100 + payload[6];
        lightThresholdPropertyInfo.setValue((float)light_threshold);
        data.setLightThreshold(lightThresholdPropertyInfo);
        devicePropertyPost.setItems(data);
        devicePropertyPost.setGmtCreate(new Date(m.getGenerateTime()));
        return devicePropertyPost;


    }

    public static String getDeviceName(String topic,String productKey) {
        String deviceName = StringUtils.substringBetween(topic, "/" + productKey + "/", "/");
        return deviceName;
    }

    public static DeviceEvent getDeviceEvent(Message m, String productKey) {
        DeviceEvent deviceEvent=new DeviceEvent();
        deviceEvent.setDeviceName(getDeviceName(m.getTopic(),productKey));
        deviceEvent.setTime(new Date(m.getGenerateTime()));
        return deviceEvent;
    }
}
