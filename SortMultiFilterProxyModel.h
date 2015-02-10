#ifndef SORTMULTIFILTERPROXYMODEL_H
#define SORTMULTIFILTERPROXYMODEL_H

#include <unordered_map>

#include <QSortFilterProxyModel>
#include <QString>
#include <QTimer>
#include <QRegularExpression>

class SortMultiFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit SortMultiFilterProxyModel(QObject *parent = 0);
public slots:
	void setMin(int col, double min)
	{
		if(min==_min[col]) {
			return;
		}
		_min[col]=min;
		correctMinMax(col);
		invalidateFilter();
	}
	void unsetMin(int col)
	{
		if(_min.count(col)==0) {
			return;
		}
		_min.erase(col);
		invalidateFilter();
	}
	void setMax(int col, double max)
	{
		if(_max[col]==max) {
			return;
		}
		_max[col]=max;
		correctMinMax(col);
		invalidateFilter();
	}
	void unsetMax(int col)
	{
		if(_max.count(col)==0) {
			return;
		}
		_max.erase(col);
		invalidateFilter();
	}
	void setMatch(int col, const QString& match )
	{
		if (match.isEmpty()) {
			unsetMatch(col);
			return;
		}
		if(_match[col]==match) {
			return;
		}
		_match[col]=match;
		invalidateFilter();
	}
	void unsetMatch(int col)
	{
		if(_match.count(col)==0) {
			return;
		}
		_match.erase(col);
		invalidateFilter();
	}
	void setNotMatch(int col, const QString& notMatch )
	{
		if (notMatch.isEmpty()) {
			unsetNotMatch(col);
			return;
		}
		if(_notMatch[col]==notMatch) {
			return;
		}
		_notMatch[col]=notMatch;
		invalidateFilter();
		//std::cout<<"setNotMatch end"<<std::endl;
	}
	void unsetNotMatch(int col)
	{
		if(_notMatch.count(col)==0) {
			return;
		}
		_notMatch.erase(col);
		invalidateFilter();
	}
	void setFilters(const QMap<int,QString>& match,
			const QMap<int,QString>& notMatch,
			const QMap<int,double>& min,
			const QMap<int,double>& max);
protected:
	bool filterAcceptsRow(int sourceRow,
			      const QModelIndex &sourceParent) const;
private:
	void correctMinMax(int col)
	{
		if (_min[col]>_max[col] /*|| _max[col]==0.0*/)
		{
			if(_max.count(col)==0) {
				return;
			}
			_max.erase(col);
		}
	}

	std::unordered_map<int, QString> _match;
	std::unordered_map<int, QString> _notMatch;
	std::unordered_map<int, double> _min;
	std::unordered_map<int, double> _max;
	QRegularExpression::PatternOption _caseSensitive=QRegularExpression::CaseInsensitiveOption;
	//QTimer timer;
};

#endif // SORTMULTIFILTERPROXYMODEL_H
