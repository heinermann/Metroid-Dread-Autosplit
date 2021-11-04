#include "Autosplit.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include"RouteSegment.h"
#include<vector>
#include<string>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QApplication::setFont(QFont("Segoe UI Semilight"));
    //set style
    QFile styleSheetFile("SpyBot.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    a.setStyleSheet(styleSheet);

    Autosplit w;
    w.show();
    return a.exec();

}
