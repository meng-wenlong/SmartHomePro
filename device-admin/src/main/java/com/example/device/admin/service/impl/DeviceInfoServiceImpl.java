package com.example.device.admin.service.impl;


import com.aliyuncs.CommonRequest;
import com.aliyuncs.iot.model.v20180120.BatchAddDeviceGroupRelationsRequest;
import com.example.device.admin.dao.entity.bo.DeviceLifecyclePost;
import com.example.device.admin.dao.entity.bo.DevicePropertyPost;
import com.example.device.admin.dao.entity.po.DeviceInfo;

import com.example.device.admin.dao.mapper.DeviceInfoMapper;
import com.example.device.admin.service.DeviceInfoService;
import com.example.device.admin.service.constant.DeviceStatusCode;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import tk.mybatis.mapper.entity.Example;

import java.util.Date;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

@Service
public class DeviceInfoServiceImpl implements DeviceInfoService {
    @Autowired
    private DeviceInfoMapper deviceInfoMapper;

    @Override
    public boolean updateStatus(String deviceName, String status) {
        String deviceStatus = DeviceStatusCode.getDeviceStatusCode(status);

        DeviceInfo deviceInfo=new DeviceInfo();
        deviceInfo.setConnectStatus(deviceStatus);
        deviceInfo.setGmtUpdate(new Date());
        deviceInfo.setLastOnlineDate(new Date());
        deviceInfo.setDeviceName(deviceName);
        int i = deviceInfoMapper.insertOrUpdateStatus(deviceInfo);
        if(i!=1){
            return false;
        }else {
            return true;
        }
    }

    @Override
    public DeviceInfo getDeviceInfoByDeviceName(String deviceName) {
        DeviceInfo deviceInfo=new DeviceInfo();
        deviceInfo.setDeviceName(deviceName);
        DeviceInfo deviceInfo1 = deviceInfoMapper.selectOne(deviceInfo);
        return deviceInfo1;
    }

    @Override
    public Set<String> listDeviceNames() {
        List<DeviceInfo> deviceInfos = deviceInfoMapper.selectAll();
        Set<String> deviceNames = deviceInfos.stream().map(DeviceInfo::getDeviceName).collect(Collectors.toSet());
        return deviceNames;
    }

    @Override   /*修改*/
    public Boolean updateOrInsertDeviceProperty(DevicePropertyPost devicePropertyPost) {
        DeviceInfo deviceInfo=new DeviceInfo();
        deviceInfo.setDeviceName(devicePropertyPost.getDeviceName());
        deviceInfo.setGmtUpdate(devicePropertyPost.getGmtCreate());
        if (devicePropertyPost.getItems() != null) {
            if(devicePropertyPost.getItems().getCurrentHumidity()!=null){
                deviceInfo.setCurrentHumidity(devicePropertyPost.getItems().getCurrentHumidity().getValue());
            }
            if(devicePropertyPost.getItems().getCurrentTemperature()!=null){
                deviceInfo.setCurrentTemperature(devicePropertyPost.getItems().getCurrentTemperature().getValue());
            }
            if(devicePropertyPost.getItems().getCurrentLight()!=null){      /*修改*/
                deviceInfo.setCurrentLight(devicePropertyPost.getItems().getCurrentLight().getValue());
            }
            if(devicePropertyPost.getItems().getTempThreshold()!=null){
                deviceInfo.setTempThreshold(devicePropertyPost.getItems().getTempThreshold().getValue());
            }
            if(devicePropertyPost.getItems().getLightThreshold()!=null){    /*修改：光强阈值*/
                deviceInfo.setLightThreshold(devicePropertyPost.getItems().getLightThreshold().getValue());
            }
            int i = deviceInfoMapper.insertOrUpdateDeviceProperty(deviceInfo);  /*更新数据库*/
            if(i>0){
                return true;
            }
        }
        return false;
    }

    @Override
    public Boolean insertOrDelete(DeviceLifecyclePost deviceLifecycle) {
        int i=0;
        if("create".equals(deviceLifecycle.getAction())){
            DeviceInfo deviceInfo=new DeviceInfo();
            deviceInfo.setDeviceName(deviceLifecycle.getDeviceName());
            deviceInfo.setGmtUpdate(deviceLifecycle.getMessageCreateTime());
            i=deviceInfoMapper.insert(deviceInfo);

        }else if("delete".equals(deviceLifecycle.getAction())){
            DeviceInfo deviceInfo=new DeviceInfo();
            deviceInfo.setDeviceName(deviceLifecycle.getDeviceName());
            i=deviceInfoMapper.delete(deviceInfo);
        }else {
            return true;
        }
        if(i!=1){
            return false;
        }else {
            return true;
        }

    }

    @Override
    public Boolean updateAlarmStatus(String deviceName, boolean b) {
        DeviceInfo deviceInfo=new DeviceInfo();
        deviceInfo.setAlarmStatus(b);
        deviceInfo.setGmtUpdate(new Date());
        if(b){
            deviceInfo.setLastAlarmDate(new Date());
        }
        Example example=new Example(DeviceInfo.class);
        Example.Criteria criteria = example.createCriteria();
        criteria.andEqualTo("deviceName",deviceName);
        int i = deviceInfoMapper.updateByExampleSelective(deviceInfo, example);
        if(i==1){
            return true;
        }
        return false;
    }

    @Override
    public void updateTempThreshold(String deviceName, Float tempThreshold) {
        DeviceInfo deviceInfo=new DeviceInfo();
        deviceInfo.setTempThreshold(tempThreshold);
        deviceInfo.setGmtUpdate(new Date());

        Example example=new Example(DeviceInfo.class);
        Example.Criteria criteria = example.createCriteria();
        criteria.andEqualTo("deviceName",deviceName);
        int i = deviceInfoMapper.updateByExampleSelective(deviceInfo, example);
    }

    @Override
    public void updateLightThreshold(String deviceName, Float lightThreshold) {
        DeviceInfo deviceInfo=new DeviceInfo();
        deviceInfo.setLightThreshold(lightThreshold);
        deviceInfo.setGmtUpdate(new Date());

        Example example=new Example(DeviceInfo.class);
        Example.Criteria criteria = example.createCriteria();
        criteria.andEqualTo("deviceName",deviceName);
        int i = deviceInfoMapper.updateByExampleSelective(deviceInfo, example);
    }

}
