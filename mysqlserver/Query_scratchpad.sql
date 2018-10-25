select * from social_network.Users;
select * from social_network.Notifications;
insert into social_network.Users (userID, userName, passwordHash, salt)
values (1, 'frodo', 'idorhuckb039', 'xrdoeuch'), (2, 'sam', 'idrhoehu082h3', 'rcicoheur');
delete from social_network.Users where userID > 0;

CREATE TABLE Notifications (
	postID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT
	,userID SMALLINT UNSIGNED NOT NULL
	,`read` boolean NOT NULL
	,timestamp DATETIME NOT NULL
	,PRIMARY KEY (postID)
	,UNIQUE KEY postID_UNIQUE (postID)
	,UNIQUE KEY userID_UNIQUE (userID)
	) engine = InnoDB AUTO_INCREMENT = 3 DEFAULT CHARSET = latin1;



CREATE TABLE `Users` (
  `userID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `userName` varchar(25) NOT NULL,
  `passwordHash` varchar(45) NOT NULL,
  `salt` varchar(45) NOT NULL,
  PRIMARY KEY (`userID`),
  UNIQUE KEY `userID_UNIQUE` (`userID`),
  UNIQUE KEY `userName_UNIQUE` (`userName`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
