#include "Autosplit.h"
#include<QGraphicsWidget>
#include<QScrollArea>
#include<QScrollBar>
#include<QFileDialog>
#include<QRegularExpression>
#include<iostream>
#include<vector>
#include<regex>
#include "Backend.h"
#include"opencv2/opencv.hpp"
#include"RouteSegment.h"
#include"Route.h"
#include<QSizePolicy>
#include"VideoReader.h"
#include<QTimer>
#include<map>
#include"Camera_MSMF.h"
#include<QMessageBox>
#include<QErrorMessage>

const QString DEFAULT_BUTTON_COLOR{ "background-color: rgb(100,100,100);" };
const QString HOVER_ROUTE_PRESS_COLOR{ "background-color: rgb(120,120,120);" };
const QString HOVER_ROUTE_BTN_LIT_COLOR{ "background-color: rgb(130,130,130);" };
const QString OFF_BUTTON_COLOR{ "background-color: rgb(65,65,65);" };

const QString DEFAULT_TEXT_COLOR{ "color: rgb(255,255,255);" };
const QString OFF_TEXT_COLOR{ "color: rgb(170,170,170);" };

const QString DARK_BUTTON_COLOR{ "background-color: rgb(80,80,80);" };
const QString DARK_BUTTON_LIT_COLOR{ "background-color: rgb(90,90,90);" };
const QString DARK_BUTTON_PRESS_COLOR{ "background-color: rgb(85,85,85);" };

const QString MAIN_TAB_OFF_COLOR{ "background-color: rgb(40,40,40);" };
const QString MAIN_TAB_LIT_COLOR{ "background-color: rgb(50,50,50);" };
const QString MAIN_TAB_PRESS_COLOR{ "background-color: rgb(60,60,60);"};

