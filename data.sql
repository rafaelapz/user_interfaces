ATTACH DATABASE 'MyDatabase.db' AS MyDatabase;

-- users table
CREATE TABLE IF NOT EXISTS Users (
    UserID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserName TEXT NOT NULL UNIQUE
);

-- friend requests table
CREATE TABLE IF NOT EXISTS FriendRequests (
    RequestID INTEGER PRIMARY KEY AUTOINCREMENT,
    SenderUserID INTEGER,
    ReceiverUserID INTEGER,
    Status TEXT DEFAULT 'Pending',
    FOREIGN KEY (SenderUserID) REFERENCES Users(UserID),
    FOREIGN KEY (ReceiverUserID) REFERENCES Users(UserID)
);

-- friends table
CREATE TABLE IF NOT EXISTS Friends (
    FriendID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserName TEXT NOT NULL,
    UserId TEXT,
    Phone TEXT,
    ParentFriendID INTEGER,
    FOREIGN KEY (ParentFriendID) REFERENCES Friends(FriendID),
    UNIQUE(UserId)
);

-- videos table
CREATE TABLE IF NOT EXISTS Videos (
    VideoID INTEGER PRIMARY KEY AUTOINCREMENT,
    UniqueUsername TEXT UNIQUE NOT NULL,
    FileLocation TEXT NOT NULL,
    Caption TEXT,
    NumberOfLikes INTEGER DEFAULT 0,
    PeopleWhoLikedIt TEXT,
    PostedDate TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT uc_UniqueUsername UNIQUE (UniqueUsername)
);

-- user profile table
CREATE TABLE IF NOT EXISTS UserProfile (
    UniqueUsername TEXT PRIMARY KEY,
    ProfilePictureLocation TEXT,
    Friends TEXT, -- list of friend usernames
    NumberOfFriends INTEGER DEFAULT 0,
    FOREIGN KEY (UniqueUsername) REFERENCES Users(UserName)
);

-- user memories table
CREATE TABLE IF NOT EXISTS UserMemories (
    MemoryID INTEGER PRIMARY KEY AUTOINCREMENT,
    UniqueUsername TEXT NOT NULL,
    VideoID INTEGER,
    FOREIGN KEY (UniqueUsername) REFERENCES UserProfile(UniqueUsername),
    FOREIGN KEY (VideoID) REFERENCES Videos(VideoID)
);
END;
