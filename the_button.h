//
//
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
class TheButtonInfo {
public:
    QUrl* url;
    QIcon* icon;
    int likes;
    QString subtitle;
    bool userLiked; // Added variable to track user's like state

    TheButtonInfo(QUrl* url, QIcon* icon, int likes, std::string subtitle)
        : url(url), icon(icon), likes(likes), subtitle(QString::fromStdString(subtitle)), userLiked(false) {}

};



class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;

     TheButton(QWidget *parent) :  QPushButton(parent) {
         setIconSize(QSize(200,110));
         connect(this, SIGNAL(released()), this, SLOT (clicked() )); // if QPushButton clicked...then run clicked() below
    }

    void init(TheButtonInfo* i);
    void likeButtonClicked();

private slots:
    void clicked();


signals:
    void jumpTo(TheButtonInfo*);

};

#endif //CW2_THE_BUTTON_H
