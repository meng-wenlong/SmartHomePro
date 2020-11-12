package com.example.device.admin.service.impl;

import com.alibaba.fastjson.JSON;
import com.aliyuncs.DefaultAcsClient;
import com.aliyuncs.exceptions.ClientException;
import com.aliyuncs.http.FormatType;
import com.aliyuncs.iot.model.v20180120.*;
import com.aliyuncs.profile.DefaultProfile;
import com.aliyuncs.profile.IClientProfile;
import com.example.device.admin.service.AliyunDeviceService;
import org.apache.commons.codec.binary.Base64;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.PostConstruct;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class AliyunDeviceServiceImpl implements AliyunDeviceService{
    private static final Logger LOGGER = LoggerFactory.getLogger(AliyunDeviceService.class);
    //@Value("${iot.regionId}")是加载application.properties配置文件对象的值
    @Value("${iot.regionId}")
    private  String regionId;

    @Value("${iot.domain}")
    private  String domain;

    @Value("${iot.productCode}")
    private  String productCode;


    @Value("${user.accessKeyID}")
    private  String accessKeyID;

    @Value("${user.accessKeySecret}")
    private  String accessKeySecret;

    @Value("${iot.productKey}")
    private String productKey;


    DefaultAcsClient client = null;
    //阿里云物联网平台SDK已经封装了API使用方法，只需调用方法即可。首先初始化SDK，被@PostConstruct修饰的方法会在初始化的时候运行，并且只会被服务器调用一次.
    @PostConstruct
    public DefaultAcsClient getClient() {

        try {
            IClientProfile profile = DefaultProfile.getProfile(regionId, accessKeyID, accessKeySecret);
            DefaultProfile.addEndpoint(regionId, regionId, productCode,
                    domain);
            // 初始化client
            client = new DefaultAcsClient(profile);

        } catch (Exception e) {
            LOGGER.error("初始化client失败！exception:" + e.getMessage());
        }
        return client;
    }

    @Override
    public List<QueryDevicePropertyStatusResponse.Data.PropertyStatusInfo> queryDevicePro(String deviceName) throws ClientException {
        QueryDevicePropertyStatusRequest queryDevicePropRequest=new QueryDevicePropertyStatusRequest();
        queryDevicePropRequest.setDeviceName(deviceName);
        queryDevicePropRequest.setProductKey(productKey);
        queryDevicePropRequest.setAcceptFormat(FormatType.JSON);

        List<QueryDevicePropertyStatusResponse.Data.PropertyStatusInfo> list=new ArrayList<>();
        QueryDevicePropertyStatusResponse acsResponse = client.getAcsResponse(queryDevicePropRequest);
        if(acsResponse != null && acsResponse.getSuccess() != false){
            QueryDevicePropertyStatusResponse.Data data = acsResponse.getData();
            if(data!=null){
                list = data.getList();
            }
        }
        return list;
    }

    @Override
    public  Boolean setDeviceProperty(String deviceName, String name, String value) throws ClientException {
        SetDevicePropertyRequest request=new SetDevicePropertyRequest();
        request.setDeviceName(deviceName);
        request.setProductKey(productKey);
        request.setAcceptFormat(FormatType.JSON);

        Map map =new HashMap();
        map.put(name,value);
        String s = JSON.toJSONString(map);
        String s1 = s.replaceAll("\"", "");
        request.setItems(s1);
        SetDevicePropertyResponse acsResponse = client.getAcsResponse(request);
        if(acsResponse != null){
            return acsResponse.getSuccess();
        }else {
            return false;
        }
    }

    @Override
    public Boolean invokeThingService(String deviceName, String identifier, String args) throws ClientException {
        InvokeThingServiceRequest request = new InvokeThingServiceRequest();
        request.setProductKey(productKey);
        request.setDeviceName(deviceName);
        request.setIdentifier(identifier);
        request.setArgs(args);
        request.setAcceptFormat(FormatType.JSON);
        InvokeThingServiceResponse acsResponse = client.getAcsResponse(request);
        if(acsResponse != null){
            return acsResponse.getSuccess();
        }else {
            return false;
        }
    }


    @Override
    public  Boolean pub(String deviceName, String identifier, String msg) throws ClientException {
        String topic ="/"+productKey+"/"+deviceName+"/user/"+identifier;
        PubRequest request = new PubRequest();
        request.setProductKey(productKey);
        request.setTopicFullName(topic);
        request.setMessageContent(Base64.encodeBase64String(msg.getBytes()));
        request.setAcceptFormat(FormatType.JSON);
        request.setQos(1);
        PubResponse acsResponse =client.getAcsResponse(request);
        if(acsResponse!=null){
            return acsResponse.getSuccess();
        }
        return false;
    }



}
