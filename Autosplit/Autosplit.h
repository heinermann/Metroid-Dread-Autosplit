#pragma once

#include <QtWidgets/QMainWindow>
#include<QButtonGroup>
#include "ui_Autosplit.h"
#include"FrameProcesser.h"
#include"VideoReader.h"
#include"opencv2/opencv.hpp"
#include "Backend.h"
#include"Route.h"
#include"Camera_MSMF.h"
#include<vector>
#include<map>

class Autosplit : public QMainWindow
{
    Q_OBJECT

public:
    Autosplit(QWidget *parent = Q_NULLPTR);
    void changeColor(QWidget* mbutton, const QString color);
    void changeTextColor(QPushButton* mbutton, const QString color);
    bool eventFilter(QObject* watched, QEvent* event);
    void updateTransports();
    QFrame* getRouteSegmentFrame();
    void displayRoute();
    void enableEditRouteButtons();
    void disableEditRouteButtons();
    QString routeToTextReadyString();
    int videoCaptureDialog();
    void setLivePreview();
    void addDisplayCaptureSelectButton(QWidget* parent,  int source, QString deviceName);
    int hotkeyDialog();
    void errorMessageDialog();
    void autosplitRunningDialog();
    void startMainAutosplitterThreads();
    void loadSettingsFile(QString fileName);
public slots:
    void processError(QString err);
    void endAutosplitter();

    void tabButtonPress();
    void tabButtonRelease();

    void videoCaptureSelectClick();

    void previewDialogClose();

    void doubleClick(QMouseEvent* event);

    void routeButtonToggle();
    void routeButtonPress();

    void routeSegmentButtonToggle();
    void routeSegmentButtonPress();
    void routeSegmentButtonRelease();

    void newRouteButtonClick();
    void loadRouteButtonClick();
    void readRouteFile(QString& fileName);
    void saveRouteButtonClick();

    void routeEditButtonClick();
    void routeEditButtonPress();
    void routeEditButtonRelease();

    void continueButtonClick();

    void splitSetHotkeyClick();
    void pauseSetHotkeyClick();

    void saveSettingsClick();
    void writeSettingsToFile(QString fileName);
    void loadSettingsClick();

    void startAutosplitterClick();

    void routeEraseClick();


    QPixmap getIconFromHTML(QString text);
private:
    Ui::AutosplitClass ui;
    Backend backend;
    QButtonGroup sourceButtons;
    QButtonGroup destButtons;
    QButtonGroup transportButtons; 
    QButtonGroup routeButtons;
    QButtonGroup captureSelectButtons;
    Route route;
    QString loadedRouteFileName;
    QString loadedSettingsFileName;
    int videoSource;
    std::map<QPushButton*, int> videoImageSources;
    std::map<int, msmf::Device> devices;
    std::map<QPushButton*, int> captureSelectButtonMap;
    cv::VideoCapture capture;
    QTimer* videoPreviewTimer;
    double fps;
    int splitHotkey;
    int pauseHotkey;
    QThread* mainAutosplitterThreads;
    bool checkPreviousSettingsUsed();
    QString getSettingsString();
    void createErrorDialog(QString err);
    VideoReader* reader;
};
