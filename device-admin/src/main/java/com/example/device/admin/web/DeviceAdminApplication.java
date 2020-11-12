package com.example.device.admin.web;

import com.example.device.admin.service.impl.DeviceInfoServiceImpl;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.ConfigurableApplicationContext;
import tk.mybatis.spring.annotation.MapperScan;

@SpringBootApplication(scanBasePackages = "com.example.device.admin")
@MapperScan(basePackages = "com.example.device.admin.dao.mapper")
public class DeviceAdminApplication {
    public static void main(String[] args) {
        SpringApplication.run(DeviceAdminApplication.class, args);
        /*试验
        byte testbyte = 50;
        int testbyte1 = -50;//1100,1110
        int gaowei = (testbyte & 0x7f) * 256 + ((testbyte>>>7) & 0x01) * 256 * 128;
        int diwei = (testbyte1 & 0x7f) + ((testbyte1>>>7) & 0x01) * 128;
        int res = gaowei + diwei;
        System.out.println(gaowei);
        System.out.println(diwei);
        System.out.println(res);
         */
    }
}


