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
#include <QJsonArray>
#include <QJsonObject>

#include "SortMultiFilterProxyModel.h"

class FilterHorizontalHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    enum WidgetType {wtString, wtInt, wtDouble, wtNone};
    explicit FilterHorizontalHeaderView(SortMultiFilterProxyModel* model, QTableView *parent = 0);
    QSize sizeHint() const;
    /*QJsonObject filters() const
    {
        QJsonObject obj;
        QMap<QString,QVariant> matchFilters;
        using MapIntLineEditCI=QMap<int, QLineEdit*>::const_iterator;
        for (MapIntLineEditCI i = matchEdits.begin(); i != matchEdits.end(); ++i)
        {
            matchFilters.insert(QString::number(i.key()),i.value()->text());
        }
        obj.insert("matchFilters",json(matchFilters));
        return obj;
    }*/

protected:
    bool event( QEvent *event );
    virtual QSize sectionSizeFromContents(int logicalIndex) const;
private slots:
    void setSortIndicator(int col, const Qt::SortOrder &order);
    void updateWidgetPositions() const;
    void updateHeaderData(int first, int last);

private:
    void updateGeometry(int logical) const;
    void updateHeaderData(int col);
    void makeWidget(int col);
    void insertColumns(int first, int last);
    /*QMap<QString,QVariant> map(const QJsonArray &array) const
    {
        QMap<QString,QVariant> map;
        for(const auto& entry:array)
        {
            auto obj=entry.toObject();
            const QJsonValue &val=obj["value"];
            QVariant var;
            switch (val.type())
            {
            case QJsonValue::Bool:
                var=val.toBool();
                break;
            case QJsonValue::Double:
                var=val.toDouble();
                break;
            case QJsonValue::String:
                var=val.toString();
                break;
            default:
                std::cerr<<"could not convert QJsonValue to QVariant"<<std::endl;
                break;
            }

            map[obj["key"].toString()]=var;
        }
        return map;
    }
    QJsonArray json(const QMap<QString,QVariant>& map) const
    {
        QJsonArray arr;
        QMap<QString, QVariant>::const_iterator i;
        for (i = map.begin(); i != map.end(); ++i)
        {
            QJsonObject obj;
            obj["key"]=i.key();
            QJsonValue val;
            const QVariant& var=i.value();
            switch (var.type())
            {
            case QVariant::Bool:
                val=var.toBool();
                break;
            case QVariant::Int:
                val=var.toInt();
                break;
            case QVariant::Double:
                val=var.toDouble();
                break;
            case QVariant::String:
                val=var.toString();
                break;
            default:
                std::cerr<<"could not convert to QVariant from JSON"<<std::endl;
                break;
            }

            obj["value"]=val;
            arr.append(obj);
        }
        return arr;
    }*/

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
    int lastSortSection=0;
    int _height=10;
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
