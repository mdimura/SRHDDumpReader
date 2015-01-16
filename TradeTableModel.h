#ifndef TRADETABLEMODEL_H
#define TRADETABLEMODEL_H

#include <QAbstractTableModel>
#include <QStandardItemModel>
#include "Galaxy.h"

class TradeTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TradeTableModel(const Galaxy* galaxy, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void reload()
    {
        beginResetModel();
        endResetModel();
    }
    void fillHeaderModel();

signals:

public slots:
private:
    const Galaxy *_galaxy;
    QStandardItemModel _horizontalHeaderModel;
};

#endif // TRADETABLEMODEL_H
