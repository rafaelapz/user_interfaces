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

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
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

    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setFixedSize(540, 960);

    // create the stacked widget
    QStackedWidget *stackedWidget = new QStackedWidget(&window);

    // create the homepage widget
    QWidget *homepage = new QWidget(&window);
    QVBoxLayout *homepageLayout = new QVBoxLayout(homepage);
    homepageLayout->addWidget(videoWidget);
    homepageLayout->addWidget(buttonWidget);

    // add the homepage to the stacked widget
    stackedWidget->addWidget(homepage);

    // create the Record Video page widget
    QWidget *recordVideoPage = new QWidget(&window);
    QVBoxLayout *recordVideoPageLayout = new QVBoxLayout(recordVideoPage);
    QPushButton *startButton = new QPushButton("Start Recording", recordVideoPage);
    QAbstractButton::connect(startButton, &QPushButton::clicked, recorder, &Recorder::startRecording);
    recordVideoPageLayout->addWidget(startButton);
    QPushButton *stopButton = new QPushButton("Stop Recording", recordVideoPage);
    QAbstractButton::connect(stopButton, &QPushButton::clicked, recorder, &Recorder::stopRecording);
    recordVideoPageLayout->addWidget(stopButton);

    //TODO: add caption to video/post button
    //TODO: add return button

    // add a QLabel to the Record Video page
    QLabel *label = new QLabel("You are now on the Record Video page", recordVideoPage);
    recordVideoPageLayout->addWidget(label);

    // add the Record Video page to the stacked widget
    stackedWidget->addWidget(recordVideoPage);

    // create the Record Video button
    QPushButton *recordButton = new QPushButton("Record Video", &window);
    QAbstractButton::connect(recordButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(1); // switch to the Record Video page
    });
    top->addWidget(recordButton);


    // add the stacked widget to the top level widget
    top->addWidget(stackedWidget);

    // show the homepage
    stackedWidget->setCurrentIndex(0);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
