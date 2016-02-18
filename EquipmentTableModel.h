#ifndef EQUIPMENTTABLEMODEL_H
#define EQUIPMENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QColor>

class Galaxy;

bool operator<(const QColor & a, const QColor & b);

class EquipmentTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EquipmentTableModel(const Galaxy* galaxy, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
		     int role = Qt::EditRole);
    void reload()
    {
	beginResetModel();
	endResetModel();
    }
    void initialiseFilterWidgets();
    QString colorName(const QColor& c) const {
	    return colorNames.value(c,c.name());
    }

signals:

public slots:

private:
    const Galaxy *_galaxy;
    QMap<int,QColor> colors;
    QMap<QColor,QString> colorNames;
};

#endif // EQUIPMENTTABLEMODEL_H
