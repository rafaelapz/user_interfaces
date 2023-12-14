//
//
//

#include "the_button.h"


void TheButton::init(TheButtonInfo* i) {
    setIcon( *(i->icon) );
    info =  i;
}



void TheButton::clicked() {
    emit jumpTo(info);
}



void TheButton::likeButtonClicked() {
    // Toggle user's like state (increase or decrease based on the current state)
    info->likes += (info->userLiked) ? -1 : 1;
    info->userLiked = !info->userLiked;
}
