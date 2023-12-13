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
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "qlabel.h"
#include "the_player.h"
#include "the_button.h"
#include "recorder.h"
#include <QStackedWidget>
#include "recordvideopage.h"
#include <QLineEdit>
#include <QStandardItemModel>
#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QListWidget>
#include <QScrollArea>
#include <QToolButton>
#include "login.h"
#include "likesubtitle.h"
#include <iostream>

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out = std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
        if (f.contains(".wmv")) { // windows
#else
        if (f.contains(".mp4") || f.contains("MOV")) { // mac/linux
#endif

            QString thumb = f.section(".", 0, -2) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                QImage sprite = imageReader->read(); // read the thumbnail
                if (!sprite.isNull()) {
                    QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                    QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url

                    // read the .txt file
                    QString txtFileName = f.section(".", 0, -2) + ".txt";
                    QFile txtFile(txtFileName);
                    if (txtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&txtFile);
                        int likes = in.readLine().toInt();
                        std::string subtitle = in.readLine().toStdString();
                        txtFile.close();

                        // add to the output list
                        out.push_back(TheButtonInfo(url, ico, likes, subtitle));
                    }
                    else {
                        qDebug() << "warning: skipping video because I couldn't process .txt file " << txtFileName << endl;
                    }
                }
                else
                    qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}

