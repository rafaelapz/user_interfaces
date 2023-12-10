CREATE DATABASE IF NOT EXISTS MyDatabase;

USE MyDatabase;

-- users table
CREATE TABLE IF NOT EXISTS Users (
    UserID INT PRIMARY KEY AUTO_INCREMENT,
    UserName VARCHAR(50) NOT NULL,
    UNIQUE(UserName)
);

-- friend requests table
CREATE TABLE IF NOT EXISTS FriendRequests (
    RequestID INT PRIMARY KEY AUTO_INCREMENT,
    SenderUserID INT,
    ReceiverUserID INT,
    Status ENUM('Pending', 'Accepted', 'Rejected') DEFAULT 'Pending',
    FOREIGN KEY (SenderUserID) REFERENCES Users(UserID),
    FOREIGN KEY (ReceiverUserID) REFERENCES Users(UserID)
);

-- friends table
CREATE TABLE IF NOT EXISTS Friends (
    FriendID INT PRIMARY KEY AUTO_INCREMENT,
    UserName VARCHAR(50) NOT NULL,
    UserId VARCHAR(50),
    Phone VARCHAR(20),
    ParentFriendID INT,
    FOREIGN KEY (ParentFriendID) REFERENCES Friends(FriendID),
    UNIQUE(UserId)
);

-- videos table
CREATE TABLE IF NOT EXISTS Videos (
    VideoID INT PRIMARY KEY AUTO_INCREMENT,
    UniqueUsername VARCHAR(50) UNIQUE NOT NULL,
    FileLocation VARCHAR(255) NOT NULL,
    Caption TEXT,
    NumberOfLikes INT DEFAULT 0,
    PeopleWhoLikedIt TEXT,
    PostedDate TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT uc_UniqueUsername UNIQUE (UniqueUsername)
);

-- user profile table
CREATE TABLE IF NOT EXISTS UserProfile (
    UniqueUsername VARCHAR(50) PRIMARY KEY,
    ProfilePictureLocation VARCHAR(255),
    Friends TEXT, -- list of friend usernames
    NumberOfFriends INT DEFAULT 0,
    FOREIGN KEY (UniqueUsername) REFERENCES Users(UserName)
);

-- user memories table
CREATE TABLE IF NOT EXISTS UserMemories (
    MemoryID INT PRIMARY KEY AUTO_INCREMENT,
    UniqueUsername VARCHAR(50) NOT NULL,
    VideoID INT,
    FOREIGN KEY (UniqueUsername) REFERENCES UserProfile(UniqueUsername),
    FOREIGN KEY (VideoID) REFERENCES Videos(VideoID)
);

-- retrieve memories for a user
DELIMITER //
CREATE FUNCTION GetUserMemories(username VARCHAR(50))
RETURNS TEXT
BEGIN
    DECLARE memory_list TEXT DEFAULT '';
    SELECT GROUP_CONCAT(FileLocation, '|', Caption) INTO memory_list
    FROM Videos
    WHERE UniqueUsername = username
    ORDER BY PostedDate DESC;
    RETURN memory_list;
END //
DELIMITER ;

-- sample data 
INSERT INTO Users (UserName) VALUES ('John'), ('Sarah'), ('James');

INSERT INTO FriendRequests (SenderUserID, ReceiverUserID) VALUES (1, 2), (2, 3), (3, 1);

UPDATE FriendRequests
SET Status = 'Accepted'
WHERE SenderUserID = 1 AND ReceiverUserID = 2;

UPDATE FriendRequests
SET Status = 'Rejected'
WHERE SenderUserID = 2 AND ReceiverUserID = 3;

SELECT Users.UserName AS SenderName
FROM FriendRequests
JOIN Users ON FriendRequests.SenderUserID = Users.UserID
WHERE ReceiverUserID = 1 AND Status = 'Pending';

-- additional sample data for videos table
INSERT INTO Videos (UniqueUsername, FileLocation, Caption) VALUES 
('John', '/videos/vid1.mp4', 'First Video'), 
('Sarah', '/videos/vid2.mp4', 'Second Video');

-- additional sample data for user profile table
INSERT INTO UserProfile (UniqueUsername, ProfilePictureLocation, Friends, NumberOfFriends) 
VALUES 
('John', '/profiles/John.jpg', 'Sarah,James', 2),
('Sarah', '/profiles/Sarah.jpg', 'John', 1),
('James', '/profiles/James.jpg', 'John', 1);