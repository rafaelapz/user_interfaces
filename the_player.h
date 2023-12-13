#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "qslider.h"
#include "the_button.h"
#include <vector>
#include <QTimer>

class ThePlayer : public QMediaPlayer {

    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    long updateCount = 0;
    bool firstMediaChanged;

public:
    ThePlayer() : QMediaPlayer(NULL) {
        setVolume(0);
        connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playStateChanged(QMediaPlayer::State)));

        mTimer = new QTimer(NULL);
        mTimer->setInterval(1000);
        mTimer->start();
        connect(mTimer, SIGNAL(timeout()), SLOT(shuffle())); // changed to use shuffle directly
    }

    void play();
    void pause();

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);


private slots:
    void playStateChanged(QMediaPlayer::State ms);

public slots:
    // start playing this ButtonInfo
    void jumpTo(TheButtonInfo* button);
    void simulateMediaChanged();
};

#endif //CW2_THE_PLAYER_H
