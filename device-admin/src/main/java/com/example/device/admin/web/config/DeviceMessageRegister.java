package com.example.device.admin.web.config;

import com.alibaba.fastjson.JSON;
import com.aliyun.openservices.iot.api.Profile;
import com.aliyun.openservices.iot.api.message.MessageClientFactory;
import com.aliyun.openservices.iot.api.message.api.MessageClient;
import com.aliyun.openservices.iot.api.message.callback.MessageCallback;
import com.aliyun.openservices.iot.api.message.entity.Message;
import com.aliyun.openservices.iot.api.message.entity.MessageToken;
import com.aliyuncs.exceptions.ClientException;
import com.example.device.admin.dao.entity.bo.DeviceEvent;
import com.example.device.admin.dao.entity.bo.DeviceLifecyclePost;
import com.example.device.admin.dao.entity.bo.DevicePropertyPost;
import com.example.device.admin.dao.entity.bo.DeviceStatusPost;
import com.example.device.admin.service.AlarmHistoryLogService;
import com.example.device.admin.service.DeviceInfoService;
import com.example.device.admin.service.DevicePropHistoryLogService;
import com.example.device.admin.web.util.AliyunMessageTransformUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.InitializingBean;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;

import javax.annotation.Resource;

/**
 * @Configuration 配置类
 * @Value 将application.propertites配置文件的属性读出来
 * InitializingBean 在spring初始化bean的时候，如果bean实现了InitializingBean接口，会自动调用afterPropertiesSet方法。
 *
 * */
@Configuration
public class DeviceMessageRegister implements InitializingBean {
    @Value("${user.accessKeyID}")
    private String accessKey;

    @Value("${user.accessKeySecret}")
    private String accessSecret;

    @Value("${iot.regionId}")
    private String regionId;

    @Value("${user.uid}")
    private String uid;

    @Value("${iot.productKey}")
    private String productKey;

    @Autowired
    private Topic topic;




    @Autowired  //自动装配
    private DeviceInfoService deviceInfoService;

    @Autowired
    private DevicePropHistoryLogService devicePropHistoryLogService;

    @Autowired
    private AlarmHistoryLogService alarmHistoryLogService;

    private Logger logger = LoggerFactory.getLogger(getClass());



    @Override
    public void afterPropertiesSet() throws Exception {
        // endPoint:  https://${uid}.iot-as-http2.${region}.aliyuncs.com
        String endPoint = "https://" + uid + ".iot-as-http2." + regionId + ".aliyuncs.com";

        // 连接配置
        Profile profile = Profile.getAccessKeyProfile(endPoint, regionId, accessKey, accessSecret);

        // 构造客户端
        MessageClient client = MessageClientFactory.messageClient(profile);
        client.setMessageListener(new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {
                return null;
            }
        });


