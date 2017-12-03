/*
Navicat MySQL Data Transfer

Source Server         : SQL
Source Server Version : 50051
Source Host           : localhost:3306
Source Database       : mnms

Target Server Type    : MYSQL
Target Server Version : 50051
File Encoding         : 65001

Date: 2017-12-03 21:43:35
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for characteravatar
-- ----------------------------
DROP TABLE IF EXISTS `characteravatar`;
CREATE TABLE `characteravatar` (
  `SN` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) default NULL,
  `Hair` int(11) default NULL,
  `Face` int(11) default NULL,
  `Skin` int(11) default NULL,
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM AUTO_INCREMENT=26 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of characteravatar
-- ----------------------------
INSERT INTO `characteravatar` VALUES ('17', '4', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('18', '5', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('19', '6', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('20', '7', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('21', '8', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('22', '9', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('23', '10', '30000', '20100', '0');
INSERT INTO `characteravatar` VALUES ('24', '11', '30000', '23301', '0');
INSERT INTO `characteravatar` VALUES ('25', '12', '37781', '21002', '0');

-- ----------------------------
-- Table structure for characterlevel
-- ----------------------------
DROP TABLE IF EXISTS `characterlevel`;
CREATE TABLE `characterlevel` (
  `CharacterID` int(11) NOT NULL,
  `Level` int(11) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of characterlevel
-- ----------------------------
INSERT INTO `characterlevel` VALUES ('4', '1');
INSERT INTO `characterlevel` VALUES ('5', '1');
INSERT INTO `characterlevel` VALUES ('6', '1');
INSERT INTO `characterlevel` VALUES ('7', '1');
INSERT INTO `characterlevel` VALUES ('8', '1');
INSERT INTO `characterlevel` VALUES ('9', '1');
INSERT INTO `characterlevel` VALUES ('10', '1');
INSERT INTO `characterlevel` VALUES ('11', '1');
INSERT INTO `characterlevel` VALUES ('12', '1');

-- ----------------------------
-- Table structure for charactermoney
-- ----------------------------
DROP TABLE IF EXISTS `charactermoney`;
CREATE TABLE `charactermoney` (
  `CharacterID` int(11) NOT NULL,
  `Money` int(11) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of charactermoney
-- ----------------------------
INSERT INTO `charactermoney` VALUES ('4', '0');
INSERT INTO `charactermoney` VALUES ('5', '0');
INSERT INTO `charactermoney` VALUES ('6', '0');
INSERT INTO `charactermoney` VALUES ('7', '0');
INSERT INTO `charactermoney` VALUES ('8', '0');
INSERT INTO `charactermoney` VALUES ('9', '0');
INSERT INTO `charactermoney` VALUES ('10', '0');
INSERT INTO `charactermoney` VALUES ('11', '0');
INSERT INTO `charactermoney` VALUES ('12', '0');

-- ----------------------------
-- Table structure for characters
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `CharacterID` int(11) NOT NULL auto_increment,
  `AccountID` int(11) default NULL,
  `Gender` int(1) default NULL,
  `WorldID` int(11) default NULL,
  `CharacterName` varchar(15) character set big5 default '',
  `Fame` int(11) default NULL,
  `GuildID` int(11) default NULL,
  `PartyID` int(11) default NULL,
  `FieldID` int(11) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=2047 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of characters
-- ----------------------------
INSERT INTO `characters` VALUES ('4', '0', '0', '0', 'sadfewrw', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('5', '1', '0', '0', 'QQQQQ', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('6', '1', '0', '0', '12345', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('9', '1', '0', '0', 'ewfwrwer', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('7', '1', '0', '0', '55555', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('8', '1', '0', '0', 'erwwegwe', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('10', '1', '0', '0', '測試', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('11', '0', '0', '0', 'ewrewrewr', '0', '0', '0', '100000000');
INSERT INTO `characters` VALUES ('12', '0', '0', '0', 'dsafsdar', '0', '0', '0', '100000000');

-- ----------------------------
-- Table structure for characterstat
-- ----------------------------
DROP TABLE IF EXISTS `characterstat`;
CREATE TABLE `characterstat` (
  `SN` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) default NULL,
  `Exp` bigint(20) default '0',
  `HP` int(11) default NULL,
  `MP` int(11) default NULL,
  `MaxHP` int(11) default NULL,
  `MaxMP` int(11) default NULL,
  `Job` int(11) default NULL,
  `SubJob` int(11) default '0',
  `Str` int(11) default NULL,
  `Dex` int(11) default NULL,
  `Int_` int(11) default NULL,
  `Luk` int(11) default NULL,
  `SP` varchar(100) default NULL,
  `AP` int(11) default NULL,
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM AUTO_INCREMENT=18 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of characterstat
-- ----------------------------
INSERT INTO `characterstat` VALUES ('9', '4', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('10', '5', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('11', '6', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('12', '7', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('13', '8', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('14', '9', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('15', '10', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('16', '11', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');
INSERT INTO `characterstat` VALUES ('17', '12', '0', '50', '50', '50', '50', '0', '0', '10', '10', '10', '10', '', '0');

-- ----------------------------
-- Table structure for itemslot_con
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_con`;
CREATE TABLE `itemslot_con` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` int(11) default NULL,
  `ItemID` int(11) default '0',
  `CharacterID` int(11) default NULL,
  `Number` int(11) default '1',
  `ExpireDate` bigint(20) default '-1',
  `Title` int(255) default NULL,
  `Attribute` int(255) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of itemslot_con
-- ----------------------------

-- ----------------------------
-- Table structure for itemslot_eqp
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_eqp`;
CREATE TABLE `itemslot_eqp` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` int(11) default NULL,
  `ItemID` varchar(255) default NULL,
  `CharacterID` int(11) default NULL,
  `Number` int(11) default NULL,
  `ExpireDate` bigint(20) default '-1',
  `Title` int(255) default NULL,
  `Attribute` int(255) default '0',
  `POS` int(11) default '0',
  `RUC` int(11) default '0',
  `CUC` int(11) default '0',
  `I_STR` int(11) default '0',
  `I_DEX` int(11) default '0',
  `I_INT` int(11) default '0',
  `I_LUK` int(11) default '0',
  `I_MaxHP` int(11) default '0',
  `I_MaxMP` int(11) default '0',
  `I_PAD` int(11) default '0',
  `I_MAD` int(11) default '0',
  `I_PDD` int(11) default '0',
  `I_MDD` int(11) default '0',
  `I_ACC` int(11) default '0',
  `I_EVA` int(11) default '0',
  `I_Speed` int(11) default '0',
  `I_Craft` int(11) default '0',
  `I_Jump` int(11) default '0',
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM AUTO_INCREMENT=2043 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of itemslot_eqp
-- ----------------------------
INSERT INTO `itemslot_eqp` VALUES ('7', '7', '1050287', '4', null, '-1', null, '0', '-5', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `itemslot_eqp` VALUES ('24', '8', '1050287', '11', null, '-1', null, '0', '-5', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `itemslot_eqp` VALUES ('25', '9', '1302000', '11', null, '-1', null, '0', '-11', '8', '5', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `itemslot_eqp` VALUES ('26', '10', '1072834', '11', null, '-1', null, '0', '-7', '0', '0', '0', '5', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `itemslot_eqp` VALUES ('2040', '11', '1302000', '11', null, '-1', null, '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `itemslot_eqp` VALUES ('2041', '12', '1072834', '12', null, '-1', null, '0', '-7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `itemslot_eqp` VALUES ('2042', '13', '1051353', '12', null, '-1', null, '0', '-5', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');

-- ----------------------------
-- Table structure for itemslot_etc
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_etc`;
CREATE TABLE `itemslot_etc` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` int(11) default NULL,
  `CharacterID` int(11) default NULL,
  `ItemID` int(11) default NULL,
  `Number` int(11) default '1',
  `ExpireDate` bigint(20) default '-1',
  `Title` int(255) default NULL,
  `Attribute` int(255) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of itemslot_etc
-- ----------------------------
INSERT INTO `itemslot_etc` VALUES ('1', '1', '11', '2070000', '40', '-5800915506248884224', null, '0', '1');

-- ----------------------------
-- Table structure for itemslot_ins
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_ins`;
CREATE TABLE `itemslot_ins` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` int(11) default NULL,
  `CharacterID` int(11) default NULL,
  `ItemID` int(11) default '0',
  `Number` int(11) default '1',
  `ExpireDate` bigint(20) default '-1',
  `Title` int(255) default NULL,
  `Attribute` int(255) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of itemslot_ins
-- ----------------------------