Autosplit::Autosplit(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    route = Route();
    if (checkPreviousSettingsUsed() == false) {
        pauseHotkey = -1;
        splitHotkey = -1;
        loadedRouteFileName = QString("None");
        writeSettingsToFile(QString("./last-settings-used.txt"));
    }

    loadedSettingsFileName = QString("None");

    sourceButtons.setExclusive(true);
    destButtons.setExclusive(true);
    transportButtons.setExclusive(true);
    routeButtons.setExclusive(true);
    captureSelectButtons.setExclusive(true);

    ui.SelectCaptureDevice->installEventFilter(this);

    ui.InsertBelowButton->installEventFilter(this);
    ui.InsertAboveButton->installEventFilter(this);
    ui.InsertEndButton->installEventFilter(this);
    ui.InsertStartButton->installEventFilter(this);

    ui.AutosplitTab->installEventFilter(this);
    ui.RouteEditorTab->installEventFilter(this);

    ui.SaveRouteButton->installEventFilter(this);
    ui.LoadRouteButton->installEventFilter(this);
    ui.NewRouteButton->installEventFilter(this);

    ui.SplitSetHotkey->installEventFilter(this);
    ui.PauseSetHotkey->installEventFilter(this);

    ui.LoadSettingsButton->installEventFilter(this);
    ui.SaveSettingsButton->installEventFilter(this);

    ui.MainLoadRouteButton->installEventFilter(this);
    ui.StartAutosplit->installEventFilter(this);

    connect(ui.SelectCaptureDevice, &QPushButton::clicked, this, &Autosplit::videoCaptureSelectClick);
    connect(ui.SelectCaptureDevice, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.SelectCaptureDevice, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.SplitSetHotkey, &QPushButton::clicked, this, &Autosplit::splitSetHotkeyClick);
    connect(ui.SplitSetHotkey, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.SplitSetHotkey, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.PauseSetHotkey, &QPushButton::clicked, this, &Autosplit::pauseSetHotkeyClick);
    connect(ui.PauseSetHotkey, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.PauseSetHotkey, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.LoadSettingsButton, &QPushButton::clicked, this, &Autosplit::loadSettingsClick);
    connect(ui.LoadSettingsButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.LoadSettingsButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.SaveSettingsButton, &QPushButton::clicked, this, &Autosplit::saveSettingsClick);
    connect(ui.SaveSettingsButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.SaveSettingsButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.MainLoadRouteButton, &QPushButton::clicked, this, &Autosplit::loadRouteButtonClick);
    connect(ui.MainLoadRouteButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.MainLoadRouteButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.StartAutosplit, &QPushButton::clicked, this, &Autosplit::startAutosplitterClick);
    connect(ui.StartAutosplit, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.StartAutosplit, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    connect(ui.AutosplitTab, &QPushButton::pressed, this, &Autosplit::tabButtonPress);
    //connect(ui.AutosplitTab, &QPushButton::clicked, this, &Autosplit::tabButtonClick);

    connect(ui.RouteEditorTab, &QPushButton::pressed, this, &Autosplit::tabButtonPress);
    //connect(ui.RouteEditorTab, &QPushButton::clicked, this, &Autosplit::tabButtonClick);


    connect(ui.InsertStartButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.InsertAboveButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.InsertBelowButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.InsertEndButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);

    connect(ui.InsertStartButton, &QPushButton::clicked, this, &Autosplit::routeEditButtonClick);
    connect(ui.InsertAboveButton, &QPushButton::clicked, this, &Autosplit::routeEditButtonClick);
    connect(ui.InsertBelowButton, &QPushButton::clicked, this, &Autosplit::routeEditButtonClick);
    connect(ui.InsertEndButton, &QPushButton::clicked, this, &Autosplit::routeEditButtonClick);

    connect(ui.InsertStartButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    connect(ui.InsertAboveButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    connect(ui.InsertBelowButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    connect(ui.InsertEndButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    
    connect(ui.SaveRouteButton, &QPushButton::clicked, this, &Autosplit::saveRouteButtonClick);
    connect(ui.NewRouteButton, &QPushButton::clicked, this, &Autosplit::newRouteButtonClick);
    connect(ui.LoadRouteButton, &QPushButton::clicked, this, &Autosplit::loadRouteButtonClick);

    connect(ui.SaveRouteButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.NewRouteButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(ui.LoadRouteButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);

    connect(ui.SaveRouteButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    connect(ui.NewRouteButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    connect(ui.LoadRouteButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    QListIterator<QObject*> i(ui.SourceFrame->children());

    while (i.hasNext())
    {
        QPushButton* button = qobject_cast<QPushButton*>(i.next());
        button->installEventFilter(this);
        sourceButtons.addButton(button);
        connect(button, &QPushButton::toggled, this, &Autosplit::routeButtonToggle);
        connect(button, &QPushButton::pressed, this, &Autosplit::routeButtonPress);
    }

    QListIterator<QObject*> j(ui.DestFrame->children());

    while (j.hasNext())  {
        QPushButton* button = qobject_cast<QPushButton*>(j.next());
        button->installEventFilter(this);
        destButtons.addButton(button);
        connect(button, &QPushButton::toggled, this, &Autosplit::routeButtonToggle);
        connect(button, &QPushButton::pressed, this, &Autosplit::routeButtonPress);
    }

    QList<QAbstractButton*> buttons = sourceButtons.buttons();

    for (QAbstractButton* button : buttons) {
        if (button->objectName() == QString("SourceArtaria")) {
            button->setChecked(true);
        }
    }

    buttons = destButtons.buttons();

    for (QAbstractButton* button : buttons) {
        if (button->objectName() == QString("DestCataris")) {
            button->setChecked(true);
        }
    }

    updateTransports();
    displayRoute();
    fps = 60;
    this->show();
    videoSource = videoCaptureDialog();
    capture = cv::VideoCapture(videoSource);
    videoPreviewTimer = new QTimer(this);
    connect(videoPreviewTimer, &QTimer::timeout, this, &Autosplit::setLivePreview);
    videoPreviewTimer->start(1000.0 / fps);

}


bool Autosplit::eventFilter(QObject* watched, QEvent* event)
{
    if (watched->parent() == ui.DestFrame    || 
        watched->parent() == ui.SourceFrame  ||
        watched->parent() == ui.verticalLayoutWidget) {

        QPushButton* button = static_cast<QPushButton*>(watched);

        if (event->type() == QEvent::Enter) {
            changeColor(button, HOVER_ROUTE_BTN_LIT_COLOR);
        }

        else if (event->type() == QEvent::Leave && !button->isChecked()) {
            changeColor(button, DEFAULT_BUTTON_COLOR);
        }
    }

    else if (watched->parent() == ui.RouteEditButtonFrame            || 
        watched->parent() == ui.RouteEditor                          || 
        watched->objectName() == "ContinueButton"                    || 
        watched->parent()->objectName() == "Autosplitter"            ||
        watched->parent() == ui.HotkeyContainer                      ||
        watched->parent()->objectName() == "AutosplitRunningDialog" || 
        watched->parent()->objectName() == "ErrorDialog"){
        QPushButton* button = static_cast<QPushButton*>(watched);
        if (button&&button->isEnabled()) {

            if (event->type() == QEvent::Enter) {
                changeColor(button, DARK_BUTTON_LIT_COLOR);
            }

            else if (event->type() == QEvent::Leave) {
                changeColor(button, DARK_BUTTON_COLOR);
            }
        }
    }

    else if (watched->parent() == ui.centralWidget) {
        QPushButton* button = static_cast<QPushButton*>(watched);
        if (button) {
            if (button == ui.AutosplitTab && ui.stackedWidget->currentWidget() == ui.RouteEditor ||
                button == ui.RouteEditorTab && ui.stackedWidget->currentWidget() == ui.Autosplitter) {
                if (event->type() == QEvent::Enter) {
                    changeColor(button, MAIN_TAB_LIT_COLOR);
                }

                else if (event->type() == QEvent::Leave) {
                    changeColor(button, MAIN_TAB_OFF_COLOR);
                }
            }
        }
    }


    else if (watched->parent()->parent() == ui.RouteScrollContents) {

        QPushButton* button = static_cast<QPushButton*>(watched);
        QFrame* frame = static_cast<QFrame*>(button->parent());

        if (event->type() == QEvent::Enter) {
            changeColor(frame, HOVER_ROUTE_BTN_LIT_COLOR);
        }

        else if (event->type() == QEvent::Leave && !button->isChecked()) {
            changeColor(frame, DEFAULT_BUTTON_COLOR);
        }
    }
       
    else if (watched->parent()->objectName() == "QDialogScrollContainer"){

        QPushButton* button = static_cast<QPushButton*>(watched);

        if (event->type() == QEvent::Enter) {
            changeColor(button, HOVER_ROUTE_BTN_LIT_COLOR);
        }

        else if (event->type() == QEvent::Leave && !button->isChecked()) {
            changeColor(button, DEFAULT_BUTTON_COLOR);
        }
    }

    else if (event->type() == QEvent::KeyPress) {
        QDialog* dialog = static_cast<QDialog*>(watched);
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        dialog->done(keyEvent->key());
    }
    return QMainWindow::eventFilter(watched, event);
}

void Autosplit::updateTransports()
{
    if (sourceButtons.checkedButton() != NULL && destButtons.checkedButton() != NULL) {
        QString source = sourceButtons.checkedButton()->text();
        QString dest = destButtons.checkedButton()->text();
        qDeleteAll(transportButtons.children());
        for (auto widget : ui.verticalLayoutWidget->findChildren<QWidget*>())
            delete widget;
        ui.TransportLayout->setAlignment(Qt::AlignTop);
        ui.RouteScrollContents->layout()->setAlignment(Qt::AlignTop);
        std::vector<QString> transports = RouteSegment::getPossibleTransports(source, dest);
        for (QString transport : transports) {
            QPushButton* button = new QPushButton(this);
            button->setFixedSize(QSize(119, 22));
            button->setText(transport);
            button->setStyleSheet(
                "border:0px;"
                "color: rgb(255, 255, 255);"
                "Text-align:left;"
                "padding: 0px 0px 0px 10px;"
                "background-color: rgb(100, 100, 100);"
                "font: 12pt \"Segoe UI Semilight\";"
            );
            button->setCheckable(true);
            transportButtons.addButton(button);
            button->setMouseTracking(true);
            button->installEventFilter(this);
            connect(button, &QPushButton::toggled, this, &Autosplit::routeButtonToggle);
            connect(button, &QPushButton::pressed, this, &Autosplit::routeButtonPress);
            ui.TransportLayout->addWidget(button);
            button->setMinimumSize(QSize(121, 31));
        }
    }
}

QPixmap Autosplit::getIconFromHTML(QString text) {
    QTextDocument Text;
    Text.setHtml(text);
    QPixmap pixmap(Text.size().width(), Text.size().height());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    Text.drawContents(&painter, pixmap.rect());
    return pixmap;
}

QString Autosplit::getSettingsString(){
    QString settings;
    settings.append(QString::fromUtf8("Automatically start:" + std::to_string(ui.StartTimerAutomatically->isChecked()) + "\n"));
    settings.append(QString::fromUtf8("Split hotkey:" + std::to_string(splitHotkey) + "\n"));
    settings.append(QString::fromUtf8("Pause hotkey:" + std::to_string(pauseHotkey) + "\n"));
    settings.append("Route:" + loadedRouteFileName);
    return settings;
}

void Autosplit::createErrorDialog(QString err)
{
    QDialog* dialog = new QDialog(this);
    dialog->setObjectName("ErrorDialog");
    dialog->setStyleSheet(
        "background-color: rgb(60,60,60);"
    );
    dialog->setWindowTitle("Error");
    dialog->setFixedSize(300, 213);
    QSize offset = QSize(this->size() / 2 - dialog->size() / 2);
    dialog->move((this->pos().x() + offset.width()), (this->pos().y() + offset.height()));
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->setAlignment(Qt::AlignTop);
    dialogLayout->setSpacing(9);
    dialog->setLayout(dialogLayout);

    QLabel* errorLabel = new QLabel(dialog);
    errorLabel->setStyleSheet(
        "border: 0px;"
        "background: transparent;"
        "font: 14pt \"Segoe UI Semilight\";"
        "color: rgb(255, 255, 255); "
        "padding: 0px 0px 0px 10px;"
    );
    errorLabel->setFixedSize(270, 143);
    errorLabel->setText(err);
    errorLabel->setWordWrap(true);
    errorLabel->setAlignment(Qt::AlignTop);
    dialogLayout->addWidget(errorLabel);

    QPushButton* continueButton = new QPushButton(dialog);
    continueButton->setFixedSize(121, 41);
    continueButton->setStyleSheet(
        "border:0px; "
        "color: rgb(255, 255, 255);"
        "padding: 0px 0px 0px 0px;"
        "background-color: rgb(80, 80, 80);"
    );
    continueButton->setText("Continue");
    continueButton->installEventFilter(this);
    continueButton->setObjectName("ContinueButton");

    dialogLayout->addWidget(continueButton);
    dialogLayout->setAlignment(continueButton, Qt::AlignHCenter);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(continueButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(continueButton, &QPushButton::clicked, dialog, &QDialog::done);
    connect(continueButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

    dialog->exec();
}

QFrame* Autosplit::getRouteSegmentFrame() {
    QFrame* frame = new QFrame(ui.RouteScrollContents);
    frame->setStyleSheet(
        "background-color: rgb(100,100,100);"
    );

    QGridLayout* layout = new QGridLayout(frame);
    layout->setHorizontalSpacing(0);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    frame->setLayout(layout);
    frame->setFixedSize(372, 41);

    QPushButton* routeSegmentButton = new QPushButton(frame);
    routeSegmentButton->setCheckable(true);
    routeSegmentButton->setFixedSize(331, 41);
    routeSegmentButton->setStyleSheet(
        "border: 0px;"
        "background: transparent;"
        "color: rgb(255, 255, 255);"
        "Text-align:left;"
        "padding: 0px 0px 0px 10px;"
        "font: 12pt \"Segoe UI Semilight\";"
    );

    QPushButton* xButton  = new QPushButton(frame);
    xButton->setFixedSize(41, 41);
    xButton->setText("X");
    xButton->setStyleSheet(
        "border: 0px;"
        "background:transparent;"
        "color: rgb(210,0,0);"
        "font: 14pt \"Segoe UI Black\";"
    );
    
    routeSegmentButton->setMouseTracking(true);
    routeButtons.addButton(routeSegmentButton);

    layout->addWidget(routeSegmentButton);
    layout->addWidget(xButton,0,1);

    connect(routeSegmentButton, &QPushButton::toggled, this, &Autosplit::routeSegmentButtonToggle);
    connect(routeSegmentButton, &QPushButton::pressed, this, &Autosplit::routeSegmentButtonPress);
    connect(xButton, &QPushButton::clicked, this, &Autosplit::routeEraseClick);
    return frame;
}

void Autosplit::addDisplayCaptureSelectButton(QWidget* parent, int source, QString displayName) {
    QPushButton* captureSelectButton = new QPushButton(parent);
    captureSelectButton->setCheckable(true);
    captureSelectButton->setFixedSize(245, 41);
    captureSelectButton->setStyleSheet(
        "border: 0px;"
        "background-color: rgb(100,100,100);"
        "color: rgb(255, 255, 255);"
        "Text-align:left;"
        "padding: 0px 0px 0px 10px;"
        "font: 12pt \"Segoe UI Semilight\";"
    );
    captureSelectButton->setText(displayName);
    captureSelectButton->setMouseTracking(true);
    captureSelectButton->installEventFilter(this);
    captureSelectButtons.addButton(captureSelectButton);
    connect(captureSelectButton, &QPushButton::toggled, this, &Autosplit::routeButtonToggle);
    captureSelectButtonMap.insert({ captureSelectButton, source });
    parent->layout()->addWidget(captureSelectButton);
}

int Autosplit::hotkeyDialog() {
    QDialog* dialog = new QDialog(this);
    dialog->setObjectName("test");
    dialog->setStyleSheet(
        "background-color: rgb(60,60,60);"
    );
    dialog->setWindowTitle("Hotkey");
    dialog->setFixedSize(161, 43);
    QSize offset = QSize(this->size() / 2 - dialog->size() / 2);
    dialog->move((this->pos().x() + offset.width()), (this->pos().y() + offset.height()));
    dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->setAlignment(Qt::AlignVCenter);
    dialog->setLayout(dialogLayout);
    dialog->installEventFilter(this);
    QLabel* selectLabel = new QLabel(dialog);
    selectLabel->setStyleSheet(
        "border: 0px;"
        "background: transparent;"
        "font: 12pt \"Segoe UI Semilight\";"
        "color: rgb(255, 255, 255); "
        "padding: 0px 0px 0px 0px;"
    );
    //selectLabel->setFixedSize(121, 33);
    selectLabel->setText("Press any key...");
    dialogLayout->addWidget(selectLabel);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

void Autosplit::startMainAutosplitterThreads() {
}


void Autosplit::routeButtonToggle(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button->isChecked()) {
        changeColor(button, HOVER_ROUTE_BTN_LIT_COLOR);
        if (button->parent() == ui.RouteScrollContents) {
            ui.InsertBelowButton->setEnabled(true);
            ui.InsertAboveButton->setEnabled(true);
            ui.InsertEndButton->setEnabled(true);
        }
    }
    else {
        changeColor(button, DEFAULT_BUTTON_COLOR);
    }

    if (button->parent() == ui.SourceFrame || button->parent() == ui.DestFrame) {
        updateTransports();
    }
}

void Autosplit::routeButtonPress() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button->isChecked())
        changeColor(button, HOVER_ROUTE_PRESS_COLOR);
}

void Autosplit::routeSegmentButtonToggle() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QFrame* frame = qobject_cast<QFrame*>(button->parent());
    if (button->isChecked()) {
        changeColor(frame, HOVER_ROUTE_BTN_LIT_COLOR);
    }
    else {
        changeColor(frame, DEFAULT_BUTTON_COLOR);
    }
}

void Autosplit::routeSegmentButtonPress() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QFrame* frame = qobject_cast<QFrame*>(button->parent());
    if (button->isChecked())
        changeColor(frame, HOVER_ROUTE_PRESS_COLOR);
}

void Autosplit::routeSegmentButtonRelease() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QFrame* frame = qobject_cast<QFrame*>(button->parent());
    if (button->isEnabled())
        changeColor(button, DEFAULT_BUTTON_COLOR);
}

void Autosplit::routeEditButtonRelease() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(button->isEnabled())
        changeColor(button, DARK_BUTTON_LIT_COLOR); 
}

void Autosplit::tabButtonRelease() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    changeColor(button, MAIN_TAB_LIT_COLOR);
}

void Autosplit::continueButtonClick(){
    QDialog* dialog = qobject_cast<QDialog*>(sender()->parent());

    QPushButton* checked = qobject_cast<QPushButton*>(captureSelectButtons.checkedButton());
    int source = captureSelectButtonMap.at(checked);
    dialog->done(source);
}

void Autosplit::splitSetHotkeyClick() {
    splitHotkey = hotkeyDialog();
    writeSettingsToFile(QString("./last-settings-used.txt"));
    ui.SplitHotkeyLabel->setText(QKeySequence(splitHotkey).toString());
}

void Autosplit::pauseSetHotkeyClick() {
    pauseHotkey = hotkeyDialog();
    writeSettingsToFile(QString("./last-settings-used.txt"));
    ui.PauseHotkeyLabel->setText(QKeySequence(pauseHotkey).toString());
}

void Autosplit::saveSettingsClick() {
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Settings File"), "./SettingsFiles/" + loadedSettingsFileName, tr("Text Files(*.txt)"));
    writeSettingsToFile(fileName);
}

void Autosplit::writeSettingsToFile(QString fileName)
{
    QFile file = QFile(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << getSettingsString();
    file.close();
}

void Autosplit::loadSettingsClick() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Settings File"), "./SettingsFiles", tr("Text Files(*.txt)"));
    loadedSettingsFileName = fileName;
    loadSettingsFile(fileName);
}

void Autosplit::loadSettingsFile(QString fileName)
{
    QFile file = QFile(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);

    QString autoStartStr = in.readLine().split(":")[1];
    QString splitHotkeyStr = in.readLine().split(":")[1];
    QString pauseHotkeyStr = in.readLine().split(":")[1];
    loadedRouteFileName = in.readLine().section(":", 1);
    qDebug() << loadedRouteFileName;

    ui.StartTimerAutomatically->setChecked(std::stoi(autoStartStr.toStdString()));
    splitHotkey = std::stoi(splitHotkeyStr.toStdString());
    pauseHotkey = std::stoi(pauseHotkeyStr.toStdString());

    if (splitHotkey == -1)
        ui.SplitHotkeyLabel->setText("Not set");
    else {
        ui.SplitHotkeyLabel->setText(QKeySequence(splitHotkey).toString());
    }
    if (pauseHotkey == -1) {
        ui.PauseHotkeyLabel->setText("Not set");
    }
    else {
        ui.PauseHotkeyLabel->setText(QKeySequence(pauseHotkey).toString());
    }
    file.close();
    readRouteFile(loadedRouteFileName);
    ui.MainRouteFileLabel->setText(loadedRouteFileName.split("/").last());
}

void Autosplit::startAutosplitterClick() {
    bool canStart{ true };
    if (splitHotkey == -1) {
        createErrorDialog("Split hotkey must be set before the program can run.");
        canStart = false;
    }
    if (pauseHotkey == -1) {
        createErrorDialog("Pause hotkey must be set before the program can run.");
        canStart = false;
    }
    if (loadedRouteFileName == "None") {
        createErrorDialog("A route must be loaded before the program can run.");
        canStart = false;
    }
    if (canStart) {
        QDialog* dialog = new QDialog(this);
        dialog->setObjectName("test");
        dialog->setStyleSheet(
            "background-color: rgb(60,60,60);"
        );
        dialog->setWindowTitle("Autosplitter");
        dialog->setFixedSize(261, 104);
        QSize offset = QSize(this->size() / 2 - dialog->size() / 2);
        dialog->move((this->pos().x() + offset.width()), (this->pos().y() + offset.height()));
        QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
        dialogLayout->setAlignment(Qt::AlignTop);
        dialogLayout->setSpacing(15);
        dialog->setLayout(dialogLayout);
        dialog->setObjectName("AutosplitRunningDialog");

        QLabel* selectLabel = new QLabel(dialog);
        selectLabel->setStyleSheet(
            "border: 0px;"
            "background: transparent;"
            "font: 14pt \"Segoe UI Semilight\";"
            "color: rgb(255, 255, 255); "
            "padding: 0px 0px 0px 0px;"
        );
        selectLabel->setText("Autosplitter is now running.");
        selectLabel->setAlignment(Qt::AlignHCenter);

        dialogLayout->addWidget(selectLabel);
        QPushButton* stopButton = new QPushButton(dialog);
        stopButton->setFixedSize(121, 41);
        stopButton->setStyleSheet(
            "border:0px; "
            "color: rgb(255, 255, 255);"
            "padding: 0px 0px 0px 0px;"
            "background-color: rgb(80, 80, 80);"
        );
        stopButton->setText("Stop");
        stopButton->installEventFilter(this);
        stopButton->setObjectName("Stop");

        dialogLayout->addWidget(stopButton);
        dialogLayout->setAlignment(stopButton, Qt::AlignHCenter);
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(stopButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);

        connect(stopButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);

        //mainAutoSplitterThreads
        std::vector<SplitImage> images = route.routeToSplitImages();
        cv::Mat frame;
        reader = new VideoReader(videoSource, frame);
        FrameProcesser* frameProcesser = new FrameProcesser(images, *reader, splitHotkey, pauseHotkey);

        QThread* videoReadThread = new QThread();
        QThread* frameProcessThread = new QThread();

        reader->moveToThread(videoReadThread);
        frameProcesser->moveToThread(frameProcessThread);

        connect(frameProcessThread, &QThread::started, frameProcesser, &FrameProcesser::process);
        connect(videoReadThread, &QThread::started, reader, &VideoReader::process);

        connect(reader, &VideoReader::error, this, &Autosplit::processError);

        connect(reader, &VideoReader::finished, dialog, &QDialog::close);
        connect(frameProcesser, &FrameProcesser::finished, this, &Autosplit::endAutosplitter);

        connect(reader, &VideoReader::finished, videoReadThread, &QThread::quit);
        connect(frameProcesser, &FrameProcesser::finished, frameProcessThread, &QThread::quit);

        connect(frameProcesser, &FrameProcesser::finished, frameProcesser, &FrameProcesser::deleteLater);
        connect(reader, &VideoReader::finished, reader, &VideoReader::deleteLater);

        connect(frameProcessThread, &QThread::finished, frameProcessThread, &QThread::deleteLater);
        connect(videoReadThread, &QThread::finished, videoReadThread, &QThread::deleteLater);

        connect(stopButton, &QPushButton::clicked, this, &Autosplit::endAutosplitter);
        connect(dialog, &QDialog::rejected, this, &Autosplit::endAutosplitter);

        videoReadThread->start();
        frameProcessThread->start();

        videoPreviewTimer->stop();

        dialog->exec();
    }
}

void Autosplit::processError(QString err) {
    createErrorDialog(err);
}

void Autosplit::endAutosplitter() {
    reader->stopReading();
}

void Autosplit::routeEraseClick() {
    QFrame* frame = qobject_cast<QFrame*>(sender()->parent());
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    for(auto button : frame->parent()->findChildren<QPushButton*>()) {
        button->removeEventFilter(this);
    }

    int index = route.getIndexFromRouteSegmentFrame(frame);
    route.removeSegment(index);

    //extra case in case the user deletes the checked button 
    if (routeButtons.checkedButton() == NULL && routeButtons.buttons().size()>0) {
        routeButtons.buttons()[0]->setChecked(true);
    }
    displayRoute();
}

void Autosplit::newRouteButtonClick() {
    loadedRouteFileName = QString("");
    ui.MainRouteFileLabel->setText("None");
    writeSettingsToFile(QString("./last-settings-used.txt"));
    disableEditRouteButtons();

    for (RouteSegment segment : route.getSegments()) {
        QPushButton* button = segment.getRouteSegmentFrame()->findChild<QPushButton*>();
        button->removeEventFilter(this);
        delete segment.getRouteSegmentFrame();
    }

    route.clear();
    displayRoute();
}

void Autosplit::loadRouteButtonClick() {
    for(RouteSegment segment : route.getSegments()){
        QPushButton* button = segment.getRouteSegmentFrame()->findChild<QPushButton*>();
        button->removeEventFilter(this);
        delete segment.getRouteSegmentFrame();
    }

    route.clear();

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Route File"), "./Routes", tr("Text Files(*.txt)"));
    loadedRouteFileName = fileName;
    writeSettingsToFile(QString("./last-settings-used.txt"));
    readRouteFile(fileName);
    ui.MainRouteFileLabel->setText(fileName.split("/").last());
    displayRoute();
}

void Autosplit::readRouteFile(QString& fileName)
{
    QFile file = QFile(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList segments = line.split(" ");
        QFrame* frame = getRouteSegmentFrame();
        QPushButton* button = frame->findChild<QPushButton*>();
        QString transport = line.section(' ', 2, 3);
        RouteSegment segment = RouteSegment(segments[0], segments[1], transport, frame);
        route.addSegment(segment);
    }
    //set first button to checked to avoid errors, if it's not empty
    if(!route.getSegments().empty())
        route.getSegments()[0].getRouteSegmentFrame()->findChild<QPushButton*>()->setChecked(true);

    file.close();
}

void Autosplit::displayRoute(){
    for (auto widget : ui.RouteScrollContents->layout()->findChildren<QWidget*>()) {
        ui.RouteScrollContents->layout()->removeWidget(widget);
    }

    for (RouteSegment segment : route.getSegments()) {
        QFrame* frame = segment.getRouteSegmentFrame();
        ui.RouteScrollContents->layout()->addWidget(frame);
        this->show();
        ui.RouteScrollArea->ensureWidgetVisible(frame);
        QPushButton* button = frame->findChild<QPushButton*>();
        button->installEventFilter(this);
        QPixmap pixmap = getIconFromHTML("<span style='color:#ffffff;'>" + segment.toRichText() + "</span>");
        QIcon ButtonIcon(pixmap);
        button->setIcon(ButtonIcon);
        button->setIconSize(pixmap.rect().size());
    }

    if (routeButtons.buttons().size() == 1) {
       routeButtons.buttons()[0]->setChecked(true);
    }

    else if (routeButtons.buttons().size() > 1) {
        ui.RouteScrollArea->ensureWidgetVisible(routeButtons.checkedButton());
        enableEditRouteButtons();
    }

    else if (routeButtons.buttons().size() <= 1) {
        disableEditRouteButtons();
    }
}
void Autosplit::saveRouteButtonClick() {
    qDebug() << loadedRouteFileName;
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Route File"), "./Routes/"+loadedRouteFileName.split("/").last(), tr("Text Files(*.txt)"));
    QFile file = QFile(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << route.toString().toLower();
    file.close();
}

void Autosplit::routeEditButtonClick() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QAbstractButton* sourceBtn = sourceButtons.checkedButton();
    QAbstractButton* destBtn = destButtons.checkedButton();
    QAbstractButton* transportBtn = transportButtons.checkedButton();
    QAbstractButton* routeBtn = routeButtons.checkedButton();

    if (sourceBtn != NULL && destBtn != NULL && transportBtn != NULL) {
        QString source = sourceBtn->text();
        QString dest = destBtn->text();
        QString transport = transportBtn->text();

        QFrame* routeSegmentFrame = getRouteSegmentFrame();
        RouteSegment segment = RouteSegment(source, dest, transport, routeSegmentFrame);
        QPushButton* newSegmentButton = segment.getRouteSegmentFrame()->findChild<QPushButton*>();
        newSegmentButton->setChecked(true);

        if (button == ui.InsertStartButton) {
            route.insertSegment(segment, 0);
            enableEditRouteButtons();
        }

        else if (button == ui.InsertBelowButton) {
            int indexOfButton = route.getIndexFromRouteSegmentFrame(qobject_cast<QFrame*>(routeBtn->parent()));
            route.insertSegment(segment, indexOfButton+1);
        }

        else if (button == ui.InsertAboveButton) {
            int indexOfButton = route.getIndexFromRouteSegmentFrame(qobject_cast<QFrame*>(routeBtn->parent()));
            route.insertSegment(segment, indexOfButton);
        }

        else if (button == ui.InsertEndButton) {
            route.insertSegment(segment, route.getSegments().size());
        }

        displayRoute();
    }
}

void Autosplit::routeEditButtonPress() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    changeColor(button, DARK_BUTTON_PRESS_COLOR);
}

void Autosplit::tabButtonPress() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    changeColor(button, MAIN_TAB_PRESS_COLOR);
    changeTextColor(button, DEFAULT_TEXT_COLOR);
    if (button == ui.AutosplitTab) {
        ui.stackedWidget->setCurrentWidget(ui.Autosplitter);
        changeColor(ui.RouteEditorTab, MAIN_TAB_OFF_COLOR);
        changeTextColor(ui.RouteEditorTab, OFF_TEXT_COLOR);
    }
    else {
        ui.stackedWidget->setCurrentWidget(ui.RouteEditor);
        changeColor(ui.AutosplitTab, MAIN_TAB_OFF_COLOR);
        changeTextColor(ui.AutosplitTab, OFF_TEXT_COLOR);
    }
}

void Autosplit::changeColor(QWidget* mbutton, const QString color) {
    mbutton->ensurePolished(); 
    QString sheet = mbutton->styleSheet();
    QRegularExpression re = QRegularExpression("background-color.*?;");
    sheet.replace(re,color);
    mbutton->setStyleSheet(sheet);
    
}

void Autosplit::changeTextColor(QPushButton* mbutton, const QString color) {
    QString sheet = mbutton->styleSheet();
    QRegularExpression re = QRegularExpression("[^-](color.*?;)");
    QRegularExpressionMatch match = re.match(sheet);
    sheet.replace(match.capturedStart(1), match.capturedLength(1), color);
    mbutton->setStyleSheet(sheet);
}

void Autosplit::disableEditRouteButtons() {
    changeTextColor(ui.InsertAboveButton, OFF_TEXT_COLOR);
    changeTextColor(ui.InsertBelowButton, OFF_TEXT_COLOR);
    changeTextColor(ui.InsertEndButton, OFF_TEXT_COLOR);

    changeColor(ui.InsertAboveButton, OFF_BUTTON_COLOR);
    changeColor(ui.InsertBelowButton, OFF_BUTTON_COLOR);
    changeColor(ui.InsertEndButton, OFF_BUTTON_COLOR);

    ui.InsertBelowButton->setEnabled(false);
    ui.InsertAboveButton->setEnabled(false);
    ui.InsertEndButton->setEnabled(false);
}

QString Autosplit::routeToTextReadyString()
{
    QString routeText;
    return routeText;
}


int Autosplit::videoCaptureDialog() {

    QDialog* dialog = new QDialog(this);
    dialog->setObjectName("test");
    dialog->setStyleSheet(
        "background-color: rgb(60,60,60);"
    );
    dialog->setWindowTitle("Capture Selection");
    dialog->setFixedSize(300, 400);
    QSize offset = QSize(this->size() / 2 - dialog->size() / 2);
    dialog->move((this->pos().x() + offset.width()), (this->pos().y() + offset.height()));
    dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->setAlignment(Qt::AlignTop);
    dialogLayout->setSpacing(9);
    dialog->setLayout(dialogLayout);

    QLabel* selectLabel = new QLabel(dialog);
    selectLabel->setStyleSheet(
        "border: 0px;"
        "background: transparent;"
        "font: 14pt \"Segoe UI Semilight\";"
        "color: rgb(255, 255, 255); "
        "padding: 0px 0px 0px 0px;"
    );
    selectLabel->setFixedSize(280, 33);
    selectLabel->setText("Select your capture device:");
    selectLabel->setAlignment(Qt::AlignHCenter);

    dialogLayout->addWidget(selectLabel);
    QScrollArea* videoScroller = new QScrollArea(dialog);
    dialogLayout->addWidget(videoScroller);
    videoScroller->setStyleSheet(
        "background-color: rgb(100,100,100);"
        "border: 0px;"
    );
    QWidget* videoScrollContainer = new QWidget(videoScroller);

    videoScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    videoScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QVBoxLayout* videoLayout = new QVBoxLayout(videoScrollContainer);
    videoLayout->setAlignment(Qt::AlignTop);
    videoLayout->setSpacing(9);
    videoScrollContainer->setLayout(videoLayout);
    videoScrollContainer->setObjectName("QDialogScrollContainer");

    std::map<int, msmf::Device> devicesTotal = msmf::DeviceEnumerator().getVideoDevicesMap();
    for (auto device : devicesTotal) {
        int id = device.first;
        std::string deviceName = device.second.deviceName;
        std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), ::tolower);
        if (deviceName.find("webcam") == std::string::npos) {
            addDisplayCaptureSelectButton(videoScrollContainer, id, QString::fromUtf8(device.second.deviceName));
        }
    }

    QPushButton* firstButton = videoScrollContainer->findChild<QPushButton*>();
    firstButton->setChecked(true);

    QPushButton* continueButton = new QPushButton(dialog);
    continueButton->setFixedSize(121, 41);
    continueButton->setStyleSheet(
        "border:0px; "
        "color: rgb(255, 255, 255);"
        "padding: 0px 0px 0px 0px;"
        "background-color: rgb(80, 80, 80);"
    );
    continueButton->setText("Continue");
    continueButton->installEventFilter(this);
    continueButton->setObjectName("ContinueButton");

    dialogLayout->addWidget(continueButton);
    dialogLayout->setAlignment(continueButton,Qt::AlignHCenter);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(continueButton, &QPushButton::pressed, this, &Autosplit::routeEditButtonPress);
    connect(continueButton, &QPushButton::clicked, this, &Autosplit::continueButtonClick);
    connect(continueButton, &QPushButton::released, this, &Autosplit::routeEditButtonRelease);
    connect(dialog, &QDialog::finished, this, &Autosplit::previewDialogClose);

    return dialog->exec();
}

