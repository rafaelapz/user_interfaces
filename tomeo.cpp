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

QWidget* createHomepage(QStackedWidget *stackedWidget, QVideoWidget *videoWidget, QWidget *buttonWidget, ThePlayer *player, std::vector<TheButton*> *buttons, std::vector<TheButtonInfo> *videos) {
    // create the homepage widget
    QWidget *homepage = new QWidget();
    QVBoxLayout *homepageLayout = new QVBoxLayout(homepage);
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

int main(int argc, char *argv[]) {
    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

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
    createHomepage(stackedWidget, videoWidget, buttonWidget, player, &buttons, &videos);

    // create the Record Video page widget
    createRecordVideoPage(stackedWidget, recorder);

    // create the Memories page widget
    createMemoriesPage(stackedWidget, buttonWidgetmem);

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

    // add navbar to the top layout of your window
    top->addWidget(navbar);









    // create the four buttons
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

