CREATE TABLE `Users` (
  `userID` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `userName` varchar(25) NOT NULL,
  `passwordHash` varchar(45) NOT NULL,
  `salt` varchar(45) NOT NULL,
  PRIMARY KEY (`userID`),
  UNIQUE KEY `userID_UNIQUE` (`userID`),
  UNIQUE KEY `userName_UNIQUE` (`userName`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

insert into social_network.Users (userID, userName, passwordHash, salt)
values (1, 'frodo', 'idorhuckb039', 'xrdoeuch'), (2, 'sam', 'idrhoehu082h3', 'rcicoheur');

CREATE TABLE `Notifications` (
  `notificationID` smallint(5) unsigned NOT NULL,
  `postID` smallint(5) unsigned NOT NULL,
  `userID` smallint(5) unsigned NOT NULL,
  `read` tinyint(3) unsigned NOT NULL,
  `timestamp` datetime NOT NULL,
  PRIMARY KEY (`notificationID`),
  UNIQUE KEY `notificationID_UNIQUE` (`notificationID`),
  KEY `fk_Notifications_1_idx` (`userID`),
  CONSTRAINT `fk_Notifications_1` FOREIGN KEY (`userID`) REFERENCES `Users` (`userID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

select * from social_network.Users;
select * from social_network.Notifications;

delete from social_network.Users where userID > 0;