#include "EquipmentTableModel.h"
#include "Galaxy.h"
#include "FilterHorizontalHeaderView.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

EquipmentTableModel::EquipmentTableModel(const Galaxy *galaxy, QObject *parent) :
    QAbstractTableModel(parent),_galaxy(galaxy)
{
}

int EquipmentTableModel::rowCount(const QModelIndex &parent) const
{
    return _galaxy->equipmentCount();
}

int EquipmentTableModel::columnCount(const QModelIndex &parent) const
{
    return 13;
}

QVariant EquipmentTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int col=index.column();
        switch (col)
        {
        case 0:
            return _galaxy->equipmentName(index.row());
        break;

        case 1:
            return _galaxy->equipmentType(index.row());
        break;

        case 2:
            return _galaxy->equipmentSize(index.row());
        break;

        case 3:
            return _galaxy->equipmentOwner(index.row());
        break;

        case 4:
            return _galaxy->equipmentCost(index.row());
        break;

        case 5:
            return _galaxy->equipmentTechLevel(index.row());
        break;

        case 6:
            return _galaxy->equipmentLocationType(index.row());
        break;

        case 7:
            return _galaxy->equipmentLocationName(index.row());
        break;

        case 8:
            return _galaxy->equipmentStarName(index.row());
        break;

        case 9://Distance to star from Player
            return std::round(_galaxy->equipmentDistFromPlayer(index.row()));
        break;

        case 10://Star Owner
            return _galaxy->equipmentStarOwner(index.row());
        break;

        case 11:
            return std::round(_galaxy->equipmentDurability(index.row())*10.0)/10.0;
        break;

        case 12:
            return _galaxy->equipmentBonus(index.row());
        break;

        }

        return "asd";
    }
    if (role==Qt::ToolTipRole) {
        QString str=data(index,Qt::DisplayRole).toString();
        if(! str.isEmpty()) {
            return QStringLiteral("<p>")+str+"</p>";
        }
    }
    return QVariant();
}

QVariant EquipmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> header={tr("Name"),tr("Type"),tr("Size"),tr("Made"),
                                          tr("Cost"),tr("TL"), tr("Location type"),
                                          tr("Location"),tr("Star"),tr("Dist."), tr("Owner"), tr("Durab."),tr("Bonus")};
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return _galaxy->equipmentId(section);
        }
        else if (orientation == Qt::Horizontal) {
            return header.at(section);
        }
    }
    else if(role==Qt::UserRole)
    {
        if(orientation == Qt::Horizontal) {
            switch (section)
            {
            case 2:
            case 4:
            case 5:
            case 9:
                return FilterHorizontalHeaderView::wtInt;
            case 11:
                return FilterHorizontalHeaderView::wtDouble;
            default:
                return FilterHorizontalHeaderView::wtString;
            }
        }
    }
    return QVariant();
}


