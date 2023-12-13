//
//
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>

class TheButtonInfo {
public:
    QUrl* url; // video file to play
    QIcon* icon; // icon to display
    int likes; // number of likes
    QString subtitle; // subtitle

    TheButtonInfo(QUrl* url, QIcon* icon, int likes, std::string subtitle)
        : url(url), icon(icon), likes(likes), subtitle(QString::fromStdString(subtitle)) {}

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

private slots:
    void clicked();

signals:
    void jumpTo(TheButtonInfo*);

};

#endif //CW2_THE_BUTTON_H