void Autosplit::previewDialogClose() {
    qDebug() << "got here";
}

void Autosplit::doubleClick(QMouseEvent* event)
{
   
}

void Autosplit::enableEditRouteButtons() {
    ui.InsertBelowButton->setEnabled(true);
    ui.InsertAboveButton->setEnabled(true);
    ui.InsertEndButton->setEnabled(true);

    changeTextColor(ui.InsertAboveButton, DEFAULT_TEXT_COLOR);
    changeTextColor(ui.InsertBelowButton, DEFAULT_TEXT_COLOR);
    changeTextColor(ui.InsertEndButton, DEFAULT_TEXT_COLOR);

    changeColor(ui.InsertAboveButton, DARK_BUTTON_COLOR);
    changeColor(ui.InsertBelowButton, DARK_BUTTON_COLOR);
    changeColor(ui.InsertEndButton, DARK_BUTTON_COLOR);
}

void Autosplit::videoCaptureSelectClick() {
    int newVideoSource = videoCaptureDialog();
    if (newVideoSource != videoSource) {
        capture = cv::VideoCapture(newVideoSource);
        videoSource = newVideoSource;
        fps = 60;
        videoPreviewTimer->start(1000.0 / fps);
    }
}

void Autosplit::setLivePreview() {
    QGraphicsView* videoImage = ui.ImagePreview;
    delete videoImage->scene();
    cv::Mat image;
    if (capture.read(image)) {
        cvtColor(image, image, cv::COLOR_BGR2RGB);
        cv::resize(image, image, cv::Size(508, 286));
        QPixmap pixmap = QPixmap::fromImage(QImage(image.data, image.cols, image.rows, QImage::Format_RGB888));
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);
        QGraphicsScene* scene = new QGraphicsScene(this);
        scene->addItem(item);
        videoImage->setScene(scene);
    }
}

bool Autosplit::checkPreviousSettingsUsed() {
    QString fileName = "./last-settings-used.txt";
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists()) {
        loadSettingsFile(fileName);
        return true;
    }
    return false;
}