QWidget* createMemoriesPage(QStackedWidget *stackedWidget, QWidget *buttonWidget) {
    // create the Memories page widget
    QWidget *memoriesPage = new QWidget();
    QVBoxLayout *memoriesPageLayout = new QVBoxLayout(memoriesPage);

    // create 'memories' label
    QLabel *labelMem = new QLabel("<h1>Memories</h1>");
    labelMem->setFixedWidth(540);
    labelMem->setFixedHeight(60);
    labelMem->setAlignment(Qt::AlignCenter);

    // create month label
    QLabel *monthLabel = new QLabel("<h2>December</h2>");
    monthLabel->setFixedWidth(540);
    monthLabel->setFixedHeight(60);

    // create day column headers
    QWidget *daysWidget = new QWidget();
    QHBoxLayout *daysLayout = new QHBoxLayout(daysWidget);
    QStringList days = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    for (const QString &day : days) {
        QLabel *dayLabel = new QLabel(day);
        dayLabel->setAlignment(Qt::AlignCenter);
        daysLayout->addWidget(dayLabel);
    }

    daysLayout->setSpacing(3);
    daysWidget->setLayout(daysLayout);
    daysWidget->setFixedHeight(60);

    // add the labels and the buttons to the Memories page
    memoriesPageLayout->addWidget(labelMem);
    memoriesPageLayout->addWidget(monthLabel);
    memoriesPageLayout->addWidget(daysWidget);
    memoriesPageLayout->addWidget(buttonWidget);

    // add the Memories page to the stacked widget
    stackedWidget->addWidget(memoriesPage);

    return memoriesPage;
}
/*
QWidget* createHomepage(QStackedWidget *stackedWidget, QVideoWidget *videoWidget, ThePlayer *player, std::vector<TheButton*> *buttons, std::vector<TheButtonInfo> *videos) {
    // create the homepage widget
    QWidget *homepage = new QWidget();
    QVBoxLayout *homepageLayout = new QVBoxLayout(homepage);
    // create a QHBoxLayout for the buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // create playback control buttons
    QToolButton *playButton = new QToolButton();
    QToolButton *pauseButton = new QToolButton();

    // create right arrow button
    QToolButton *rightArrowButton = new QToolButton();
    rightArrowButton->setIcon(QIcon("://right.svg"));
    rightArrowButton->setStyleSheet("QToolButton {"
                                    " background-color: #ffffff;"
                                    " border: none;"
                                    "}"
                                    "QToolButton:pressed {"
                                    " background-color: #606060;"
                                    "}");

    // Connect the clicked signal of the right arrow button to move to the next video
    QObject::connect(rightArrowButton, &QToolButton::clicked, [=]() {
        QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
        for (size_t i = 0; i < buttons->size(); ++i) {
            if (buttons->at(i)->info->url->toString() == currentVideoUrl) {
                int nextIndex = (i + 1) % buttons->size();
                TheButtonInfo* nextVideo = buttons->at(nextIndex)->info;
                player->jumpTo(nextVideo);
                break;
            }
        }
    });

    // create left arrow button
    QToolButton *leftArrowButton = new QToolButton();
    leftArrowButton->setIcon(QIcon("://left.svg"));
    leftArrowButton->setStyleSheet("QToolButton {"
                                   " background-color: #ffffff;"
                                   " border: none;"
                                   "}"
                                   "QToolButton:pressed {"
                                   " background-color: #606060;"
                                   "}");

    // Connect the clicked signal of the left arrow button to move to the previous video
    QObject::connect(leftArrowButton, &QToolButton::clicked, [=]() {
        QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
        for (size_t i = 0; i < buttons->size(); ++i) {
            if (buttons->at(i)->info->url->toString() == currentVideoUrl) {
                int prevIndex = (i - 1 + buttons->size()) % buttons->size(); // Handle wrapping around to the end
                TheButtonInfo* prevVideo = buttons->at(prevIndex)->info;
                player->jumpTo(prevVideo);
                break;
            }
        }
    });

    // add the left arrow button to the button layout
    buttonLayout->addWidget(leftArrowButton);

    // add the right and left arrow buttons to the button layout
    buttonLayout->addWidget(leftArrowButton);
    buttonLayout->addWidget(rightArrowButton);

    // set the icons of the buttons
    playButton->setIcon(QIcon("://play-circle-fill.svg"));
    pauseButton->setIcon(QIcon("://pause-circle-fill.svg"));

    // set the stylesheet of the buttons
    playButton->setStyleSheet("QToolButton {"
                              " background-color: #ffffff;"
                              " border: none;"
                              "}"
                              "QToolButton:pressed {"
                              " background-color: #606060;"
                              "}");
    pauseButton->setStyleSheet("QToolButton {"
                               " background-color: #ffffff;"
                               " border: none;"
                               "}"
                               "QToolButton:pressed {"
                               " background-color: #606060;"
                               "}");

    // connect the clicked signals of the buttons to the slots of ThePlayer
    QObject::connect(playButton, &QToolButton::clicked, player, &ThePlayer::play);
    QObject::connect(pauseButton, &QToolButton::clicked, player, &ThePlayer::pause);

    // add the buttons to the button layout
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(pauseButton);

    // create a QWidget for the buttons
    QWidget *buttonWidget = new QWidget();
    buttonWidget->setLayout(buttonLayout);

    // Create QLabel for subtitle
    QLabel *subtitleLabel = new QLabel;
    subtitleLabel->setAlignment(Qt::AlignCenter);

    // Create like button and QLabel for likes
    QPushButton* likeButton = new QPushButton();
    QLabel *likeLabel = new QLabel;

    // Create a QHBoxLayout for subtitle and likes
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addWidget(subtitleLabel);
    infoLayout->addStretch(); // Add a stretchable space between subtitle and likes
    infoLayout->addWidget(likeButton);
    infoLayout->addWidget(likeLabel);

    // Create a QWidget to hold the subtitle and likes layout
    QWidget *infoWidget = new QWidget;
    infoWidget->setLayout(infoLayout);

    // Add the video widget, button widget, and info widget to the homepage layout
    homepageLayout->addWidget(videoWidget);
    homepageLayout->addWidget(buttonWidget);
    homepageLayout->addWidget(infoWidget);

    // connect the recordButton and memoriesButton to switch pages
    QObject::connect(rightArrowButton, &QToolButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(1); // switch to the Record Video page
    });

    QObject::connect(leftArrowButton, &QToolButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(2); // switch to the Memories page
    });

    // connect the homeButton to switch back to the homepage
    QObject::connect(player, &ThePlayer::mediaChanged, [=]() {
        // Update subtitle and likes when the media changes
        const QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
        for (size_t i = 0; i < videos->size(); ++i) {
            if (videos->at(i).url->toString() == currentVideoUrl) {
                subtitleLabel->setText(videos->at(i).subtitle);
                likeLabel->setText(QString::number(videos->at(i).likes));
                break;
            }
        }
    });

    // add the homepage to the stacked widget
    stackedWidget->addWidget(homepage);

    // Set initial subtitle and likes for the first video
    if (!videos->empty()) {
        subtitleLabel->setText(videos->at(0).subtitle);
        likeLabel->setText(QString::number(videos->at(0).likes));
    }

    return homepage;
}

*/

void setInitialSubtitleAndLikes(ThePlayer *player, QLabel *subtitleLabel, QLabel *likeLabel, const std::vector<TheButtonInfo> *videos) {
    const QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
    for (size_t i = 0; i < videos->size(); ++i) {
        if (videos->at(i).url->toString() == currentVideoUrl) {
            subtitleLabel->setText(videos->at(i).subtitle);
            likeLabel->setText(QString::number(videos->at(i).likes));
            break;
        }
    }
}





