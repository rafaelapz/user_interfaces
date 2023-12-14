#ifndef RECORDVIDEOPAGE_H
#define RECORDVIDEOPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class Recorder;

class RecordVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit RecordVideoPage(Recorder* recorder, QWidget *parent = nullptr);

private:
    QPushButton *startButton;
    QLabel *label;
};

#endif // RECORDVIDEOPAGE_H
