#include "recordvideopage.h"
#include "recorder.h"
#include <QVBoxLayout>

RecordVideoPage::RecordVideoPage(Recorder* recorder, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *RecordVideoPagelayout = new QVBoxLayout(this);

    startButton = new QPushButton("Start Recording", this);
    QAbstractButton::connect(startButton, &QPushButton::clicked, recorder, &Recorder::startRecording);
    RecordVideoPagelayout->addWidget(startButton);

    label = new QLabel("You are now on the Record Video page", this);
    RecordVideoPagelayout->addWidget(label);
}

