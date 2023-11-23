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

-- adding a friend with a parent
INSERT INTO Friends (UserName, RealName, Phone, ParentFriendID)
VALUES ('John Jr.', 'Doe', '555-1111', 1);

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
VALUES (1, 2); -- user id 1 adding user id 2

-- table for videos
CREATE TABLE IF NOT EXISTS Videos (
    VideoID INT PRIMARY KEY AUTO_INCREMENT,
    UniqueVideoID VARCHAR(50) UNIQUE NOT NULL,
    Likes INT DEFAULT 0,
    LocationOfFile VARCHAR(255) NOT NULL,
    VideoName VARCHAR(50) NOT NULL,
    PeopleWhoLikedIt TEXT -- list of user ids who liked it
);

-- inserting a video
INSERT INTO Videos (UniqueVideoID, LocationOfFile, VideoName)
VALUES ('UniVidId', '/videos/UniVidId.mp4', 'video');

-- number of likes
UPDATE Videos
SET Likes = 10
WHERE UniqueVideoID = 'UniVidId';

-- people who liked the video
UPDATE Videos
SET PeopleWhoLikedIt = 'id of people who liked'
WHERE UniqueVideoID = 'UniVidId';

-- accept friend requests
UPDATE FriendRequests
SET Status = 'Accepted'
WHERE SenderUserID = 1 AND ReceiverUserID = 2;

-- reject friend requests
UPDATE FriendRequests
SET Status = 'Rejected'
WHERE SenderUserID = 1 AND ReceiverUserID = 2;

-- pending friend requests
SELECT Users.UserName AS SenderName
FROM FriendRequests
JOIN Users ON FriendRequests.SenderUserID = Users.UserID
WHERE ReceiverUserID = 2 AND Status = 'Pending';
