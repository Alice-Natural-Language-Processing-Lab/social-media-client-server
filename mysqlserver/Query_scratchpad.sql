CREATE TABLE `Users` (
  `userID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `userName` varchar(25) NOT NULL,
  `passwordHash` varchar(45) NOT NULL,
  `salt` varchar(45) NOT NULL,
  PRIMARY KEY (`userID`),
  UNIQUE KEY `userID_UNIQUE` (`userID`),
  UNIQUE KEY `userName_UNIQUE` (`userName`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `Posts` (
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

CREATE TABLE `Notifications` (
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

CREATE TABLE `InteractionLog` (
  `interactionID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `userID` smallint(5) unsigned NOT NULL,
  `sessionID` varchar(25),
  `timestamp` datetime NOT NULL,
  `userStatus` boolean NOT NULL,
  `socketDescriptor` smallint(5) NOT NULL,
  `command` varchar(25) NOT NULL,
  PRIMARY KEY (`interactionID`),
  UNIQUE KEY `interactionID_UNIQUE` (`interactionID`),
  KEY `fk_InteractionLog_1_idx` (`userID`),
  CONSTRAINT `fk_InteractionLog_1` FOREIGN KEY (`userID`) REFERENCES `Users` (`userID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

insert into social_network.Users (userName, passwordHash, salt)
values ('frodo', 'idorhuckb039', 'xrdoeuch'), ('sam', 'idrhoehu082h3', 'rcicoheur');

insert into social_network.Posts (posterUserID, posteeUserID, timestamp, content)
values (1, 2, '2018-11-07', "Hello friends, how's it going?"), (2, 2, '2018-11-07', "Super political post");

insert into social_network.Notifications (postID, userID, `read`, timestamp)
values (1, 1, 0, '2018-11-07 00:00:00'), (2,2,0,'2018-11-07');

insert into social_network.InteractionLog (userID, sessionID, timestamp, userStatus, socketDescriptor, command)
values (1, "id2k9h20dx9oeh", '2018-11-07', 1, 5, "show wall frodo");

update Users set userID = 5 where userID = 1;

delete from Users where userID = 1;

select * from social_network.Users;
select * from social_network.Posts;
select * from social_network.Notifications;
select * from social_network.InteractionLog;

DROP TABLE `social_network`.`InteractionLog`;
DROP TABLE `social_network`.`Notifications`;
DROP TABLE `social_network`.`Posts`;
DROP TABLE `social_network`.`Users`;
