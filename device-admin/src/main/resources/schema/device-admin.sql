/*
SQLyog Trial v13.1.1 (64 bit)
MySQL - 5.7.23-log : Database - device_admin
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`device_admin` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `device_admin`;

/*Table structure for table `alarm_history_log` */

CREATE TABLE `alarm_history_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_name` varchar(100) DEFAULT NULL COMMENT '设备名',
  `alarm_temperature` float DEFAULT NULL COMMENT '报警时温度值',
  `temp_threshold` float DEFAULT NULL COMMENT '报警时温度阈值',
  `gmt_create` datetime DEFAULT NULL COMMENT '创建时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;



CREATE TABLE `device_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_name` varchar(100) DEFAULT NULL COMMENT '设备名',
  `current_temperature` float DEFAULT NULL COMMENT '当前温度值',
  `current_humidity` float DEFAULT NULL COMMENT '当前湿度值',
  `current_light` float DEFAULT NULL COMMENT '当前光强值',
  `temp_threshold` float DEFAULT NULL COMMENT '温度报警阈值',
  `light_threshold` float DEFAULT NULL COMMENT '光照强度阈值',
  `alarm_status` tinyint(1) DEFAULT NULL COMMENT '报警状态',
  `last_alarm_date` datetime DEFAULT NULL COMMENT '最近一次报警时间',
  `last_online_date` datetime DEFAULT NULL COMMENT '最近一次上线时间',
  `connect_status` varchar(50) DEFAULT NULL COMMENT '在线状态',
  `gmt_update` datetime DEFAULT NULL COMMENT '刷新时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `device_name` (`device_name`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;



CREATE TABLE `device_prop_history_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_name` varchar(100) DEFAULT NULL COMMENT '设备名',
  `current_temperature` float DEFAULT NULL COMMENT '当前温度值',
  `current_humidity` float DEFAULT NULL COMMENT '当前湿度值',
  `current_light` float DEFAULT NULL COMMENT '当前光强值',
  `gmt_create` datetime DEFAULT NULL COMMENT '创建时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

/*Data for the table `device_prop_history_log` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
