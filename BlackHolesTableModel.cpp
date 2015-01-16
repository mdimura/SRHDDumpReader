#include "BlackHolesTableModel.h"


BlackHolesTableModel::BlackHolesTableModel(const Galaxy *galaxy, QObject *parent):
    QAbstractTableModel(parent),_galaxy(galaxy)
{

}

int BlackHolesTableModel::rowCount(const QModelIndex &parent) const
{
    return _galaxy->blackHoleCount();
}

int BlackHolesTableModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant BlackHolesTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int col=index.column();
        int row=index.row();
        switch (col)
        {
        case 0:
            return _galaxy->blackHoleStar1(row);
        case 1:
            return std::round(_galaxy->blackHoleStar1Distance(row)*10.0)*0.1;
        case 2:
            return _galaxy->blackHoleStar2(row);
        case 3:
            return std::round(_galaxy->blackHoleStar2Distance(row)*10.0)*0.1;
        case 4:
            return _galaxy->blackHoleTurnsToClose(row);
        default:
            return QVariant();
        }

        return index.row()+index.column();
    }
    return QVariant();
}

QVariant BlackHolesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> header={tr("Star 1"),tr("Dist."),tr("Star 2"),
                                          tr("Dist."), tr("Days left")};
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return _galaxy->blackHoleId(section);
        }
        else if (orientation == Qt::Horizontal) {
            return header.at(section);
        }
    }
    return QVariant();
}
