#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);
    MainWindow w;
#ifdef _WIN32
    RegisterHotKey((HWND)w.winId(), 100, 0, 0x76);//F7
    RegisterHotKey((HWND)w.winId(), 101, 0, 0x75);//F6
#endif
    w.show();

    return a.exec();
}
