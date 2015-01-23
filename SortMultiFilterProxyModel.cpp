#include "SortMultiFilterProxyModel.h"

SortMultiFilterProxyModel::SortMultiFilterProxyModel(QObject *parent):QSortFilterProxyModel(parent)
{
    timer.setInterval(300);
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(invalidate()));
}

bool SortMultiFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	int colCnt=sourceModel()->columnCount();
	QVector<QModelIndex> indices;
	indices.reserve(colCnt);
	for(int col=0; col<colCnt; col++)
	{
		indices.push_back(sourceModel()->index(sourceRow, col, sourceParent));
	}
	for(const auto& pair:_min)
	{
		int col=pair.first;
		double cellValue=sourceModel()->data(indices[col]).toDouble();
		if(cellValue<pair.second) {
			return false;
		}
	}
	for(const auto& pair:_max)
	{
		int col=pair.first;
		double cellValue=sourceModel()->data(indices[col]).toDouble();
		if(cellValue>pair.second) {
			return false;
		}
	}
	for(const auto& pair:_match)
	{
		int col=pair.first;
		const QString& cellValue=sourceModel()->data(indices[col]).toString();
		if(! cellValue.contains(QRegularExpression(pair.second,_caseSensitive))) {
			return false;
		}
	}
	for(const auto& pair:_notMatch)
	{
		int col=pair.first;
		const QString& cellValue=sourceModel()->data(indices[col]).toString();
		if(cellValue.contains(QRegularExpression(pair.second,_caseSensitive))) {
			return false;
		}
	}
	return true;
}

