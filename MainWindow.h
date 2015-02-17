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
#include <QMutex>
#include <QWaitCondition>

#include "Equipment.h"
#include "Ship.h"
#include "Star.h"
#include "Planet.h"
#include "BlackHole.h"
#include "Galaxy.h"
#include "TradeTableModel.h"
#include "EquipmentTableModel.h"
#include "HierarchicalHeaderView.h"
#include "BlackHolesTableModel.h"
#include "PlanetsTableModel.h"
#include "SortMultiFilterProxyModel.h"
#include "FilterHorizontalHeaderView.h"


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
    void saveReport();
    void setMapScale(double scale)
    {
	mapScale=scale;
	updateMap();
    }
    void loadNextDump();
    void loadPreviousDump();

#ifdef _WIN32
public slots:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long*);
    void saveDumpWin();
public:
    bool simulateInput(const std::string& str) const;
    QImage currentScreen(float kx, float ky, float kw, float kh);
#endif

private:
    void savePreset(const QVariantMap& preset, const QString& fileName) const;
    void generateGalaxies();
    void responsiveSleep(int msec) const;
    void loadPresets();
    void updateMap();
    void updateDumpArrows();
    void saveMap();
    bool eventFilter(QObject* object, QEvent* event);

    QVariantMap loadPreset(const QString &fileName) const;

private:
    Ui::MainWindow *ui;
    QString _filename;
    Galaxy galaxy;
    QDateTime _fileModified;

    TradeTableModel tradeModel;
    QSortFilterProxyModel tradeProxyModel;

    EquipmentTableModel eqModel;
    FilterHorizontalHeaderView* eqHeaderView;
    SortMultiFilterProxyModel eqProxyModel;

    BlackHolesTableModel bhModel;

    PlanetsTableModel planetsModel;
    SortMultiFilterProxyModel planetsProxyModel;
    FilterHorizontalHeaderView *planetsHeaderView;

    QTimer reloadTimer;
    QDoubleSpinBox _mapScaleSpinBox{this};

    QMenu reloadMenu;
    QMenu saveReportMenu;

    QSoundEffect sound;

    const QString rangersDir=QStandardPaths::locate(QStandardPaths::DocumentsLocation,"SpaceRangersHD",QStandardPaths::LocateDirectory);
    const QString presetDirPlanets="presets/planets/";
    const QString presetDirEq="presets/equipment/";
    const QString presetDirPlanetsReport="presets/planetsReport/";
    const QString presetDirEqReport="presets/equipmentReport/";
    int maxGenerationTime=120000;
    int screenSaveLag=200;
    int shortSleep=25;
    int minHIGplanets=0;//Huge industrial gaal planets limit
    float mapScale=7.f;
    QStringList planetsReportPresets;
    QStringList eqReportPresets;
    QMap<QString,int> minRowsPreset;
    QMap<QString,int> reportSummary;
    QStringList dumpFileList;
    int currentDumpIndex=-1;
    QImage galaxyMap;
};

#endif // MAINWINDOW_H
