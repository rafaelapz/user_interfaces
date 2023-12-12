#include "recorder.h"
#include <QCamera>
#include <QMediaRecorder>
#include <QUrl>
#include <QCameraInfo>

Recorder::Recorder(QObject *parent)
    : QObject(parent)
{
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (cameras.isEmpty()) {
        qDebug() << "No cameras found";
        return;
    }

    camera = new QCamera(cameras.first(), this);
    recorder = new QMediaRecorder(camera, this);

    connect(camera, &QCamera::stateChanged, this, &Recorder::handleCameraStateChanged);
    connect(recorder, &QMediaRecorder::stateChanged, this, &Recorder::handleRecorderStateChanged);

    // Set output location for the recording
    recorder->setOutputLocation(QUrl::fromLocalFile("/path/to/save/video.mp4"));
}

void Recorder::startRecording()
{
    // Start recording
    if (recorder->state() != QMediaRecorder::RecordingState) {
        recorder->record();
    }
}

void Recorder::stopRecording()
{
    // Stop recording
    if (recorder->state() == QMediaRecorder::RecordingState) {
        recorder->stop();
    }
}

void Recorder::handleCameraStateChanged(QCamera::State state)
{
    if (state == QCamera::ActiveState) {
        startRecording();
    }
}

void Recorder::handleRecorderStateChanged(QMediaRecorder::State state)
{
    if (state == QMediaRecorder::RecordingState) {
        qDebug() << "Recording started";
    } else if (state == QMediaRecorder::StoppedState) {
        qDebug() << "Recording stopped";
    }
}
