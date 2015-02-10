#include "SortMultiFilterProxyModel.h"

SortMultiFilterProxyModel::SortMultiFilterProxyModel(QObject *parent):QSortFilterProxyModel(parent)
{
    //timer.setInterval(300);
    //timer.setSingleShot(true);
    //connect(&timer,SIGNAL(timeout()),this,SLOT(invalidate()));
}

void SortMultiFilterProxyModel::setFilters(const QMap<int, QString> &match, const QMap<int, QString> &notMatch, const QMap<int, double> &min, const QMap<int, double> &max)
{
	_match.clear();
	_notMatch.clear();
	_min.clear();
	_max.clear();
	using MapIntStrCI=QMap<int, QString>::const_iterator;
	for (MapIntStrCI i = match.begin(); i != match.end(); ++i)
	{
		_match[i.key()]=i.value();
	}
	for (MapIntStrCI i = notMatch.begin(); i != notMatch.end(); ++i)
	{
		_notMatch[i.key()]=i.value();
	}

	using MapIntDoubleCI=QMap<int, double>::const_iterator;
	for (MapIntDoubleCI i = min.begin(); i != min.end(); ++i)
	{
		_min[i.key()]=i.value();
	}
	for (MapIntDoubleCI i = max.begin(); i != max.end(); ++i)
	{
		if(i.value()>=_min[i.key()]) {
			_max[i.key()]=i.value();
		}
	}
	invalidateFilter();
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

