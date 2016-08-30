DROP TABLE IF EXISTS `DBConnectPool`;
CREATE TABLE `DBConnectPool`
(
	`ID` int unsigned NOT NULL AUTO_INCREMENT,
	`USERID` int unsigned NOT NULL,
	`NAME`   varchar(32) NOT NULL,
	`DATA`   varchar(1) NOT NULL,
	PRIMARY KEY(`ID`)
)ENGINE=InnoDB DEFAULT CHARSET = utf8;
