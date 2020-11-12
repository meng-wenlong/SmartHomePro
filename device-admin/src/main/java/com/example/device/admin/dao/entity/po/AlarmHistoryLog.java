package com.example.device.admin.dao.entity.po;



import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import java.util.Date;

public class AlarmHistoryLog {

    @Id
    @GeneratedValue(generator = "JDBC")
    private Integer id;

    private String deviceName;

    private Float alarmTemperature;

    private Float tempThreshold;

    private Date gmtCreate;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName == null ? null : deviceName.trim();
    }

    public Float getAlarmTemperature() {
        return alarmTemperature;
    }

    public void setAlarmTemperature(Float alarmTemperature) {
        this.alarmTemperature = alarmTemperature;
    }

    public Float getTempThreshold() {
        return tempThreshold;
    }

    public void setTempThreshold(Float tempThreshold) {
        this.tempThreshold = tempThreshold;
    }

    public Date getGmtCreate() {
        return gmtCreate;
    }

    public void setGmtCreate(Date gmtCreate) {
        this.gmtCreate = gmtCreate;
    }
}