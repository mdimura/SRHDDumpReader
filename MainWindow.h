#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifdef _WIN32
#include <windows.h>
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
#include <QStatusBar>
#include <QComboBox>
#include <QColor>
#include <QItemEditorFactory>

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
QStringList supportedColors();

class ColorListEditor : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor USER true)

public:
	ColorListEditor(QWidget *widget = 0): QComboBox(widget)
	{
		populateList(supportedColors());
	}

public:
	QColor color() const
	{
		return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
	}
	void setColor(QColor c)
	{
		setCurrentIndex(findData(c, int(Qt::DecorationRole)));
	}

private:
	void populateList(const QStringList& colorNames)
	{
		for (int i = 0; i < colorNames.size(); ++i) {
			QColor color(colorNames[i]);
			insertItem(i, colorNames[i]);
			setItemData(i, color, Qt::DecorationRole);
		}
	}
};

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
	bool parseDump(const QString &filename=QString());
	bool openDump();
	void showAbout();
	void saveReport();
	void saveAllReports();
    void setMapScale(int width)
	{
	mapWidth=width;
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

private slots:
	void customHeaderMenuRequested(QPoint pos);

private:
	//    using Scorer=std::vector<std::tuple<QString,double,bool>>;
	struct Scorer
	{
		void read(const QVariantMap& map);
		void addPreset(const QString presetName,double weight,bool isBool,bool depthPenalize);
		double score(const QMap<QString,int>& reportSummary,const QMap<QString,QVector<int>>& _reportDepthList) const
		{
			double score=0.0;
			const double depthOffset=100;
			for(int iPreset=0; iPreset<presetNames.size(); iPreset++)
			{
				double numRows=reportSummary.value(presetNames[iPreset]);
				if(numRows==0.0) {continue;}
				numRows=areBoolean[iPreset]?std::min(numRows,1.0):numRows;
				if(depthPenalized[iPreset]) {
					auto depthList=_reportDepthList.value(presetNames[iPreset]);
					if(areBoolean[iPreset]) {
						int minDepth=*std::min_element(depthList.constBegin(), depthList.constEnd());
						minDepth+=depthOffset;
						score+=1.0*weights[iPreset]/minDepth;
					} else {
						for(int depth: depthList) {
							depth+=depthOffset;
							score+=1.0*weights[iPreset]/depth;
						}
					}
				} else {
					score+=numRows*weights[iPreset];
				}
			}
			return score;
		}

		QVector<QString> presetNames;
		QVector<double> weights;
		QVector<bool> areBoolean;
		QVector<bool> depthPenalized;
	};
	void showMessage(const QString& str,int timeout=0) const
	{
		std::cout<<str.toStdString()<<std::endl;
		statusBar()->showMessage(str,timeout);
	}
	bool openDump(const QString& fileName);
	void savePreset(const QVariantMap& preset, const QString& fileName) const;
	void generateGalaxies();
	void responsiveSleep(int msec) const;
	void loadPresets();
	void updateMap();
	void updateDumpArrows();
	void saveMap();
	bool eventFilter(QObject* object, QEvent* event);
	QVariantMap loadPreset(const QString &fileName) const;
	QString scoresSummary(bool desc=true) const
	{
		QString summary;
		using MapStrScorerCI=QMap<QString,Scorer>::const_iterator;
		for (MapStrScorerCI i = scorers.begin(); i != scorers.end(); ++i)
		{
			if(desc) {summary+=i.key()+" = ";}
			summary+=QString::number(i.value().score(_reportSummary,_reportDepthList))+
				 "\t";
		}
		return summary;
	}
	QString scoresSummaryHeader() const
	{
		QString summary;
		using MapStrScorerCI=QMap<QString,Scorer>::const_iterator;
		for (MapStrScorerCI i = scorers.begin(); i != scorers.end(); ++i)
		{
			summary+=i.key()+"\t";
		}
		return summary;
	}
	QString reportSummary(bool desc=true) const
	{
		QString summary=scoresSummary(desc);
		using MapStrIntCI=QMap<QString,int>::const_iterator;
		for (MapStrIntCI i = _reportSummary.begin(); i != _reportSummary.end(); ++i)
		{
			if (desc) { summary+=i.key()+": ";}
			summary+=QString::number(i.value())+"\t";
		}
		return summary;
	}
	QString reportSummaryHeader() const
	{
		QString summary="dump name\t"+scoresSummaryHeader();
		using MapStrIntCI=QMap<QString,int>::const_iterator;
		for (MapStrIntCI i = _reportSummary.begin(); i != _reportSummary.end(); ++i)
		{
			summary+=i.key()+"\t";
		}
		return summary;
	}
	static QMap<QString,Scorer> readScorers(const QString &filename);

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
	QMenu eqMenu;
	unsigned eqMenuRow=0;

	BlackHolesTableModel bhModel;

	PlanetsTableModel planetsModel;
	SortMultiFilterProxyModel planetsProxyModel;
	FilterHorizontalHeaderView *planetsHeaderView;

	QTimer reloadTimer;
    QSpinBox _mapScaleSpinBox{this};

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
    unsigned mapWidth=800;
	QStringList planetsReportPresets;
	QStringList eqReportPresets;
	QMap<QString,int> minRowsPreset;
	QMap<QString,int> _reportSummary;
	QMap<QString,QVector<int>> _reportDepthList;
	QStringList dumpFileList;
	int currentDumpIndex=-1;
	QImage galaxyMap;

	QMap<QString,Scorer> scorers;
};

#endif // MAINWINDOW_H