QWidget* createHomepage(QStackedWidget *stackedWidget, QVideoWidget *videoWidget, ThePlayer *player, std::vector<TheButton*> *buttons, std::vector<TheButtonInfo> *videos) {
    // create the homepage widget
    QWidget *homepage = new QWidget();
    QVBoxLayout *homepageLayout = new QVBoxLayout(homepage);
    // create a QHBoxLayout for the buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // create playback control buttons
    QToolButton *playButton = new QToolButton();
    QToolButton *pauseButton = new QToolButton();

    // create right arrow button
    QToolButton *rightArrowButton = new QToolButton();
    rightArrowButton->setIcon(QIcon("://right.svg"));
    rightArrowButton->setStyleSheet("QToolButton {"
                                    " background-color: #ffffff;"
                                    " border: none;"
                                    "}"
                                    "QToolButton:pressed {"
                                    " background-color: #606060;"
                                    "}");

    // Connect the clicked signal of the right arrow button to move to the next video
    QObject::connect(rightArrowButton, &QToolButton::clicked, [=]() {
        QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
        for (size_t i = 0; i < buttons->size(); ++i) {
            if (buttons->at(i)->info->url->toString() == currentVideoUrl) {
                int nextIndex = (i + 1) % buttons->size();
                TheButtonInfo* nextVideo = buttons->at(nextIndex)->info;
                player->jumpTo(nextVideo);
                break;
            }
        }
    });

    // create left arrow button
    QToolButton *leftArrowButton = new QToolButton();
    leftArrowButton->setIcon(QIcon("://left.svg"));
    leftArrowButton->setStyleSheet("QToolButton {"
                                   " background-color: #ffffff;"
                                   " border: none;"
                                   "}"
                                   "QToolButton:pressed {"
                                   " background-color: #606060;"
                                   "}");

    // Connect the clicked signal of the left arrow button to move to the previous video
    QObject::connect(leftArrowButton, &QToolButton::clicked, [=]() {
        QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
        for (size_t i = 0; i < buttons->size(); ++i) {
            if (buttons->at(i)->info->url->toString() == currentVideoUrl) {
                int prevIndex = (i - 1 + buttons->size()) % buttons->size(); // Handle wrapping around to the end
                TheButtonInfo* prevVideo = buttons->at(prevIndex)->info;
                player->jumpTo(prevVideo);
                break;
            }
        }
    });



    // add the left arrow button to the button layout
    buttonLayout->addWidget(leftArrowButton);

    // add the right and left arrow buttons to the button layout
    buttonLayout->addWidget(leftArrowButton);
    buttonLayout->addWidget(rightArrowButton);

    // set the icons of the buttons
    playButton->setIcon(QIcon("://play-circle-fill.svg"));
    pauseButton->setIcon(QIcon("://pause-circle-fill.svg"));

    // set the stylesheet of the buttons
    playButton->setStyleSheet("QToolButton {"
                              " background-color: #ffffff;"
                              " border: none;"
                              "}"
                              "QToolButton:pressed {"
                              " background-color: #606060;"
                              "}");
    pauseButton->setStyleSheet("QToolButton {"
                               " background-color: #ffffff;"
                               " border: none;"
                               "}"
                               "QToolButton:pressed {"
                               " background-color: #606060;"
                               "}");

    // connect the clicked signals of the buttons to the slots of ThePlayer
    QObject::connect(playButton, &QToolButton::clicked, player, &ThePlayer::play);
    QObject::connect(pauseButton, &QToolButton::clicked, player, &ThePlayer::pause);

    // add the buttons to the button layout
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(pauseButton);

    // create a QWidget for the buttons
    QWidget *buttonWidget = new QWidget();
    buttonWidget->setLayout(buttonLayout);

    // Create QLabel for subtitle
    QLabel *subtitleLabel = new QLabel;
    subtitleLabel->setAlignment(Qt::AlignCenter);

    // Create like button and QLabel for likes
    QPushButton* likeButton = new QPushButton();
    likeButton->setIcon(QIcon(":/heart.png"));
    QLabel *likeLabel = new QLabel;

    // Create a QHBoxLayout for subtitle and likes
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addWidget(subtitleLabel);
    infoLayout->addStretch(); // Add a stretchable space between subtitle and likes
    infoLayout->addWidget(likeButton);
    infoLayout->addWidget(likeLabel);

    // Create a QWidget to hold the subtitle and likes layout
    QWidget *infoWidget = new QWidget;
    infoWidget->setLayout(infoLayout);

    // Connect signals to update subtitle and likes when the media changes
    QObject::connect(player, &ThePlayer::mediaChanged, [=]() {
        const QString currentVideoUrl = player->currentMedia().canonicalUrl().toString();
        for (size_t i = 0; i < videos->size(); ++i) {
            if (videos->at(i).url->toString() == currentVideoUrl) {
                subtitleLabel->setText(videos->at(i+1).subtitle);
                likeLabel->setText(QString::number(videos->at(i+1).likes));
                break;
            }
        }
    });

    // Add the video widget, button widget, and info widget to the homepage layout
    homepageLayout->addWidget(videoWidget);
    homepageLayout->addWidget(buttonWidget);
    homepageLayout->addWidget(infoWidget);

    // add the homepage to the stacked widget
    stackedWidget->addWidget(homepage);

    // Set initial subtitle and likes for the first video
    if (!videos->empty()) {
        subtitleLabel->setText(videos->at(0).subtitle);
        likeLabel->setText(QString::number(videos->at(0).likes));
    }

    // Trigger the initial update for the first video when the application is opened
    player->simulateMediaChanged();
    return homepage;
}







