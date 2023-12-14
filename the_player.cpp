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

// change the image and video for one button every one second
/* void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
    //        setMedia(*i->url);
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
} */

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
    case QMediaPlayer::State::StoppedState:
        play(); // starting playing again...
        break;
    default:
        break;
    }
}
void ThePlayer::simulateMediaChanged() {
    emit mediaStatusChanged(QMediaPlayer::LoadedMedia);
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

