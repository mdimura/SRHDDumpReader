#ifndef BLACKHOLESTABLEMODEL_H
#define BLACKHOLESTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QStandardItemModel>

#include "Galaxy.h"

class BlackHolesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BlackHolesTableModel(const Galaxy* _galaxy, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void reload()
    {
        beginResetModel();
        endResetModel();
    }

signals:

public slots:
private:
    const Galaxy *_galaxy;
};

#endif // BLACKHOLESTABLEMODEL_H
