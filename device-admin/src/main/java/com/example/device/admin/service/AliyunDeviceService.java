package com.example.device.admin.service;

import com.aliyuncs.exceptions.ClientException;
import com.aliyuncs.iot.model.v20180120.QueryDevicePropertyStatusResponse;

import java.util.List;

public interface AliyunDeviceService {

    List<QueryDevicePropertyStatusResponse.Data.PropertyStatusInfo> queryDevicePro(String deviceName) throws ClientException;

    Boolean setDeviceProperty(String deviceName, String name, String value) throws ClientException;

    Boolean invokeThingService(String deviceName, String identifier, String args) throws ClientException;

    Boolean pub(String deviceName, String identifier, String msg) throws ClientException;
}
