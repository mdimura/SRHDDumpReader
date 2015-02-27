#ifndef STAR_H
#define STAR_H
#include <unordered_map>
#include <QTextStream>
#include <QPointF>
#include "Equipment.h"
#include "Ship.h"
#include "Planet.h"
class Star
{
public:
	Star(QTextStream &stream, Galaxy& galaxy, unsigned id=0);
	unsigned id() const
	{
		return _id;
	}
	QString name() const
	{
		return _name;
	}
	QString owner() const
	{
		return _owner;
	}
	QString domSeries() const
	{
		return _domSeries;
	}
	QPointF position() const
	{
		return QPointF(_x,_y);
	}
private:
	unsigned _id;
	QString _name, _owner, _domSeries;
	double _x, _y;
};
void readStars(QTextStream &stream, Galaxy &galaxy);
#endif // STAR_H
