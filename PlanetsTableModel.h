#ifndef PLANETSTABLEMODEL_H
#define PLANETSTABLEMODEL_H

#include <QAbstractTableModel>

#include "Galaxy.h"

class PlanetsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PlanetsTableModel(const Galaxy* galaxy, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void reload()
    {
        beginResetModel();
        endResetModel();
    }
private:
    const Galaxy *_galaxy;
};

#endif // PLANETSTABLEMODEL_H
