package com.example.device.admin.dao.entity.bo;



import java.awt.peer.LightweightPeer;
import java.util.Date;

public class DevicePropertyPost {
    private String deviceType;
    private String iotId;
    private String productKey;
    private Date gmtCreate;
    private String deviceName;
    private Data items;

    public String getDeviceType() { return deviceType; }

    public String getIotId() { return iotId; }

    public String getProductKey() { return productKey; }

    public Date getGmtCreate() {
        return gmtCreate;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public Data getItems() {
        return items;
    }

    public void setDeviceType(String deviceType) {
        this.deviceType = deviceType;
    }

    public void setIotId(String iotId) { this.iotId = iotId; }

    public void setProductKey(String productKey) {
        this.productKey = productKey;
    }

    public void setGmtCreate(Date gmtCreate) {
        this.gmtCreate = gmtCreate;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public void setItems(Data items) {
        this.items = items;
    }

    public static class Data {
        private CurrentHumidityPropertyInfo CurrentHumidity;
        private CurrentTemperaturePropertyInfo CurrentTemperature;
        private CurrentLightPropertyInfo CurrentLight;      /*修改*/
        private TempThresholdPropertyInfo TempThreshold;
        private LightThresholdPropertyInfo LightThreshold;  /*修改*/

        public CurrentHumidityPropertyInfo getCurrentHumidity() {
            return CurrentHumidity;
        }

        public CurrentTemperaturePropertyInfo getCurrentTemperature() {
            return CurrentTemperature;
        }

        public CurrentLightPropertyInfo getCurrentLight() { return CurrentLight; }      /*修改*/

        public TempThresholdPropertyInfo getTempThreshold() {
            return TempThreshold;
        }

        public LightThresholdPropertyInfo getLightThreshold() { return LightThreshold; }    /*修改*/

        public void setCurrentHumidity(CurrentHumidityPropertyInfo currentHumidity) {
            CurrentHumidity = currentHumidity;
        }

        public void setCurrentLight(CurrentLightPropertyInfo currentLight) {    /*修改*/
            CurrentLight = currentLight;
        }

        public void setCurrentTemperature(CurrentTemperaturePropertyInfo currentTemperature) {
            CurrentTemperature = currentTemperature;
        }

        public void setTempThreshold(TempThresholdPropertyInfo tempThreshold) {
            TempThreshold = tempThreshold;
        }

        public void setLightThreshold(LightThresholdPropertyInfo lightThreshold) { LightThreshold = lightThreshold; } /*修改*/
    }
    public static class CurrentLightPropertyInfo{   /*修改*/
        private Float value;
        private Date time;

        public Float getValue() { return value; }

        public Date getTime() { return time; }

        public void setValue(Float value) { this.value = value; }

        public void setTime(Date time) { this.time = time; }
    }
    public static class CurrentHumidityPropertyInfo{
        private Float value;
        private Date  time;

        public Float getValue() {
            return value;
        }

        public Date getTime() {
            return time;
        }

        public void setValue(Float value) {
            this.value = value;
        }

        public void setTime(Date time) {
            this.time = time;
        }
    }
    public static class CurrentTemperaturePropertyInfo{
        private Float value;
        private Date  time;

        public Float getValue() {
            return value;
        }

        public Date getTime() {
            return time;
        }

        public void setValue(Float value) {
            this.value = value;
        }

        public void setTime(Date time) {
            this.time = time;
        }
    }
    public static class TempThresholdPropertyInfo{
        private Float value;
        private Date time;

        public Float getValue() {
            return value;
        }

        public Date getTime() {
            return time;
        }

        public void setValue(Float value) {
            this.value = value;
        }

        public void setTime(Date time) {
            this.time = time;
        }
    }
    public static class LightThresholdPropertyInfo{
        private Float value;
        private Date time;

        public Float getValue() { return value; }

        public Date getTime() { return time; }

        public void setValue(Float value) { this.value = value; }

        public void setTime(Date time) { this.time = time; }
    }

}
