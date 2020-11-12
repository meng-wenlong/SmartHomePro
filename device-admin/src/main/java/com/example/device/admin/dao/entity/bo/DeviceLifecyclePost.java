package com.example.device.admin.dao.entity.bo;

import java.util.Date;

public class DeviceLifecyclePost {
    private String action;
    private String iotId;
    private String productKey;
    private String deviceName;
    private String deviceSecret;
    private Date messageCreateTime;

    public String getAction() {
        return action;
    }

    public String getIotId() {
        return iotId;
    }

    public String getProductKey() {
        return productKey;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public String getDeviceSecret() {
        return deviceSecret;
    }

    public Date getMessageCreateTime() {
        return messageCreateTime;
    }

    public void setAction(String action) {
        this.action = action;
    }

    public void setIotId(String iotId) {
        this.iotId = iotId;
    }

    public void setProductKey(String productKey) {
        this.productKey = productKey;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public void setDeviceSecret(String deviceSecret) {
        this.deviceSecret = deviceSecret;
    }

    public void setMessageCreateTime(Date messageCreateTime) {
        this.messageCreateTime = messageCreateTime;
    }
}
