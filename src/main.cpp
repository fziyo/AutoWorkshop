#include "ui/MainWindow.h"
#include "log/Log.h"
#include <QApplication>
#include <QDebug>
#include <QStyleFactory>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    // a.setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    LOG_DEBUG(logApp) << "MainWindow constructed";

    return a.exec();
}
