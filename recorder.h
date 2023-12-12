#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaRecorder>
#include <QCameraInfo>

class Recorder : public QObject
{
    Q_OBJECT

public:
    explicit Recorder(QObject *parent = nullptr);

public slots:
    void startRecording();
    void stopRecording();
    void handleCameraStateChanged(QCamera::State state);
    void handleRecorderStateChanged(QMediaRecorder::State state);

private:
    QCamera *camera;
    QMediaRecorder *recorder;
};

#endif // RECORDER_H
