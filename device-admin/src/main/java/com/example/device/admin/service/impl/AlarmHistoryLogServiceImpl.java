package com.example.device.admin.service.impl;

import com.aliyuncs.exceptions.ClientException;
import com.aliyuncs.iot.model.v20180120.QueryDevicePropertyStatusResponse;
import com.example.device.admin.dao.entity.bo.DeviceEvent;
import com.example.device.admin.dao.entity.po.AlarmHistoryLog;
import com.example.device.admin.dao.entity.po.DeviceInfo;
import com.example.device.admin.dao.mapper.AlarmHistoryLogMapper;
import com.example.device.admin.service.AlarmHistoryLogService;
import com.example.device.admin.service.AliyunDeviceService;
import com.example.device.admin.service.DeviceInfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import tk.mybatis.mapper.entity.Example;

import java.util.Date;
import java.util.List;


@Service
public class AlarmHistoryLogServiceImpl implements AlarmHistoryLogService {
    @Autowired
    private AlarmHistoryLogMapper alarmHistoryLogMapper;

    @Autowired
    private AliyunDeviceService aliyunDeviceService;

    @Autowired
    private DeviceInfoService deviceInfoService;

    @Override
    public List<AlarmHistoryLog> listAlarmHistoryLogs(String deviceName, Date startTime, Date endTime) {
        Example example=new Example(AlarmHistoryLog.class);
        Example.Criteria criteria = example.createCriteria();
        if (startTime != null ) {
            criteria.andGreaterThanOrEqualTo("gmtCreate",startTime);
        }
        if (endTime != null) {
            criteria.andLessThanOrEqualTo("gmtCreate",endTime);
        }
        criteria.andEqualTo("deviceName",deviceName);
        List<AlarmHistoryLog> alarmHistoryLogs = alarmHistoryLogMapper.selectByExample(example);
        return alarmHistoryLogs;
    }


    @Override
    public Boolean insertAlarmHistoryLogByAliyunQuery(DeviceEvent deviceEvent) throws ClientException {
        AlarmHistoryLog alarmHistoryLog=new AlarmHistoryLog();
        List<QueryDevicePropertyStatusResponse.Data.PropertyStatusInfo> propertyStatusInfos = aliyunDeviceService.queryDevicePro(deviceEvent.getDeviceName());
        for (QueryDevicePropertyStatusResponse.Data.PropertyStatusInfo propertyStatusInfo: propertyStatusInfos) {
            if("CurrentTemperature".equals(propertyStatusInfo.getIdentifier())){
                alarmHistoryLog.setAlarmTemperature(Float.valueOf(propertyStatusInfo.getValue()));
            }

            if("TempThreshold".equals(propertyStatusInfo.getIdentifier())){
                alarmHistoryLog.setTempThreshold(Float.valueOf(propertyStatusInfo.getValue()));
            }
        }
        alarmHistoryLog.setDeviceName(deviceEvent.getDeviceName());
        alarmHistoryLog.setGmtCreate(deviceEvent.getTime());
        int i = alarmHistoryLogMapper.insert(alarmHistoryLog);
        if(i!=1){
            return false;
        }
        return true;
    }

    @Override
    public Boolean insertAlarmHistoryLogByMysqlQuery(DeviceEvent deviceEvent) {
        AlarmHistoryLog alarmHistoryLog=new AlarmHistoryLog();
        DeviceInfo deviceInfo= deviceInfoService.getDeviceInfoByDeviceName(deviceEvent.getDeviceName());
        if(deviceInfo==null){
            return false;
        }
        alarmHistoryLog.setGmtCreate(deviceEvent.getTime());
        if(deviceInfo.getTempThreshold()!=null){
            alarmHistoryLog.setTempThreshold(deviceInfo.getTempThreshold());
        }
        alarmHistoryLog.setAlarmTemperature(deviceInfo.getCurrentTemperature());
        alarmHistoryLog.setDeviceName(deviceEvent.getDeviceName());
        int i = alarmHistoryLogMapper.insert(alarmHistoryLog);
        if(i!=1){
            return false;
        }
        return true;
    }
}
