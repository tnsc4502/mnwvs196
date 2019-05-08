/*
Navicat MySQL Data Transfer

Source Server         : SQL
Source Server Version : 50051
Source Host           : localhost:3306
Source Database       : mnms

Target Server Type    : MYSQL
Target Server Version : 50051
File Encoding         : 65001

Date: 2019-05-08 15:41:39
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `AccountID` int(11) NOT NULL auto_increment,
  `AccountName` varchar(64) NOT NULL default '',
  `Password` varchar(255) NOT NULL default '',
  `NexonCash` int(11) default '0',
  `MaplePoint` int(11) default '0',
  `NXPoint3` int(11) default '0',
  `RegisterDate` datetime default NULL,
  PRIMARY KEY  (`AccountID`,`AccountName`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cashiteminfo
-- ----------------------------
DROP TABLE IF EXISTS `cashiteminfo`;
CREATE TABLE `cashiteminfo` (
  `SN` int(11) NOT NULL auto_increment,
  `CashItemSN` bigint(21) NOT NULL default '0',
  `AccountID` int(11) NOT NULL default '0',
  `CharacterID` int(11) NOT NULL default '0',
  `ItemID` int(11) default '0',
  `CommodityID` int(11) default '0',
  `Number` int(6) default '1',
  `BuyCharacterID` varchar(32) default '',
  `DateExpire` bigint(21) default '0',
  `PaybackRate` int(11) default '0',
  `DiscountRate` decimal(10,0) default '0',
  `OrderNo` int(11) default '0',
  `ProductNo` int(11) default '0',
  `Refundable` int(1) default '0',
  `SourceFlag` int(1) default '0',
  `StoreBank` int(1) default '0',
  `OptExpireDate` bigint(21) default '0',
  `OptGrade` int(11) default '0',
  `Opt1` int(11) default '0',
  `Opt2` int(11) default '0',
  `Opt3` int(11) default '0',
  PRIMARY KEY  (`SN`,`AccountID`,`CashItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=35 DEFAULT CHARSET=utf8;

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
-- Table structure for characterlevel
-- ----------------------------
DROP TABLE IF EXISTS `characterlevel`;
CREATE TABLE `characterlevel` (
  `CharacterID` int(11) NOT NULL,
  `Level` int(11) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
-- Table structure for characters
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `CharacterID` int(11) NOT NULL auto_increment,
  `AccountID` int(11) default NULL,
  `WorldID` int(11) default NULL,
  `CharacterName` varchar(15) character set big5 default '',
  `GuildID` int(11) default '0',
  `PartyID` int(11) default '0',
  `FieldID` int(11) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=2048 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for characterslotcount
-- ----------------------------
DROP TABLE IF EXISTS `characterslotcount`;
CREATE TABLE `characterslotcount` (
  `CharacterID` int(11) NOT NULL,
  `EquipSlot` int(3) NOT NULL,
  `ConSlot` int(3) NOT NULL,
  `InstallSlot` int(3) NOT NULL,
  `EtcSlot` int(3) NOT NULL,
  `CashSlot` int(3) NOT NULL,
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for characterstat
-- ----------------------------
DROP TABLE IF EXISTS `characterstat`;
CREATE TABLE `characterstat` (
  `SN` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) NOT NULL,
  `Exp` bigint(20) default '0',
  `HP` int(11) default '0',
  `MP` int(11) default '0',
  `MaxHP` int(11) default '0',
  `MaxMP` int(11) default '0',
  `Gender` int(1) default '0',
  `Fame` int(11) default '0',
  `Job` int(11) default '0',
  `SubJob` int(11) default '0',
  `Str` int(11) default NULL,
  `Dex` int(11) default '0',
  `Int_` int(11) default '0',
  `Luk` int(11) default '0',
  `SP` varchar(100) default '0,0,0,0,0,0,0,0,0,0,0',
  `AP` int(11) default '0',
  `POP` int(11) default '0',
  `CharismaEXP` int(11) default '0',
  `InsightEXP` int(11) default '0',
  `WillEXP` int(11) default '0',
  `SenseEXP` int(11) default '0',
  `CharmEXP` int(11) default '0',
  `Hair` int(11) default '0',
  `Skin` int(11) default '0',
  `Face` int(11) default '0',
  `FaceMark` int(11) default '0',
  PRIMARY KEY  (`SN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=43 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for funckeymapped
-- ----------------------------
DROP TABLE IF EXISTS `funckeymapped`;
CREATE TABLE `funckeymapped` (
  `CharacterID` int(11) NOT NULL,
  `Key` int(11) NOT NULL,
  `Type` int(3) default '0',
  `Value` int(11) default '0',
  PRIMARY KEY  (`CharacterID`,`Key`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemlocker
-- ----------------------------
DROP TABLE IF EXISTS `itemlocker`;
CREATE TABLE `itemlocker` (
  `SN` int(11) NOT NULL auto_increment,
  `CashItemSN` int(11) NOT NULL,
  `AccountID` bigint(21) NOT NULL default '0',
  `Type` int(1) default '1',
  `Locked` int(1) default '0',
  PRIMARY KEY  (`SN`,`CashItemSN`,`AccountID`)
) ENGINE=MyISAM AUTO_INCREMENT=33 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_cash
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_cash`;
CREATE TABLE `itemslot_cash` (
  `SN` int(11) NOT NULL auto_increment,
  `CashItemSN` int(11) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`,`CashItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=648 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_con
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_con`;
CREATE TABLE `itemslot_con` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` bigint(21) NOT NULL,
  `ItemID` int(11) default '0',
  `CharacterID` int(11) NOT NULL,
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`,`ItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=783 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_eqp
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_eqp`;
CREATE TABLE `itemslot_eqp` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` bigint(21) NOT NULL,
  `CashItemSN` bigint(21) default '-1',
  `ItemID` int(11) default NULL,
  `CharacterID` int(11) NOT NULL,
  `Number` int(11) default NULL,
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  `RUC` int(11) default '0',
  `CUC` int(11) default '0',
  `Cuttable` int(11) default '0',
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
  PRIMARY KEY  (`SN`,`ItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=2250 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_etc
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_etc`;
CREATE TABLE `itemslot_etc` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` int(11) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default NULL,
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`,`ItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=216 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_ins
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_ins`;
CREATE TABLE `itemslot_ins` (
  `SN` int(11) NOT NULL auto_increment,
  `ItemSN` int(11) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`SN`,`ItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=206 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_pet
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_pet`;
CREATE TABLE `itemslot_pet` (
  `SN` int(11) NOT NULL auto_increment,
  `CashItemSN` int(11) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `PetAttribute` int(11) default '0',
  `POS` int(11) default '0',
  `Level` int(3) default '0',
  `Repleteness` int(3) default '0',
  `Tameness` int(6) default '0',
  `PetName` varchar(18) default '',
  `RemainLife` int(11) default '0',
  `ActiveState` int(1) default '0',
  `AutoBuffSkill` int(11) default '0',
  `PetHue` int(11) default '0',
  `GiantRate` int(6) default '0',
  `PetSkill` int(6) default '0',
  PRIMARY KEY  (`SN`,`CashItemSN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=210 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for mobrewards
-- ----------------------------
DROP TABLE IF EXISTS `mobrewards`;
CREATE TABLE `mobrewards` (
  `id` bigint(20) NOT NULL auto_increment,
  `dropperid` int(11) NOT NULL,
  `itemid` int(11) NOT NULL default '0',
  `minimum_quantity` int(11) NOT NULL default '1',
  `maximum_quantity` int(11) NOT NULL default '1',
  `questid` int(11) NOT NULL default '0',
  `chance` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `mobid` (`dropperid`)
) ENGINE=MyISAM AUTO_INCREMENT=45197 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for questrecord
-- ----------------------------
DROP TABLE IF EXISTS `questrecord`;
CREATE TABLE `questrecord` (
  `ID` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) NOT NULL default '0',
  `QuestID` int(11) default '0',
  `State` int(11) default '0',
  `Time` bigint(21) default '0',
  `StrRecord` varchar(128) default '',
  `MobRecord` varchar(255) default '',
  PRIMARY KEY  (`ID`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=108397 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for shop
-- ----------------------------
DROP TABLE IF EXISTS `shop`;
CREATE TABLE `shop` (
  `SN` int(11) NOT NULL auto_increment,
  `NpcID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `Price` int(10) default '0',
  `TokenItemID` int(11) default '0',
  `TokenPrice` int(11) default '0',
  `Quantity` int(6) default '0',
  `MaxPerSlot` int(11) default '0',
  `TabIndex` int(11) default '0',
  `LevelLimited` int(11) default '0',
  `Period` bigint(21) default '0',
  `ShowLevMin` int(11) default '0',
  `ShowLevMax` int(11) default '0',
  `ftSellStart` bigint(21) default '94354848000000000',
  `ftSellEnd` bigint(21) default '150842304000000000',
  PRIMARY KEY  (`SN`,`NpcID`)
) ENGINE=MyISAM AUTO_INCREMENT=7098 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for skillrecord
-- ----------------------------
DROP TABLE IF EXISTS `skillrecord`;
CREATE TABLE `skillrecord` (
  `CharacterID` int(11) NOT NULL,
  `SkillID` int(11) NOT NULL,
  `SLV` int(11) default NULL,
  `MasterLevel` int(11) default NULL,
  `Expired` bigint(20) default NULL,
  PRIMARY KEY  (`CharacterID`,`SkillID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
