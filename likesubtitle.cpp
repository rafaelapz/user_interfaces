#include "likesubtitle.h"
#include <QVBoxLayout>

LikeSubtitleWidget::LikeSubtitleWidget(int likes, const QString &subtitle, QWidget *parent)
    : QWidget(parent)
{
    likeButton = new QPushButton(QString::number(likes), this);
    subtitleLabel = new QLabel(subtitle, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(likeButton);
    layout->addWidget(subtitleLabel);

    setLikes(likes);
    setSubtitle(subtitle);
}



void LikeSubtitleWidget::setLikes(int likes) {
    likeButton->setText(QString::number(likes));
}

void LikeSubtitleWidget::setSubtitle(const QString &subtitle) {
    subtitleLabel->setText(subtitle);
}
