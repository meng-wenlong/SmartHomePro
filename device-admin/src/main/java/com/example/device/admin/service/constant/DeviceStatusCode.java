package com.example.device.admin.service.constant;

public enum DeviceStatusCode {
    ONLINE("online","在线"),
    OFFLINE("offline","离线"),
    UNACTIVE("unactive","未激活"),
    DISABLE("disable","已禁用");

    private final String code;
    private final String meaning;

    DeviceStatusCode(String code, String meaning) {
        this.code=code;
        this.meaning = meaning;
    }
    public String getMeaning() {
        return meaning;
    }
    public String getCode() {
        return code;
    }

    public static String getDeviceStatusCode(String code) {
        for (DeviceStatusCode deviceStandardConstant : DeviceStatusCode.values()) {
            if (code.equals(deviceStandardConstant.getCode())) {
                return deviceStandardConstant.getMeaning();
            }
        }
        return null;
    }
}
