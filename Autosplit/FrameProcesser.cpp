#include "FrameProcesser.h"
#include"Windows.h"
#include<qDebug>
#include<qstring.h>

void FrameProcesser::process() {

    bool captureStopped{ false };
    Sleep(100);
    IMAGE_TYPE previous(IMAGE_TYPE::BLACK);
	for (SplitImage image : splitImages) {
		bool foundMatch{ false };

		if (image.imageType == IMAGE_TYPE::COLOR) {
			compare = cv::imread("./SplitImages/"+image.name+".png");
		}
        int interval = 5;
		while (!foundMatch) {
            frame = reader.read();
            if (!frame.empty()) {
                switch (image.imageType) {
                case IMAGE_TYPE::BLACK:
                    if (previous == IMAGE_TYPE::BLACK) {
                        interval = 30;
                    }
                    else if (previous == IMAGE_TYPE::COLOR) {
                        interval = 1; 
                    }
                    foundMatch = isBlack(frame);
                    break;
                case IMAGE_TYPE::NON_BLACK:
                    interval = 1;
                    foundMatch = !(isBlack(frame));
                    break;

                case IMAGE_TYPE::COLOR:
                    double score;
                    if (image.mask == "") {
                        score = getSimilarity(frame, compare);
                    }
                    else {
                        mask = cv::imread("./Masks/" + image.mask);
                        score = getSimilarity(frame, compare, mask);
                    }

                    if (score > DUMMY_IMAGE_THRESHOLD) {
                        foundMatch = true;
                    }
                    break;
                }
                Sleep(interval);
            }
            captureStopped = !reader.isCapturing();
            if (captureStopped) break;
		}
        if (captureStopped) break;
        previous = image.imageType;
        if ( image.hotkey == HOTKEY_TYPE(SPLIT) || image.hotkey == HOTKEY_TYPE(PAUSE) ) {
            sendKeyboardInput(image.hotkey);
        }
        Sleep(image.cooldown);
	}
    qDebug() << "done with frame processing thread";
    emit finished();
}



FrameProcesser::FrameProcesser(std::vector<SplitImage> images, VideoReader& rdr, int splitKey, int pauseKey)
    :splitImages(images), reader(rdr), splitHotkey(splitKey), pauseHotkey(pauseKey) {}

FrameProcesser::~FrameProcesser(){
}


double FrameProcesser::getSimilarity(const cv::Mat A, const cv::Mat B, const cv::Mat mask) {

    cv::Mat scoreImg;
    double maxScore;

    cv::Mat AResized;
    cv::Mat BResized;
    cv::Mat maskResized;


    resize(A, AResized, cv::Size(25, 25), cv::INTER_AREA);
    resize(B, BResized, cv::Size(25, 25), cv::INTER_AREA);
    if (!mask.empty()) 
        resize(mask, maskResized, cv::Size(25, 25), cv::INTER_AREA);

    cvtColor(AResized, AResized, cv::COLOR_BGR2GRAY);
    cvtColor(BResized, BResized, cv::COLOR_BGR2GRAY);
    if (!mask.empty())
        cvtColor(maskResized, maskResized, cv::COLOR_BGR2GRAY);

    if (!mask.empty()) {
        matchTemplate(AResized, BResized, scoreImg, cv::TM_CCOEFF_NORMED, maskResized);
    }
    else {
        matchTemplate(AResized, BResized, scoreImg, cv::TM_CCOEFF_NORMED);
    }

    minMaxLoc(scoreImg, 0, &maxScore);

    return maxScore;
}

void FrameProcesser::sendKeyboardInput(HOTKEY_TYPE hotkeyType) {
    qDebug() << "sending input of type:" << hotkeyType;
    int code;

    code = (hotkeyType == SPLIT) ? splitHotkey : pauseHotkey;

    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
    inputs[0].ki.wVk = code;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

bool FrameProcesser::isBlack(cv::Mat frame) {
    cv::Mat frameGrayscale;
    resize(frame, frameGrayscale, cv::Size(1, 1), cv::INTER_AREA);
    cvtColor(frame, frameGrayscale, cv::COLOR_BGR2GRAY);
    return (countNonZero(frameGrayscale) == 0);
}
