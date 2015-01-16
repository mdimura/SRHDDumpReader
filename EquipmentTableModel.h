#ifndef EQUIPMENTTABLEMODEL_H
#define EQUIPMENTTABLEMODEL_H

#include <QAbstractTableModel>

class Galaxy;

class EquipmentTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EquipmentTableModel(const Galaxy* galaxy, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void reload()
    {
        beginResetModel();
        endResetModel();
    }
    void initialiseFilterWidgets();

signals:

public slots:

private:
    const Galaxy *_galaxy;
};

#endif // EQUIPMENTTABLEMODEL_H
