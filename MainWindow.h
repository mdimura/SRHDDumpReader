#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifdef _WIN32
#include <Windows.h>
#endif

#include <unordered_map>
#include <QMainWindow>
#include <QDateTime>
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QApplication>
#include <QSoundEffect>
#include <QStandardPaths>

#include "Equipment.h"
#include "Ship.h"
#include "Star.h"
#include "Planet.h"
#include "BlackHole.h"
#include "Galaxy.h"
#include "TradeTableModel.h"
#include "EquipmentTableModel.h"
#include "HierarchicalHeaderView.h"
#include "MultiFilterProxyModel.h"
#include "WidgetHeaderView.h"
#include "BlackHolesTableModel.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readSettings();
    void writeSettings() const;
    void closeEvent(QCloseEvent *event);
public slots:
    bool parseDump();
    bool openDump();
    void showAbout();
    int saveReport() const;

#ifdef _WIN32
public slots:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long*);
    void saveDumpWin();
public:
    bool simulateInput(const std::string& str) const;
    QImage currentScreen(float kx, float ky, float kw, float kh) const;
#endif
private:
    void generateGalaxies();
    void responsiveSleep(int msec) const
    {
        int t;
        for(t=0; t<msec-shortSleep; t+=shortSleep)
        {
            Sleep(shortSleep);
            QCoreApplication::processEvents();
        }
        Sleep(msec-t);
    }

private:
    Ui::MainWindow *ui;
    QString _filename;
    Galaxy galaxy;
    QDateTime _fileModified;

    TradeTableModel tradeModel;
    QSortFilterProxyModel tradeProxyModel;

    EquipmentTableModel eqModel;
    MultiFilterProxyModel eqProxyModel;

    BlackHolesTableModel bhModel;

    QTimer reloadTimer;

    QSoundEffect sound;

    const QString rangersDir=QStandardPaths::locate(QStandardPaths::DocumentsLocation,"SpaceRangersHD",QStandardPaths::LocateDirectory);
    int maxGenerationTime=80000;
    int shortSleep=25;
};

#endif // MAINWINDOW_H
