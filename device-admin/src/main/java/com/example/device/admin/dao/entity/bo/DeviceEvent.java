package com.example.device.admin.dao.entity.bo;

import java.util.Date;

public class DeviceEvent {
    private String deviceType;
    private String identifier;
    private String iotId;
    private String name;
    private String deviceName;
    private Date time;
    private String type;
    private String productKey;

    public String getDeviceType() {
        return deviceType;
    }

    public String getIdentifier() {
        return identifier;
    }

    public String getIotId() {
        return iotId;
    }

    public String getName() {
        return name;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public Date getTime() {
        return time;
    }

    public String getType() {
        return type;
    }

    public String getProductKey() {
        return productKey;
    }

    public void setDeviceType(String deviceType) {
        this.deviceType = deviceType;
    }

    public void setIdentifier(String identifier) {
        this.identifier = identifier;
    }

    public void setIotId(String iotId) {
        this.iotId = iotId;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public void setTime(Date time) {
        this.time = time;
    }

    public void setType(String type) {
        this.type = type;
    }

    public void setProductKey(String productKey) {
        this.productKey = productKey;
    }
}
