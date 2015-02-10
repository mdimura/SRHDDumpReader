#ifndef FILTERHORIZONTALHEADERVIEW_H
#define FILTERHORIZONTALHEADERVIEW_H

#include <cassert>
#include <iostream>

#include <QHeaderView>
#include <QTableView>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QLineEdit>
#include <QSpinBox>
#include <QAction>
#include <QMenu>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QMessageBox>
#include "SortMultiFilterProxyModel.h"

class FilterHorizontalHeaderView : public QHeaderView
{
	Q_OBJECT
public:
	enum WidgetType {wtString, wtInt, wtDouble, wtNone};
	explicit FilterHorizontalHeaderView(SortMultiFilterProxyModel* model, QTableView *parent = 0);
	QSize sizeHint() const;
	void addPreset(const QVariantMap& p, const QString& name)
	{
		_presets.push_back(p);
		QAction *act=new QAction(name,this);
		int i=_presets.count()-1;
		connect(act,&QAction::triggered,[=](){
			activatePreset(i);
		});
		contextMenu.addAction(act);
	}
	void setPreset(const QVariantMap& p);
	QVariantMap preset() const;
signals:
	void presetSaved(QVariantMap, QString);
protected:
	virtual bool event( QEvent *event );
	virtual QSize sectionSizeFromContents(int logicalIndex) const;
	virtual void contextMenuEvent(QContextMenuEvent *event)
	{
		contextMenu.exec(event->globalPos());
	}
private slots:
	void setSortIndicator(int col, const Qt::SortOrder &order);
	void updateWidgetPositions() const;
	void updateHeaderData(int first, int last);
	void savePreset();
	void activatePreset(int i);
	void clearAllFilters();
	void applyFilters();

private:
	void updateGeometry(int logical) const;
	void updateHeaderData(int col);
	void makeWidget(int col);
	void insertColumns(int first, int last);

protected:
	void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
	QTimer timer;

	int lastSortSection=0;
	int _height=10;
	QAction saveAct{tr("save preset"),this};
	QAction clearAct{tr("clear filters"),this};
	QMenu contextMenu{this};
	QVector<QVariantMap> _presets;
	SortMultiFilterProxyModel* _model=nullptr;

	QMap<int,QWidget*> headerWidgets;
	QMap<int,QLabel*> headerNames;
	QMap<int,QLabel*> headerSortIndicators;

	QMap<int,QLineEdit*> matchEdits;
	QMap<int,QLineEdit*> notMatchEdits;
	QMap<int,QSpinBox*> minIntEdits;
	QMap<int,QSpinBox*> maxIntEdits;
	QMap<int,QDoubleSpinBox*> minDoubleEdits;
	QMap<int,QDoubleSpinBox*> maxDoubleEdits;

	bool frame=true;
	int inputHeight=QFontMetrics(QFont("sans",11)).height();
};

#endif // FILTERHORIZONTALHEADERVIEW_H
