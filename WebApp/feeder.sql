/*
SQLyog Ultimate v11.27 (32 bit)
MySQL - 5.6.24 : Database - feeder
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`feeder` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `feeder`;

/*Table structure for table `feeder_cnf` */

DROP TABLE IF EXISTS `feeder_cnf`;

CREATE TABLE `feeder_cnf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `a1h` int(11) DEFAULT NULL,
  `a1m` int(11) DEFAULT NULL,
  `a1s` int(11) DEFAULT NULL,
  `a2h` int(11) DEFAULT NULL,
  `a2m` int(11) DEFAULT NULL,
  `a2s` int(11) DEFAULT NULL,
  `count` int(11) DEFAULT NULL,
  `state` int(11) DEFAULT NULL,
  `acttime` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=38 DEFAULT CHARSET=utf8;

/*Data for the table `feeder_cnf` */

insert  into `feeder_cnf`(`id`,`a1h`,`a1m`,`a1s`,`a2h`,`a2m`,`a2s`,`count`,`state`,`acttime`) values (37,1,1,1,2,2,2,3,0,'2018-01-16 16:07:42');

/*Table structure for table `feeder_log` */

DROP TABLE IF EXISTS `feeder_log`;

CREATE TABLE `feeder_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(11) DEFAULT NULL COMMENT '1.对时请求,2.配置请求,3.日志上报',
  `subtype` int(11) DEFAULT NULL COMMENT '0.忽略,1.开机,2.配置上报,3.闹钟发生4.闹钟清除5.心跳6、闹钟详情',
  `info` text COMMENT '详细信息',
  `recordtime` datetime DEFAULT NULL COMMENT '服务器时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=142 DEFAULT CHARSET=utf8;

/*Data for the table `feeder_log` */

insert  into `feeder_log`(`id`,`type`,`subtype`,`info`,`recordtime`) values (103,2,5,'4_2017-12-21_22:25:00|<c!8:40:55_14:22:20_10>','2018-01-15 14:33:52'),(104,1,1,'7:40:55_23:2:0_1|<t!2018-01-15_14:22:36_1>','2018-01-15 14:22:36'),(105,2,5,'1_2018-01-15_14:25:00|<c!8:40:55_14:25:20_10>','2018-01-15 14:25:02'),(106,3,3,'alarm2get|','2018-01-15 14:25:21'),(107,3,6,'alarm2_ta:1683_tc:10|','2018-01-15 14:25:37'),(108,2,5,'1_2018-01-15_14:30:00|<c!8:40:55_14:29:20_10>','2018-01-15 14:30:02'),(109,1,1,'7:40:55_23:2:0_1|<t!2018-01-15_14:31:47_1>','2018-01-15 14:31:47'),(110,2,5,'1_2018-01-15_14:32:00|<c!14:35:55_14:33:20_10>','2018-01-15 14:32:02'),(111,3,3,'alarm2get|','2018-01-15 14:33:23'),(112,3,6,'alarm2_ta:1719_tc:10|','2018-01-15 14:33:40'),(113,3,5,'1_2018-01-15_14:34:01|<e!2018-01-15_14:34:02_1>','2018-01-15 14:34:02'),(114,3,3,'alarm1get|','2018-01-15 14:35:56'),(115,3,6,'alarm1_ta:1703_tc:12|','2018-01-15 14:36:12'),(116,2,5,'1_2018-01-15_14:36:20|<c!14:35:55_14:33:20_10>','2018-01-15 14:36:21'),(117,3,5,'1_2018-01-15_14:38:00|<e!2018-01-15_14:38:01_1>','2018-01-15 14:38:01'),(118,2,5,'1_2018-01-15_14:40:00|<c!14:35:55_14:33:20_10>','2018-01-15 14:40:01'),(119,3,5,'1_2018-01-15_14:42:01|<e!2018-01-15_14:42:02_1>','2018-01-15 14:42:02'),(120,2,5,'1_2018-01-15_14:44:01|<c!14:35:55_14:33:20_10>','2018-01-15 14:44:02'),(121,1,1,'14:35:55_14:33:20_10|<t!2018-01-15_14:45:06_1>','2018-01-15 14:45:06'),(122,3,3,'alarm1get|','2018-01-15 14:45:09'),(123,3,6,'alarm1_ta:1676_tc:12|','2018-01-15 14:45:26'),(124,3,3,'alarm2get|','2018-01-15 14:45:32'),(125,3,6,'alarm2_ta:1191_tc:12|','2018-01-15 14:45:46'),(126,3,5,'1_2018-01-15_14:46:00|<e!2018-01-15_14:46:02_1>','2018-01-15 14:46:02'),(127,1,1,'14:35:55_14:33:20_10|<t!2018-01-15_14:47:55_1>','2018-01-15 14:47:55'),(128,3,3,'alarm1get|','2018-01-15 14:47:58'),(129,3,6,'alarm1_ta:1585_tc:10|','2018-01-15 14:48:14'),(130,3,3,'alarm2get|','2018-01-15 14:48:20'),(131,3,6,'alarm2_ta:1740_tc:10|','2018-01-15 14:48:37'),(132,2,5,'1_2018-01-15_14:48:45|<c!14:51:55_14:49:20_10>','2018-01-15 14:48:46'),(133,3,4,'alarm1clear|','2018-01-15 14:48:49'),(134,3,4,'alarm2clear|','2018-01-15 14:48:56'),(135,3,3,'alarm2get|','2018-01-15 14:49:21'),(136,3,6,'alarm2_ta:1617_tc:10|','2018-01-15 14:49:37'),(137,3,5,'1_2018-01-15_14:50:01|<e!2018-01-15_14:50:02_1>','2018-01-15 14:50:02'),(138,3,3,'alarm1get|','2018-01-15 14:51:57'),(139,3,6,'alarm1_ta:2612_tc:17|','2018-01-15 14:52:19'),(140,2,5,'1_2018-01-15_14:52:27|<c!14:51:55_14:49:20_10>','2018-01-15 14:52:28'),(141,3,5,'1_2018-01-15_14:54:00|<e!2018-01-15_14:54:01_1>','2018-01-15 14:54:01');

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
