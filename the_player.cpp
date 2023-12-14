//
//
//

#include "the_player.h"
#include "qslider.h"



// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}


void ThePlayer::simulateMediaChanged() {
    emit mediaStatusChanged(QMediaPlayer::LoadedMedia);
}
void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
    case QMediaPlayer::State::StoppedState:
        play(); // starting playing again...
        break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}

void ThePlayer::play() {
    QMediaPlayer::play();
}

void ThePlayer::pause() {
    QMediaPlayer::pause();
}

