#include "MainWindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);
    QTranslator translator;
    translator.load("SRHDDumpReader_ru");
    a.installTranslator(&translator);
    MainWindow w;
#ifdef _WIN32
    RegisterHotKey((HWND)w.winId(), 100, 0, 0x76);//F7
    RegisterHotKey((HWND)w.winId(), 101, 0, 0x75);//F6
    QDir::setCurrent(QCoreApplication::applicationDirPath());
#endif
    w.show();

    return a.exec();
}