        //属性上报  topic:   /productKey/#/thing/event/property/post
        client.setMessageListener(topic.getDevicePropertyPostTopic(), new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {  //设置消息接收接口
                Message m = messageToken.getMessage();
                logger.info("receive 属性上报" + new String(messageToken.getMessage().getPayload()));
                DevicePropertyPost devicePropertyPost = JSON.parseObject(new String(m.getPayload()), DevicePropertyPost.class);
                Boolean isUpdateDeviceProperty = deviceInfoService.updateOrInsertDeviceProperty(devicePropertyPost);
                Boolean isUpdateDevicePropHistoryLog = devicePropHistoryLogService.insertDevicePropHistoryLog(devicePropertyPost);
                if (isUpdateDeviceProperty && isUpdateDevicePropHistoryLog) {
                    return Action.CommitSuccess;
                }
                return Action.CommitFailure; }});


        //设备状态变化上报
        client.setMessageListener(topic.getDeviceStatusTopic(), new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {
                Message m = messageToken.getMessage();
                logger.info("receive 设备状态变化上报" + new String(messageToken.getMessage().getPayload()));

                DeviceStatusPost deviceStatus = JSON.parseObject(new String(m.getPayload()), DeviceStatusPost.class);

                String status = deviceStatus.getStatus();
                String deviceName = deviceStatus.getDeviceName();

                if (status == null || deviceName == null) {
                    return Action.CommitFailure;
                }

                boolean b = deviceInfoService.updateStatus(deviceName, status);

                if (b) {
                    return Action.CommitSuccess;
                }
                return Action.CommitFailure;

            }
        });


        //设备事件上报
        client.setMessageListener(topic.getDeviceEventPostTopic(), new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {
                Message m = messageToken.getMessage();
                logger.info("receive 设备事件上报" + new String(messageToken.getMessage().getPayload()));
                DeviceEvent deviceEvent = JSON.parseObject(new String(m.getPayload()), DeviceEvent.class);
                Boolean isInsertAlarmHistoryLog = null;
                Boolean isUpdateAlarmStatus = null;
                try {
                    isInsertAlarmHistoryLog = alarmHistoryLogService.insertAlarmHistoryLogByAliyunQuery(deviceEvent);
                    isUpdateAlarmStatus = deviceInfoService.updateAlarmStatus(deviceEvent.getDeviceName(), true);
                } catch (ClientException e) {
                    e.printStackTrace();
                }
                if (isInsertAlarmHistoryLog && isUpdateAlarmStatus) {
                    return Action.CommitSuccess;
                }
                return Action.CommitFailure; }});


        //设备生命周期上报 {payload={"iotId":"DszqFgXkdL0fv6TGaSI70010f4b200","action":"delete","messageCreateTime":1547196768873,
        // "productKey":"a1I64MeQmoo","deviceName":"test_zhinengjiaju"}
        client.setMessageListener(topic.getDeviceLifecyclePostTopic(), new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {
                Message m = messageToken.getMessage();
                logger.info("receive 设备生命周期上报" + new String(messageToken.getMessage().getPayload()));
                DeviceLifecyclePost deviceLifecycle = JSON.parseObject(new String(m.getPayload()), DeviceLifecyclePost.class);
                Boolean b = deviceInfoService.insertOrDelete(deviceLifecycle);
                if (b) {
                    return Action.CommitSuccess;
                }
                return Action.CommitFailure;

            }
        });

        client.setMessageListener(topic.getTempHumUploadTopic(), new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {
                Message m = messageToken.getMessage();
                logger.info("receive 设备透传方式上报温湿度及光照强度" + new String(messageToken.getMessage().getPayload()));
                //收到数据以后转化格式
                DevicePropertyPost devicePropertyPost = AliyunMessageTransformUtils.getDevicePropertyPost(m, productKey);
                //更新设备信息表
                Boolean isUpdateDeviceProperty = deviceInfoService.updateOrInsertDeviceProperty(devicePropertyPost);
                //更新设备历史温湿度及光强表
                Boolean isUpdateDevicePropHistoryLog = devicePropHistoryLogService.insertDevicePropHistoryLog(devicePropertyPost);
                if (isUpdateDeviceProperty && isUpdateDevicePropHistoryLog) {
                    return Action.CommitSuccess;
                }
                return Action.CommitFailure; }});


        client.setMessageListener(topic.getTempAlarmTopic(), new MessageCallback() {
            @Override
            public Action consume(MessageToken messageToken) {
                Message m = messageToken.getMessage();
                logger.info("receive 透传方式设备报警" + new String(messageToken.getMessage().getPayload()));
                DeviceEvent deviceEvent = AliyunMessageTransformUtils.getDeviceEvent(m, productKey);
                Boolean isInsertAlarmHistoryLog = null;
                Boolean isUpdateAlarmStatus = null;
                isInsertAlarmHistoryLog = alarmHistoryLogService.insertAlarmHistoryLogByMysqlQuery(deviceEvent);
                isUpdateAlarmStatus = deviceInfoService.updateAlarmStatus(deviceEvent.getDeviceName(), true);
                if (isInsertAlarmHistoryLog && isUpdateAlarmStatus) {
                    return Action.CommitSuccess;
                }
                return Action.CommitFailure;
            }
        });


        // 数据接收
        client.connect(messageToken -> {
            Message m = messageToken.getMessage();
            byte[] payload = m.getPayload();
            System.out.println("receive message from " + m);
            System.out.println("m.getTopic() = " + m.getTopic());
            return MessageCallback.Action.CommitSuccess;
        });


    }
}
