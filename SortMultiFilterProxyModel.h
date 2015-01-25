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
        invalidate();
    }
    void unsetMin(int col)
    {
        if(_min.count(col)==0) {
            return;
        }
        _min.erase(col);
        invalidate();
    }
    void setMax(int col, double max)
    {
        if(_max[col]==max) {
            return;
        }
        _max[col]=max;
        invalidate();
    }
    void unsetMax(int col)
    {
        if(_max.count(col)==0) {
            return;
        }
        _max.erase(col);
        invalidate();
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
        invalidate();
    }
    void unsetMatch(int col)
    {
        if(_match.count(col)==0) {
            return;
        }
        _match.erase(col);
        invalidate();
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
        invalidate();
        //std::cout<<"setNotMatch end"<<std::endl;
    }
    void unsetNotMatch(int col)
    {
        if(_notMatch.count(col)==0) {
            return;
        }
        _notMatch.erase(col);
        invalidate();
    }

    void setMinDelayed(int col, double min)
    {
        _min[col]=min;
        timer.start();
    }
    void unsetMinDelayed(int col)
    {
        _min.erase(col);
        timer.start();
    }
    void setMaxDelayed(int col, double max)
    {
        _max[col]=max;
        timer.start();
    }
    void unsetMaxDelayed(int col)
    {
        _max.erase(col);
        timer.start();
    }
    void setMatchDelayed(int col, const QString& match )
    {
        if (match.isEmpty()) {
            unsetMatch(col);
            return;
        }
        _match[col]=match;
        timer.start();
    }
    void unsetMatchDelayed(int col)
    {
        _match.erase(col);
        timer.start();
    }
    void setNotMatchDelayed(int col, const QString& notMatch )
    {

        if (notMatch.isEmpty()) {
            unsetNotMatch(col);
            return;
        }
        _notMatch[col]=notMatch;
        timer.start();
        //std::cout<<"setNotMatch end"<<std::endl;
    }
    void unsetNotMatchDelayed(int col)
    {
        _notMatch.erase(col);
        timer.start();
    }

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex &sourceParent) const;
private:
    std::unordered_map<int, QString> _match;
    std::unordered_map<int, QString> _notMatch;
    std::unordered_map<int, double> _min;
    std::unordered_map<int, double> _max;
    QRegularExpression::PatternOption _caseSensitive=QRegularExpression::CaseInsensitiveOption;
    QTimer timer;
};

#endif // SORTMULTIFILTERPROXYMODEL_H
