#pragma once
#include "opencv2/opencv.hpp"
#include "VideoReader.h"
#include<vector>
#include"SplitImage.h"

class FrameProcesser : public QObject {
        Q_OBJECT
private:
    const double DUMMY_IMAGE_THRESHOLD{ .85 };
    VideoReader& reader;

    int splitHotkey;
    int pauseHotkey;

    cv::Mat mask;
    cv::Mat frame;
    cv::Mat compare;
    std::vector<SplitImage> splitImages;

    double getSimilarity(const cv::Mat A, const cv::Mat B, const cv::Mat m = cv::Mat());
    void sendKeyboardInput(HOTKEY_TYPE);
    bool isBlack(const cv::Mat frame);

public slots:
    void process();
signals:
    void finished();
    void error(QString err);
public:
    FrameProcesser(std::vector<SplitImage> splitImages, VideoReader& reader, int splitHotkey, int pauseHotkey);
    ~FrameProcesser();
};