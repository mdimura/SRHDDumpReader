#include "PlanetsTableModel.h"

PlanetsTableModel::PlanetsTableModel(const Galaxy *galaxy, QObject *parent):
    QAbstractTableModel(parent),_galaxy(galaxy)
{

}

int PlanetsTableModel::rowCount(const QModelIndex &parent) const
{
    return _galaxy->planetCount();
}

int PlanetsTableModel::columnCount(const QModelIndex &parent) const
{
    return 31;
}
QVariant PlanetsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> header={tr("Name"),tr("Dist."),tr("Owner"),tr("Race"),
                                          tr("TL"),tr("Size"),tr("Economy"),
                                          tr("Gowernment"),tr("CI"),tr("CIP"),
                                          tr("Relation"),tr("TL1"),tr("TL2"),
                                          tr("TL3"), tr("TL4"),tr("TL5"),
                                          tr("TL6"), tr("TL7"),tr("TL8"),
                                          tr("TL9"), tr("TL10"),tr("TL11"),
                                          tr("TL12"), tr("TL13"),tr("TL14"),
                                          tr("TL15"), tr("TL16"),tr("TL17"),
                                          tr("TL18"), tr("TL19"),tr("TL20")};
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return _galaxy->planet(section).id();
        }
        else if (orientation == Qt::Horizontal) {
            return header.at(section);
        }
    }
    return QVariant();
}
QVariant PlanetsTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int col=index.column();
        int row=index.row();
        switch (col)
        {
        case 0:
            return _galaxy->planet(row).name();
        case 1:
            return std::round(_galaxy->planetDistance(row));
        case 2:
            return _galaxy->planet(row).owner();
        case 3:
            return _galaxy->planet(row).race();
        case 4:
            return _galaxy->planet(row).techLevel();
        case 5:
            return _galaxy->planet(row).size();
        case 6:
            return _galaxy->planet(row).economy();
        case 7:
            return _galaxy->planet(row).government();
        case 8:
            return _galaxy->planet(row).currentInvetion();
        case 9:
            return std::round(_galaxy->planet(row).currentInvetionPoints()*10.0)*0.1;
        case 10:
            return _galaxy->planet(row).relation();
        default:
            return _galaxy->planet(row).techLevel(col-11);
        }

        return QVariant("UC");
    }
    return QVariant();
}
