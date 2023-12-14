#ifndef LIKESUBTITLE_H
#define LIKESUBTITLE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class LikeSubtitleWidget : public QWidget {
    Q_OBJECT
public:
    LikeSubtitleWidget(int likes, const QString &subtitle, QWidget *parent = nullptr);

    void setLikes(int likes);
    void setSubtitle(const QString &subtitle);

private:
    QPushButton *likeButton;
    QLabel *subtitleLabel;
};

#endif // LIKESUBTITLE_H
