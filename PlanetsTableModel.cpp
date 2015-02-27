#include "PlanetsTableModel.h"
#include "FilterHorizontalHeaderView.h"

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
    return 32;
}
QVariant PlanetsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> header={tr("Name"),tr("Star"),tr("Dist."),tr("Owner"),tr("Race"),
					  tr("TL"),tr("Size"),tr("Economy"),
					  tr("Government"),tr("CI"),tr("CIP"),
					  tr("Relation"),tr("TL0"),tr("TL1"),
					  tr("TL2"),
					  tr("TL3"), tr("TL4"),tr("TL5"),
					  tr("TL6"), tr("TL7"),tr("TL8"),
					  tr("TL9"), tr("TL10"),tr("TL11"),
					  tr("TL12"), tr("TL13"),tr("TL14"),
					  tr("TL15"), tr("TL16"),tr("TL17"),
					  tr("TL18"), tr("TL19")};
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
    if(orientation==Qt::Horizontal && role==Qt::UserRole)
    {
	if (section==2) {
	    return FilterHorizontalHeaderView::wtInt;
	}
	else if(section<5) {
	    return FilterHorizontalHeaderView::wtString;
	}
	else if(section==10) {
	    return FilterHorizontalHeaderView::wtDouble;
	}
	else if (section<7 || section>8) {
	    return FilterHorizontalHeaderView::wtInt;
	}
	return FilterHorizontalHeaderView::wtString;
    }
    return QVariant();
}
QVariant PlanetsTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
	int col=index.column();
	int row=index.row();
	if(_galaxy->planet(row).owner()=="None" && col>2) {
	    return "-";
	}
	switch (col)
	{
	case 0:
	    return _galaxy->planet(row).name();
	case 1:
	    return _galaxy->planetStarName(row);
	case 2:
	    return std::round(_galaxy->planetDistance(row));
	case 3:
	    return _galaxy->planetOwner(row);
	case 4:
	    return _galaxy->planet(row).race();
	case 5:
	    return _galaxy->planet(row).techLevel();
	case 6:
	    return _galaxy->planet(row).size();
	case 7:
	    return _galaxy->planet(row).economy();
	case 8:
	    return _galaxy->planet(row).government();
	case 9:
	    return _galaxy->planet(row).currentInvetion();
	case 10:
	    return std::round(_galaxy->planet(row).currentInvetionPoints()*1000.0)*0.001;
	case 11:
	    return _galaxy->planet(row).relation();
	default:
	    return _galaxy->planet(row).techLevel(col-12);
	}

	return QVariant("UC");
    }
    return QVariant();
}