QWidget* createRecordVideoPage(QStackedWidget *stackedWidget, Recorder *recorder) {
    // create the Record Video page widget
    QWidget *recordVideoPage = new QWidget();
    QVBoxLayout *recordVideoPageLayout = new QVBoxLayout(recordVideoPage);
    QPushButton *startButton = new QPushButton("Start Recording", recordVideoPage);
    QAbstractButton::connect(startButton, &QPushButton::clicked, recorder, &Recorder::startRecording);
    recordVideoPageLayout->addWidget(startButton);
    QPushButton *stopButton = new QPushButton("Stop Recording", recordVideoPage);
    QAbstractButton::connect(stopButton, &QPushButton::clicked, recorder, &Recorder::stopRecording);
    recordVideoPageLayout->addWidget(stopButton);

    // add a QLabel to the Record Video page
    QLabel *labelRecord = new QLabel("You are now on the Record Video page", recordVideoPage);
    recordVideoPageLayout->addWidget(labelRecord);

    // add the Record Video page to the stacked widget
    stackedWidget->addWidget(recordVideoPage);
    //TODO: add caption to video/post button
    //TODO: add return button
    return recordVideoPage;
}

QWidget* createSearchPage(QStackedWidget *stackedWidget) {
    // create the Search page widget
    QWidget *searchPage = new QWidget();
    QVBoxLayout *searchPageLayout = new QVBoxLayout(searchPage);

    // create a QLineEdit for the search bar
    QLineEdit *searchBar = new QLineEdit(searchPage);
    searchBar->setPlaceholderText("Search for a user...");

    // create a QStringList to store the dummy user data
    QStringList users;
    users << "John Doe" << "David Parks" << "Emma Johnson" << "Michael Brown" << "Sophia Miller" << "James Wilson" << "Olivia Taylor" << "Liam Thompson" << "Ava Anderson" << "William Harris" << "Isabella Garcia" << "Mason Moore" << "Mia Clark" << "Ethan Martinez" << "Emily Rodriguez" << "Jacob Lewis" << "Sofia Lee" << "Logan Robinson" << "Charlotte Turner" << "Ethan Torres" << "Grace Martinez" << "Daniel Garcia" << "Madison Wilson" << "Levi Torres" << "Abigail Clark" << "Matthew Martinez" << "Harper Rodriguez" << "Jack Lewis" << "Ella Moore" << "Ethan Torres" << "Grace Martinez" << "Daniel Garcia" << "Madison Wilson" << "Levi Torres" << "Abigail Clark" << "Matthew Martinez" << "Harper Rodriguez" << "Jack Lewis" << "Ella Moore" << "Ethan Torres" << "Grace Martinez" << "Daniel Garcia" << "Madison Wilson" << "Levi Torres" << "Abigail Clark" << "Matthew Martinez" << "Harper Rodriguez" << "Jack Lewis" << "Ella Moore";
    // create a QStringList to store the dummy user usernames
    QStringList usernames;
    for (const QString &user : users) {
        QString username = "@" + user.toLower().replace(" ", "_");
        usernames << username;
    }
    // create a QListWidget to list all users
    QListWidget *userList = new QListWidget(searchPage);

    // create a QCompleter to provide auto-completion suggestions
    QCompleter *completer = new QCompleter(users, searchPage);
    searchBar->setCompleter(completer);

    // add the search bar to the Search page
    searchPageLayout->addWidget(searchBar);

    // add a search button to the Search page
    QPushButton *searchButton = new QPushButton(QIcon(":/path/to/magnifying/glass/icon.png"), "", searchPage);
    searchPageLayout->addWidget(searchButton);

    // add a QScrollArea to make the page scrollable
    QScrollArea *scrollArea = new QScrollArea(searchPage);
    QWidget *scrollWidget = new QWidget(scrollArea);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    // add the QListWidget to the QScrollArea
    scrollLayout->addWidget(userList);

    // add the QScrollArea to the Search page
    searchPageLayout->addWidget(scrollArea);

    // add the Search page to the stacked widget
    stackedWidget->addWidget(searchPage);

    // connect the textChanged signal of the QLineEdit to the slot function that filters the users
    QAbstractButton::connect(searchBar, &QLineEdit::textChanged, [=](const QString &text) {
        for (int i = 0; i < userList->count(); ++i) {
            QListWidgetItem *item = userList->item(i);
            QWidget *userWidget = userList->itemWidget(item);
            QLabel *userLabel = userWidget->findChild<QLabel*>();
            item->setHidden(!userLabel->text().contains(text, Qt::CaseInsensitive));
        }
    });

    // connect the clicked signal of the search button to the slot function that filters the users
    QAbstractButton::connect(searchButton, &QPushButton::clicked, [=]() {
        for (int i = 0; i < userList->count(); ++i) {
            QListWidgetItem *item = userList->item(i);
            QWidget *userWidget = userList->itemWidget(item);
            QLabel *userLabel = userWidget->findChild<QLabel*>();
            item->setHidden(!userLabel->text().contains(searchBar->text(), Qt::CaseInsensitive));
        }
    });

    // add all users to the QListWidget
    for (int i = 0; i < users.size(); ++i) {
        QWidget *userWidget = new QWidget(userList);
        QHBoxLayout *userLayout = new QHBoxLayout(userWidget);

        QLabel *userLabel = new QLabel(users.at(i) + " " + usernames.at(i), userWidget);
        userLayout->addWidget(userLabel);

        QPushButton *sendRequestButton = new QPushButton("Send friend request", userWidget);
        userLayout->addWidget(sendRequestButton);

        QListWidgetItem *item = new QListWidgetItem(userList);
        item->setSizeHint(userWidget->sizeHint());
        userList->addItem(item);
        userList->setItemWidget(item, userWidget);
    }


    return searchPage;
}


