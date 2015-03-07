#ifndef PLANET_H
#define PLANET_H
#include <QString>
#include "Equipment.h"
#include "GoodsArr.h"
#include <array>
class Planet
{
public:
	Planet(QTextStream &stream, Galaxy& galaxy, unsigned id=0, unsigned starId=0);
	unsigned id() const
	{
		return _id;
	}
	bool hasMarket() const
	{
		return _owner!="None" && _owner!="Kling";
	}
	QString name() const
	{
		return _name;
	}
	const GoodsArr& goodsCount() const
	{
		return _goodsShopQuantity;
	}
	const GoodsArr& goodsSale() const
	{
		return _goodsSale;
	}
	const GoodsArr& goodsBuy() const
	{
		return _goodsBuy;
	}
	unsigned starId() const
	{
		return _starId;
	}
	unsigned size() const
	{
		return _size/10-5;
	}
	unsigned techLevel() const
	{
		return _techLevel;
	}
	unsigned techLevel(int i) const
	{
		return _techLevels[i];
	}
	QString economy() const
	{
		return _economy;
	}
	QString owner() const
	{
		return _owner;
	}
	void readTechLevels(QString &str)
	{
		QTextStream a(&str);
		char c;
		a>>_techLevels[0];
		for(int i=1; i<20; i++)
		{
			a>>c>>_techLevels[i];
		}
	}
	float currentInvetionPoints() const
	{
		return _currentInventionPoints;
	}
	unsigned currentInvetion() const
	{
		return _currentInvention;
	}
	QString race() const
	{
		return _race;
	}
	QString government() const
	{
		return _government;
	}
	unsigned relation() const
	{
		return _relation;
	}
private:
	QString _name, _owner, _race, _economy, _government;
	unsigned _id, _size, _relation, _techLevel;
	unsigned _waterSpace, _landSpace, _hillSpace;
	unsigned _waterComplete, _landComplete, _hillComplete;
	unsigned _orbitCount, _currentInvention;
	float _currentInventionPoints;
	std::unordered_set<unsigned> _eqIdList;
	GoodsArr _goodsShopQuantity, _goodsSale, _goodsBuy;
	std::array<unsigned,20> _techLevels;
	unsigned _starId;
};
void readPlanets(QTextStream &stream, Galaxy &galaxy, unsigned starId);
#endif // PLANET_H
