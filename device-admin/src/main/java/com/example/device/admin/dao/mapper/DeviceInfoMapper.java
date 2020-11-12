package com.example.device.admin.dao.mapper;

/* 对DeviceInfo表的操作部 */

import com.example.device.admin.dao.entity.po.DeviceInfo;
import com.example.device.admin.dao.util.BaseMapper;


public interface DeviceInfoMapper extends BaseMapper<DeviceInfo> {

    int insertOrUpdateDeviceProperty(DeviceInfo deviceInfo);

    int insertOrUpdateStatus(DeviceInfo deviceInfo);
}