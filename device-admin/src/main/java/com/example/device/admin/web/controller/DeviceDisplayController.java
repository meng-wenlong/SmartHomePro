package com.example.device.admin.web.controller;


import com.aliyuncs.exceptions.ClientException;
import com.example.device.admin.common.constant.result.BaseResp;
import com.example.device.admin.common.constant.result.ResultStatus;
import com.example.device.admin.dao.entity.po.AlarmHistoryLog;
import com.example.device.admin.dao.entity.po.DeviceInfo;
import com.example.device.admin.dao.entity.po.DevicePropHistoryLog;
import com.example.device.admin.service.AlarmHistoryLogService;
import com.example.device.admin.service.AliyunDeviceService;
import com.example.device.admin.service.DeviceInfoService;
import com.example.device.admin.service.DevicePropHistoryLogService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.bind.annotation.CrossOrigin;

import javax.annotation.Resource;
import java.util.*;


@Controller
@CrossOrigin
public class DeviceDisplayController {

    @Autowired
    public DeviceInfoService deviceInfoService;

    @Autowired
    public DevicePropHistoryLogService devicePropHistoryLogService;

    @Autowired
    public AlarmHistoryLogService alarmHistoryLogService;


    @Autowired
    public AliyunDeviceService aliyunDeviceService;


    //获取设备属性(当前温湿度及报警阀值及设备在线状态)
    @RequestMapping(value = "/api/v1/device/queryDeviceProp",method = RequestMethod.GET)
    public @ResponseBody  BaseResp<DeviceInfo> queryDeviceProp(String deviceName) {
        //System.out.println(deviceInfoService.getClass().toString());
        DeviceInfo deviceInfo=deviceInfoService.getDeviceInfoByDeviceName(deviceName);
        return new BaseResp<>(ResultStatus.SUCCESS,deviceInfo);
    }

    //查询所有设备名
    @RequestMapping(value = "/api/v1/device/listDeviceName",method = RequestMethod.GET)
    public @ResponseBody  BaseResp listDeviceNames() {
        Set<String> deviceNames=deviceInfoService.listDeviceNames();
        return new BaseResp<>(ResultStatus.SUCCESS,deviceNames);
    }



    //查询一段时间温湿度数据 deviceName,startTime,endTime请求参数  @DateTimeFormat配置请求参数日期访问格式
    //调用devicePropHistoryLogService.listDevicePropHistoryLogs方法获取数据
    @RequestMapping(value = "/api/v1/device/queryDevicePropHistoryLogs",method = RequestMethod.GET)
    public @ResponseBody BaseResp queryDevicePropHistoryLogs(String deviceName,
                                                             @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss")Date startTime,
                                                             @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss")Date endTime) {
        List<DevicePropHistoryLog> devicePropHistoryLogs=devicePropHistoryLogService.listDevicePropHistoryLogs(deviceName,startTime,endTime);
        return new BaseResp<>(ResultStatus.SUCCESS,devicePropHistoryLogs);
    }



    //查询一段时间温湿度数据
    @RequestMapping(value = "/api/v1/device/queryAlarmHistoryLogs",method = RequestMethod.GET)
    public @ResponseBody BaseResp queryAlarmHistoryLogs(String deviceName, @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss")
            Date startTime,@DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") Date endTime) {
        List<AlarmHistoryLog> alarmHistoryLogs=alarmHistoryLogService.listAlarmHistoryLogs(deviceName,startTime,endTime);
        return new BaseResp<>(ResultStatus.SUCCESS,alarmHistoryLogs);
    }


/*   //Alink方式修改设备报警阀值，如果是高级版，请启用此段代码 如果是基础版，请注释此段代码
    @RequestMapping(value = "/api/v1/device/setDeviceProperty",method = RequestMethod.GET)
    public @ResponseBody BaseResp setDeviceProperty(String deviceName, Integer tempThreshold) {
        Boolean b = null;
        try {
            b = aliyunDeviceService.setDeviceProperty(deviceName, "TempThreshold", tempThreshold.toString());
        } catch (ClientException e) {
            e.printStackTrace();
        }
        Map<String,Boolean> map=new HashMap();
        map.put("isSuccess",b);
        return new BaseResp<>(ResultStatus.SUCCESS,map); }
*/

    //透传方式修改设备报警阀值 如果是基础版，请启用此段代码 如果是高级版，请注释此段代码
    @RequestMapping(value = "/api/v1/device/setDeviceProperty",method = RequestMethod.GET)
    public @ResponseBody BaseResp setDeviceProperty(String deviceName, Float tempThreshold) {
        Boolean b = null;
        try {
            b = aliyunDeviceService.pub(deviceName, "tempThresholdSet",
                    Character.toString((char)tempThreshold.floatValue()));
            if(b){
                deviceInfoService.updateTempThreshold(deviceName,tempThreshold);
            }
        } catch (ClientException e) {
            e.printStackTrace();
        }
        Map<String,Boolean> map=new HashMap();
        map.put("isSuccess",b);
        return new BaseResp<>(ResultStatus.SUCCESS,map); }


    @RequestMapping(value = "/api/v1/device/setDeviceLightProperty", method = RequestMethod.GET)
    public @ResponseBody BaseResp setDeviceLightProperty(String deviceName, Float lightThreshold) {
        Boolean b = null;
        try {
            b = aliyunDeviceService.pub(deviceName, "lightThresholdSet",
                    Character.toString((char)lightThreshold.floatValue()));
            if(b){
                deviceInfoService.updateLightThreshold(deviceName,lightThreshold);
            }
        } catch (ClientException e) {
            e.printStackTrace();
        }
        Map<String,Boolean> map=new HashMap();
        map.put("isSuccess",b);
        return new BaseResp<>(ResultStatus.SUCCESS,map);
    }


/*
   //Alink方式设备报警状态取消 如果是高级版，请启用此段代码 如果是基础版，请注释此段代码
   @RequestMapping(value = "/api/v1/device/clearAlarm",method = RequestMethod.GET)
    public @ResponseBody BaseResp clearAlarm(String deviceName) {
        Boolean b = null;
        try {
            b = aliyunDeviceService.invokeThingService(deviceName, "ClearAlarm", "{}");
            if(b){
                deviceInfoService.updateAlarmStatus(deviceName,false);
            }
        } catch (ClientException e) {
            e.printStackTrace();
        }
        Map<String,Boolean> map=new HashMap();
        map.put("isSuccess",b);
        return new BaseResp<>(ResultStatus.SUCCESS,map);}
    */

    //透传方式设备报警状态取消 如果是基础版，请启用此段代码 如果是高级版，请注释此段代码
    @RequestMapping(value = "/api/v1/device/clearAlarm",method = RequestMethod.GET)
    public @ResponseBody BaseResp clearAlarm(String deviceName) {
        Boolean b = null;
        try {
            b = aliyunDeviceService.pub(deviceName, "clearAlarm", Character.toString((char)1));
            if(b){
                deviceInfoService.updateAlarmStatus(deviceName,false);
            }
        } catch (ClientException e) {
            e.printStackTrace();
        }
        Map<String,Boolean> map=new HashMap();
        map.put("isSuccess",b);
        return new BaseResp<>(ResultStatus.SUCCESS,map); }


}
