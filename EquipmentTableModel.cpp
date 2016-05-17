#include "EquipmentTableModel.h"
#include "Galaxy.h"
#include "FilterHorizontalHeaderView.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QComboBox>

EquipmentTableModel::EquipmentTableModel(const Galaxy *galaxy, QObject *parent) :
	QAbstractTableModel(parent),_galaxy(galaxy)
{
	for(const QString& name:QColor::colorNames()) {
		QColor c(name);
		colorNames[c.rgb()]=name;
	}
}

int EquipmentTableModel::rowCount(const QModelIndex &parent) const
{
	return _galaxy->equipmentCount();
}

int EquipmentTableModel::columnCount(const QModelIndex &parent) const
{
	return 14;
}

QVariant EquipmentTableModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::BackgroundRole) {
		return QBrush(colors.value(index.row(),QColor("white")));
	}
	int col=index.column();
	if (role == Qt::EditRole) {
		if (col==0) {
			return colors.value(index.row(),QColor("white"));
		}
	}
	if (role == Qt::ForegroundRole) {
		QColor bg=colors.value(index.row(),QColor("white"));
		int lightness;//=bg.lightness();
		lightness=bg.red()*0.299 + bg.green()*0.587 + bg.blue()*0.114;
		if(lightness<128) {
			return QBrush(QColor("white"));
		}
		return QBrush(QColor("black"));
	}
	if (role == Qt::DisplayRole)
	{
		switch (col)
		{
		case 0:
			return colorName(colors.value(index.row(),QColor("white")));
			break;
		case 1:
			return _galaxy->equipmentName(index.row());
			break;

		case 2:
			return _galaxy->equipmentType(index.row());
			break;

		case 3:
			return _galaxy->equipmentSize(index.row());
			break;

		case 4:
			return _galaxy->equipmentOwner(index.row());
			break;

		case 5:
			return _galaxy->equipmentCost(index.row());
			break;

		case 6:
			return _galaxy->equipmentTechLevel(index.row());
			break;

		case 7:
			return _galaxy->equipmentLocationType(index.row());
			break;

		case 8:
			return _galaxy->equipmentLocationName(index.row());
			break;

		case 9:
			return _galaxy->equipmentStarName(index.row());
			break;

		case 10://Distance to star from Player
			return std::round(_galaxy->equipmentDistFromPlayer(index.row()));
			break;

		case 11://Star Owner
			return _galaxy->equipmentStarOwner(index.row());
			break;

		case 12:
			return std::round(_galaxy->equipmentDurability(index.row())*10.0)/10.0;
			break;

		case 13:
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
	static const QVector<QString> header={tr("Color"),tr("Name"),tr("Type"),tr("Size"),tr("Made"),
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
			case 0:
				return FilterHorizontalHeaderView::wtNone;
			case 3:
			case 5:
			case 6:
			case 10:
				return FilterHorizontalHeaderView::wtInt;
			case 12:
				return FilterHorizontalHeaderView::wtDouble;
			default:
				return FilterHorizontalHeaderView::wtString;
			}
		}
	}
	return QVariant();
}

Qt::ItemFlags EquipmentTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	if (index.column()==0) {
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	}
	return QAbstractItemModel::flags(index);
}

bool EquipmentTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::EditRole && index.column()==0) {
		colors[index.row()]=value.value<QColor>();
		emit dataChanged(index, index);
		return true;
	}
	return false;
}



bool operator<(const QColor &a, const QColor &b) {
	return a.redF() < b.redF()
			|| a.greenF() < b.greenF()
			|| a.blueF() < b.blueF()
			|| a.alphaF() < b.alphaF();
}