int main(int argc, char *argv[]) {
    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << Qt::endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // Show the login dialog
    /*if (!showLoginDialog()) {
        return 1; // If the user is denied access, close the app
    }*/

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {
        const int result = QMessageBox::information(
            NULL,
            QString("Tomeo"),
            QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setFixedSize(540, 960);

    // create navbar
    QWidget *navbar = new QWidget();
    navbar->setFixedHeight(50);
    QHBoxLayout *navbarLayout = new QHBoxLayout(navbar);

    // create buttons for navbar
    QPushButton *homeButton = new QPushButton("Home", navbar);
    QPushButton *recordButton = new QPushButton("Record Video", navbar);
    QPushButton *memoriesButton = new QPushButton("Memories", navbar);
    QPushButton *searchButton = new QPushButton("Search", navbar);

    navbarLayout->addWidget(homeButton);
    navbarLayout->addWidget(recordButton);
    navbarLayout->addWidget(memoriesButton);
    navbarLayout->addWidget(searchButton);

    // create the stacked widget
    QStackedWidget *stackedWidget = new QStackedWidget(&window);

    // create buttonWidget
    QWidget *buttonWidgetmem = new QWidget();
    QHBoxLayout *layoutmem = new QHBoxLayout();
    buttonWidgetmem->setLayout(layoutmem);

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    QHBoxLayout *layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    Recorder *recorder = new Recorder;

    // create the homepage widget
    createHomepage(stackedWidget, videoWidget, player, &buttons, &videos);

    // create the Record Video page widget
    createRecordVideoPage(stackedWidget, recorder);

    // create the Memories page widget
    createMemoriesPage(stackedWidget, buttonWidgetmem);

    // create the Search page widget
    createSearchPage(stackedWidget);

    // connect the recordButton and memoriesButton to switch pages
    QObject::connect(recordButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(1); // switch to the Record Video page
    });

    QObject::connect(memoriesButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(2); // switch to the Memories page
    });

    // connect the homeButton to switch to the homepage
    QObject::connect(homeButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(0); // switch to the homepage
    });

    // connect the searchButton to switch to the Search page
    QObject::connect(searchButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(3); // switch to the Search page
    });

    // add navbar to the top layout of your window
    top->addWidget(navbar);









    // create the four buttons. DO NOT DELETE
    for ( int i = 0; i < 4; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    // add the stacked widget to the top level widget
    top->addWidget(stackedWidget);

    // show the homepage
    stackedWidget->setCurrentIndex(0);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
