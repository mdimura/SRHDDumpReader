#ifndef MULTIFILTERPROXYMODEL_H
#define MULTIFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QTimer>
class MultiFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MultiFilterProxyModel(QObject *parent = 0);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setSourceModel(QAbstractItemModel * sourceModel)
    {
        QSortFilterProxyModel::setSourceModel(sourceModel);
        initialiseFilterWidgets();
    }
protected:
    bool filterAcceptsRow(int sourceRow,
            const QModelIndex &sourceParent) const;
signals:

public slots:
private:
    void initialiseFilterWidgets();
private:
    QVector<QWidget*> filterWidgets;
    QMap<unsigned,QLineEdit*> regExpEdits;
    QMap<unsigned,QDoubleSpinBox*> minDoubleSpinBoxes;
    QMap<unsigned,QDoubleSpinBox*> maxDoubleSpinBoxes;
    QMap<unsigned,QSpinBox*> minSpinBoxes;
    QMap<unsigned,QSpinBox*> maxSpinBoxes;
    QTimer timer;

};

#endif // MULTIFILTERPROXYMODEL_H
