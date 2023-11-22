CREATE DATABASE IF NOT EXISTS MyFriendsDatabase;

USE MyFriendsDatabase;

CREATE TABLE IF NOT EXISTS Friends (
    FriendID INT PRIMARY KEY AUTO_INCREMENT,
    UserName VARCHAR(50) NOT NULL,
    RealName VARCHAR(50),
    Phone VARCHAR(20),
    ParentFriendID INT,
    FOREIGN KEY (ParentFriendID) REFERENCES Friends(FriendID)
);

INSERT INTO Friends (UserName, RealName, Phone)
VALUES ('John', 'Doe', '555-1234');

INSERT INTO Friends (UserName, RealName, Phone)
VALUES ('Jane', 'Smith', '555-5678');

--Adding a friend with a parent
INSERT INTO Friends (UserName, RealName, Phone, ParentFriendID)
VALUES ('John Jr.', 'Doe', '555-1111', 1);

SELECT f1.UserName AS FriendName, f2.UserName AS ParentName
FROM Friends f1
LEFT JOIN Friends f2 ON f1.ParentFriendID = f2.FriendID;

CREATE TABLE IF NOT EXISTS Users (
    UserID INT PRIMARY KEY AUTO_INCREMENT,
    UserName VARCHAR(50) NOT NULL,
);

CREATE TABLE IF NOT EXISTS FriendRequests (
    RequestID INT PRIMARY KEY AUTO_INCREMENT,
    SenderUserID INT,
    ReceiverUserID INT,
    Status ENUM('Pending', 'Accepted', 'Rejected') DEFAULT 'Pending',
    FOREIGN KEY (SenderUserID) REFERENCES Users(UserID),
    FOREIGN KEY (ReceiverUserID) REFERENCES Users(UserID)
);

INSERT INTO FriendRequests (SenderUserID, ReceiverUserID)
VALUES (1, 2); --Assuming user with ID 1 is sending a friend request to user with ID 2

--To accept
UPDATE FriendRequests
SET Status = 'Accepted'
WHERE SenderUserID = 1 AND ReceiverUserID = 2;

--To reject
UPDATE FriendRequests
SET Status = 'Rejected'
WHERE SenderUserID = 1 AND ReceiverUserID = 2;

SELECT Users.UserName AS SenderName
FROM FriendRequests
JOIN Users ON FriendRequests.SenderUserID = Users.UserID
WHERE ReceiverUserID = 2 AND Status = 'Pending';