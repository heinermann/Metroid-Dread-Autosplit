#pragma once
#include "opencv2/opencv.hpp"
#include<QObject>

class VideoReader : public QObject {
    Q_OBJECT
private:
    void update();
    cv::VideoCapture capture;
    cv::Mat frame;
    bool readCapture;
public:
    VideoReader(int source, cv::Mat& frame);
    ~VideoReader();
    cv::Mat read();
    void startCapture();
    bool isCapturing();
public slots:
    void stopReading();
    void process();
signals:
    void finished();
    void error(QString err);
};