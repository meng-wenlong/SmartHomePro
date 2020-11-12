package com.example.device.admin.service.impl;


import com.example.device.admin.dao.entity.bo.DevicePropertyPost;
import com.example.device.admin.dao.entity.po.DevicePropHistoryLog;
import com.example.device.admin.dao.mapper.DevicePropHistoryLogMapper;
import com.example.device.admin.service.DevicePropHistoryLogService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import tk.mybatis.mapper.entity.Example;

import java.util.Date;
import java.util.List;

@Service
public class DevicePropHistoryLogServiceImpl implements DevicePropHistoryLogService {
    @Autowired
    private DevicePropHistoryLogMapper devicePropHistoryLogMapper;


    @Override
    public List<DevicePropHistoryLog> listDevicePropHistoryLogs(String deviceName, Date startTime, Date endTime) {
        Example devicePropHistoryLogExample=new Example(DevicePropHistoryLog.class);
        Example.Criteria criteria = devicePropHistoryLogExample.createCriteria();
        if (startTime != null ) {
            criteria.andGreaterThanOrEqualTo("gmtCreate",startTime);
        }

        if (endTime != null) {
            criteria.andLessThanOrEqualTo("gmtCreate",endTime);
        }
        criteria.andEqualTo("deviceName",deviceName);
        List<DevicePropHistoryLog> devicePropHistoryLogs = devicePropHistoryLogMapper.selectByExample(devicePropHistoryLogExample);
        return devicePropHistoryLogs;
    }


    @Override   /*修改*/
    public Boolean insertDevicePropHistoryLog(DevicePropertyPost devicePropertyPost) {
        DevicePropHistoryLog devicePropHistoryLog=new DevicePropHistoryLog();
        if (devicePropertyPost.getItems() != null) {    /*修改：当三个元素都接收为空时是正常状态*/
            if(devicePropertyPost.getItems().getCurrentTemperature()==null&&devicePropertyPost.getItems().getCurrentHumidity()==null&&devicePropertyPost.getItems().getCurrentLight()==null){
                return true;
            }
            if(devicePropertyPost.getItems().getCurrentHumidity()!=null){
                devicePropHistoryLog.setCurrentHumidity(devicePropertyPost.getItems().getCurrentHumidity().getValue());
            }
            if(devicePropertyPost.getItems().getCurrentTemperature()!=null){
                devicePropHistoryLog.setCurrentTemperature(devicePropertyPost.getItems().getCurrentTemperature().getValue());
            }
            if(devicePropertyPost.getItems().getCurrentLight()!=null){
                devicePropHistoryLog.setCurrentLight(devicePropertyPost.getItems().getCurrentLight().getValue());
            }

            devicePropHistoryLog.setGmtCreate(devicePropertyPost.getGmtCreate());
            devicePropHistoryLog.setDeviceName(devicePropertyPost.getDeviceName());

            int i = devicePropHistoryLogMapper.insert(devicePropHistoryLog);
            if(i!=1){
                return false;
            }
        }
        return true;
    }
}
