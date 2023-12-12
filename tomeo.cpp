//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QInputDialog>
#include "the_player.h"
#include "the_button.h"

// Function to send a friend request
void sendFriendRequest(const QString& senderUsername, const QString& receiverUsername) {
    QSqlQuery query;
    query.prepare("INSERT INTO FriendRequests (SenderUserID, ReceiverUserID) "
                  "VALUES ((SELECT UserID FROM Users WHERE UserName = :sender), "
                  "(SELECT UserID FROM Users WHERE UserName = :receiver));");
    query.bindValue(":sender", senderUsername);
    query.bindValue(":receiver", receiverUsername);

    if (query.exec()) {
        std::cout << "Friend request sent." << std::endl;
    } else {
        std::cerr << "Error sending friend request: " << query.lastError().text().toStdString() << std::endl;
    }
}

// Function to search for a user
QString searchUser(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT UserName FROM Users WHERE UserName = :username;");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    } else {
        std::cerr << "Error searching for user: " << query.lastError().text().toStdString() << std::endl;
        return "";
    }
}

// Read in videos and thumbnails from this directory
std::vector<TheButtonInfo> getInfoIn(const std::string& loc) {
    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) { // For all files
        QString f = it.next();

        if (f.contains("."))
#if defined(_WIN32)
            if (f.contains(".wmv")) { // Windows
#else
            if (f.contains(".mp4") || f.contains("MOV")) { // Mac/Linux
#endif
                QString thumb = f.left(f.length() - 4) + ".png";
                if (QFile(thumb).exists()) { // If a png thumbnail exists
                    QImageReader* imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // Read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // Create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f));      // Convert the file location to a generic url
                        out.push_back(TheButtonInfo(url, ico));           // Add to the output list
                    } else {
                        qDebug() << "Warning: Skipping video because I couldn't process thumbnail " << thumb;
                    }
                } else {
                    qDebug() << "Warning: Skipping video because I couldn't find thumbnail " << thumb;
                }
            }
    }

    return out;
}

int main(int argc, char* argv[]) {
    // Your existing code for setting up Qt, videoWidget, player, buttons, etc.
    // Let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR;

    // Initialize the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("finaldata.db");
    if (!db.open()) {
        qDebug() << "Error: Unable to open the database.";
        return -1;
    }

    // Create the Qt Application
    QApplication app(argc, argv);

    // Collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn(std::string(argv[1]));

    if (videos.size() == 0) {
        const int result = QMessageBox::information(
            nullptr,
            QString("Tomeo"),
            QString("No videos found! Add command line argument to \"quoted\" file location."));
        return -1;
    }

    // The widget that will show the video
    QVideoWidget* videoWidget = new QVideoWidget;

    // The QMediaPlayer which controls the playback
    ThePlayer* player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // A row of buttons
    QWidget* buttonWidget = new QWidget();
    // A list of the buttons
    std::vector<TheButton*> buttons;
    // The buttons are arranged horizontally
    QHBoxLayout* layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    // Create the four buttons
    for (int i = 0; i < 4 && i < videos.size(); i++) {
        TheButton* button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*))); // When clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // Tell the player what buttons and videos are available
    player->setContent(&buttons, &videos);

    // Get the logged-in user (replace this with your actual login logic)
    QString loggedInUser = "John"; // Replace with the actual username

    // Friend search and request
    QString friendToSearch = QInputDialog::getText(nullptr, "Friend Search", "Enter the username of the friend you want to search for:");

    QString foundUser = searchUser(friendToSearch);

    if (!foundUser.isEmpty()) {
        // Check if they are already friends
        QSqlQuery friendshipQuery;
        friendshipQuery.prepare("SELECT * FROM Friendship "
                                "WHERE (User1ID = (SELECT UserID FROM Users WHERE UserName = :loggedInUser) AND "
                                "User2ID = (SELECT UserID FROM Users WHERE UserName = :foundUser)) OR "
                                "(User1ID = (SELECT UserID FROM Users WHERE UserName = :foundUser) AND "
                                "User2ID = (SELECT UserID FROM Users WHERE UserName = :loggedInUser));");
        friendshipQuery.bindValue(":loggedInUser", loggedInUser);
        friendshipQuery.bindValue(":foundUser", foundUser);

        if (friendshipQuery.exec() && !friendshipQuery.next()) {
            // They are not already friends, so send a friend request
            sendFriendRequest(loggedInUser, foundUser);
        } else {
            std::cout << "You are already friends with " << friendToSearch.toStdString() << std::endl;
        }
    } else {
        std::cout << "User not found: " << friendToSearch.toStdString() << std::endl;
    }

    // Your existing code for creating the main window, layout, and showing the application
    // Create the main window and layout
    QWidget window;
    QVBoxLayout* top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("Tomeo");
    window.setMinimumSize(800, 680);

    // Add the video and the buttons to the top-level widget
    top->addWidget(videoWidget);
    top->addWidget(buttonWidget);

    // Showtime!
    window.show();

    // Wait for the app to terminate
    return app.exec();
}
