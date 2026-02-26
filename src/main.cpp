#include "ui/MainWindow.h"
#include "logger/Log.h"
#include <QApplication>
#include <QDebug>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    Log::init(true);
#else
    Log::init(false);
#endif
    QApplication a(argc, argv);
    // a.setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    LOG_DEBUG(logApp) << "MainWindow constructed";

    return a.exec();
}
