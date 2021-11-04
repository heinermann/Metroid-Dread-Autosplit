#include "VideoReader.h"
#include<thread>
#include<vector>
#include"opencv2/opencv.hpp"
#include<qDebug>
#include"Windows.h"

VideoReader::VideoReader(int src, cv::Mat& fr)
	:capture(src), frame(fr), readCapture(true) {}

VideoReader::~VideoReader(){
}

cv::Mat VideoReader::read() {
	if (readCapture)
		return frame;
	else
		return cv::Mat();
}

void VideoReader::startCapture() {
	while(readCapture) {
		bool success = capture.read(frame);
		if (!success) {
			readCapture = false;
			emit finished();
			emit error("Capture device was disconnected and autosplit has stopped. Reconnect your device and try again.");
		}
	}
	emit finished();
}

void VideoReader::process() {
	startCapture();
}


void VideoReader::stopReading() {
	readCapture = false;
}

bool VideoReader::isCapturing()
{
	return readCapture;
}
