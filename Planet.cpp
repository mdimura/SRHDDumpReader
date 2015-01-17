#include "Planet.h"
#include "Galaxy.h"
#include <QMap>

Planet::Planet(QTextStream &stream, Galaxy &galaxy, unsigned id, unsigned starId):_id(id),_starId(starId)
{
	const static QMap<QString,int> planetOptions={
		{"PlanetName",0},
		{"Owner",1},
		{"Race",2},
		{"Economy",3},
		{"Goverment",4},
		{"ISize",5},
		{"RelationToPlayer",6},
		{"IMainTechLevel",7},
		{"OrbitCnt",8},
		{"ShopGoods",9},
		{"ShopGoodsSale",10},
		{"ShopGoodsBuy",11},
		{"WaterSpace",12},
		{"LandSpace",13},
		{"HillSpace",14},
		{"WaterComplate",15},
		{"LandComplate",16},
		{"HillComplate",17},
		{"EqShop ^{",18},
		{"Storage ^{",19},
		{"Treasure ^{",20},
		{"Garrison ^{",21},
		{"TechLevels",22},
		{"CurrentInvention",23},
		{"CurrentInventionPoints",24}
	};
	int nesting=0;
	QString line = stream.readLine().trimmed();;
	while (!line.isNull())
	{
		nesting+=line.contains('{');
		nesting-=line.contains('}');
		if(line.startsWith("PlanetId"))
		{
			nesting-=1;
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			_id=idRef.toInt();

		}
		if(nesting<0)
		{
			break;
		}
		QString varname=line.section('=',0,0);
		QString value=line.section('=',-1);
		switch(planetOptions.value(varname,-1))
		{
		case 0://PlanetName
			_name=value;
			break;

		case 1://Owner
			_owner=value;
			break;

		case 2://Race
			_race=value;
			break;

		case 3://Economy
			_economy=value;
			break;

		case 4://Goverment
			_government=value;
			break;

		case 5://ISize
			_size=value.toInt();
			break;

		case 6://RelationToPlayer
			_relation=value.toInt();
			break;

		case 7://IMainTechLevel
			_techLevel=value.toInt();
			break;

		case 8://OrbitCnt
			_orbitCount=value.toInt();
			break;

		case 9://ShopGoods
			_goodsShopQuantity=GoodsArr(value);
			break;

		case 10://ShopGoodsSale
			_goodsSale=GoodsArr(value);
			break;

		case 11://ShopGoodsBuy
			_goodsBuy=GoodsArr(value);
			break;

		case 12://WaterSpace
			_waterSpace=value.toInt();
			break;

		case 13://LandSpace
			_landSpace=value.toInt();
			break;

		case 14://HillSpace
			_hillSpace=value.toInt();
			break;

		case 15://WaterComplate
			_waterComplete=value.toInt();
			break;

		case 16://LandComplate
			_landComplete=value.toInt();
			break;

		case 17://HillComplate
			_hillComplete=value.toInt();
			break;

		case 18://EqShop ^{
			if(_owner=="None") {
				break;
			}
		{
			auto eqListAppend=readEqlist(stream, galaxy,Equipment::kPlanetShop,_id);
			_eqIdList.insert(eqListAppend.begin(),eqListAppend.end());
			--nesting;
		}
			break;

		case 19://Storage ^{
		{
			auto eqListAppend=readEqlist(stream, galaxy,Equipment::kPlanetStorage,_id);
			_eqIdList.insert(eqListAppend.begin(),eqListAppend.end());
			--nesting;
		}
			break;

		case 20://Treasure ^{
		{
			auto eqListAppend=readEqlist(stream, galaxy,Equipment::kPlanetTreasure,_id);
			_eqIdList.insert(eqListAppend.begin(),eqListAppend.end());
			--nesting;
		}
			break;

		case 21://Garrison ^{
			readShiplist(stream,galaxy,_starId);
			--nesting;
			break;

		case 22://TechLevels ^{
			readTechLevels(value);
			break;
		case 23://CurrentInvention ^{
			_currentInvention=value.toUInt();
			break;
		case 24://CurrentInventionPoints ^{
			_currentInventionPoints=value.toFloat();
			break;
		default:
			//skip record
			break;
		}
		line = stream.readLine().trimmed();
	}
	galaxy.addPlanet(std::move(*this));
}


void readPlanets(QTextStream &stream, Galaxy &galaxy, unsigned starId)
{
	QString line = stream.readLine().trimmed();;
	while (!line.isNull())
	{
		if(line.contains("PlanetId"))
		{
			QStringRef idRef(&line,8,line.indexOf(' ')-8);
			unsigned id=idRef.toInt();
			Planet(stream, galaxy, id,starId);
		}
		else if(line.contains('}'))
		{
			break;
		}
		line = stream.readLine().trimmed();
	}
}
