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
#include <QLineEdit>
#include <QStandardItemModel>
#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QListWidget>
#include <QScrollArea>
#include <QToolButton>
#include "login.h"

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

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                QImage sprite = imageReader->read(); // read the thumbnail
                if (!sprite.isNull()) {
                    QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                    QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                    out . push_back(TheButtonInfo( url , ico ) ); // add to the output list
                }
                else
                    qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << "\n";
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << Qt::endl;
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

QWidget* createHomepage(QStackedWidget *stackedWidget, QVideoWidget *videoWidget, ThePlayer *player, std::vector<TheButton*> *buttons, std::vector<TheButtonInfo> *videos) {
    // create the homepage widget
    QWidget *homepage = new QWidget();
    QVBoxLayout *homepageLayout = new QVBoxLayout(homepage);
    // create a QHBoxLayout for the buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // create playback control buttons
    QToolButton *playButton = new QToolButton();
    QToolButton *pauseButton = new QToolButton();

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

    // add the video widget and the button widget to the homepage layout
    homepageLayout->addWidget(videoWidget);
    homepageLayout->addWidget(buttonWidget);

    // add the homepage to the stacked widget
    stackedWidget->addWidget(homepage);

    return homepage;
}

QWidget* createRecordVideoPage(QStackedWidget *stackedWidget, Recorder *recorder) {
    // create the Record Video page widget
    QWidget *recordVideoPage = new QWidget();
    QVBoxLayout *recordVideoPageLayout = new QVBoxLayout(recordVideoPage);
    //comment out the connections of buttons as they crash the app when camera is not connected on linux when clicked
    //QAbstractButton::connect(startButton, &QPushButton::clicked, recorder, &Recorder::startRecording);
    QPushButton *startButton = new QPushButton("Start Recording", recordVideoPage);
    recordVideoPageLayout->addWidget(startButton);
    //QAbstractButton::connect(stopButton, &QPushButton::clicked, recorder, &Recorder::stopRecording);
    QPushButton *stopButton = new QPushButton("Stop Recording", recordVideoPage);
    recordVideoPageLayout->addWidget(stopButton);

    // add a QLabel to the Record Video page
    QLabel *labelRecord = new QLabel("You are now on the Record Video page", recordVideoPage);
    recordVideoPageLayout->addWidget(labelRecord);

    // add a QLineEdit for the video caption
    QLineEdit *captionField = new QLineEdit("", recordVideoPage);
    captionField->setPlaceholderText("Enter video caption here...");

    recordVideoPageLayout->addWidget(captionField);

    // add a QPushButton for the "Submit Post" button
    QPushButton *submitPostButton = new QPushButton("Submit Post", recordVideoPage);
    recordVideoPageLayout->addWidget(submitPostButton);

    // add the Record Video page to the stacked widget
    stackedWidget->addWidget(recordVideoPage);

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

    app.setStyleSheet("QWidget { background-color: black; color: white; }"
                      "QPushButton { border-style: solid; border-width: 2px; border-color: white;}"
                      "QPushButton:hover { background-color: white; color: black; }");

    // Show the login dialog
    if (!showLoginDialog()) {
        return 1; // If the user is denied access, close the app
    }

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {
        const int result = QMessageBox::information(
            NULL,
            QString("VidReal"),
            QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("VidReal");
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

