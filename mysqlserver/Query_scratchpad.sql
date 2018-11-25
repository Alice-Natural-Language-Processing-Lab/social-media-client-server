CREATE DATABASE `SocialNetwork` /*!40100 DEFAULT CHARACTER SET latin1 */;

CREATE TABLE `SocialNetwork`.`Users` (
  `userID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `userName` varchar(25) NOT NULL,
  `passwordHash` varchar(15) NOT NULL,
  PRIMARY KEY (`userID`),
  UNIQUE KEY `userID_UNIQUE` (`userID`),
  UNIQUE KEY `userName_UNIQUE` (`userName`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `SocialNetwork`.`Posts` (
  `postID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `posterUserID` smallint(5) unsigned NOT NULL,
  `posteeUserID` smallint(5) unsigned NOT NULL,
  `timestamp` datetime NOT NULL,
  `content` TEXT NOT NULL,
  PRIMARY KEY (`postID`),
  UNIQUE KEY `postID_UNIQUE` (`postID`),
  KEY `fk_Posts_1_idx` (`posterUserID`),
  CONSTRAINT `fk_Posts_1` FOREIGN KEY (`posterUserID`) REFERENCES `Users` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE,
  KEY `fk_Posts_2_idx` (`posteeUserID`),
  CONSTRAINT `fk_Posts_2` FOREIGN KEY (`posteeUserID`) REFERENCES `Users` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `SocialNetwork`.`Notifications` (
  `notificationID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `postID` smallint(5) unsigned NOT NULL,
  `userID` smallint(5) unsigned NOT NULL,
  `read` boolean NOT NULL,
  `timestamp` datetime NOT NULL,
  PRIMARY KEY (`notificationID`),
  UNIQUE KEY `notificationID_UNIQUE` (`notificationID`),
  UNIQUE KEY `post_user_UNIQUE` (`postID`,`userID`),
  KEY `fk_Notifications_1_idx` (`userID`),
  CONSTRAINT `fk_Notifications_1` FOREIGN KEY (`userID`) REFERENCES `Users` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE,
  KEY `fk_Notifications_2_idx` (`postID`),
  CONSTRAINT `fk_Notifications_2` FOREIGN KEY (`postID`) REFERENCES `Posts` (`postID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `SocialNetwork`.`InteractionLog` (
  `interactionID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `userID` smallint(5) unsigned NOT NULL,
  `sessionID` int unsigned,
  `timestamp` datetime NOT NULL,
  `logout` boolean NOT NULL,
  `socketDescriptor` smallint(5) NOT NULL,
  `command` varchar(25) NOT NULL,
  PRIMARY KEY (`interactionID`),
  UNIQUE KEY `interactionID_UNIQUE` (`interactionID`),
  KEY `fk_InteractionLog_1_idx` (`userID`),
  CONSTRAINT `fk_InteractionLog_1` FOREIGN KEY (`userID`) REFERENCES `Users` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

insert into SocialNetwork.Users (userName, passwordHash)
values ('frodo', '123456789012345'), ('sam', '543210987654321');

insert into SocialNetwork.Posts (posterUserID, posteeUserID, timestamp, content)
values (1, 2, '2018-11-07', "Hello friends, how's it going?"), (2, 2, '2018-11-07', "Super political post");

insert into SocialNetwork.Notifications (postID, userID, `read`, timestamp)
values (1, 1, 0, '2018-11-07 00:00:00'), (2,2,0,'2018-11-07');

insert into SocialNetwork.InteractionLog (userID, sessionID, timestamp, logout, socketDescriptor, command)
values (1, "2345678901", '2018-11-24', 0, 5, "show wall frodo"), (1, "2345678901", '2018-11-24 22:14:00', 0, 5, "post wall frodo Hi there!")
, (2, "3456789012", '2018-11-24 22:16:00', 0, 6, "show wall sam"), (2, "3456789012", '2018-11-24 22:18:00', 1, 6, "logout") ;

select * from SocialNetwork.Users;
select * from SocialNetwork.Posts;
select * from SocialNetwork.Notifications;
select * from SocialNetwork.InteractionLog;

/*for deleting everything*/
DROP TABLE `SocialNetwork`.`InteractionLog`;
DROP TABLE `SocialNetwork`.`Notifications`;
DROP TABLE `SocialNetwork`.`Posts`;
DROP TABLE `SocialNetwork`.`Users`;

DROP DATABASE `SocialNetwork`;

/*------------------------------------------------------------*/


