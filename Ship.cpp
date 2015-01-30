#include "Ship.h"
#include "Galaxy.h"

Ship::Ship(QTextStream &stream, Galaxy& galaxy, unsigned id, unsigned starId):_id(id),_relation(0),_money(0),_starId(starId),_special(0)
{
	const static QMap<QString,int> shipOptions={
		{"ICurStarId",0},{"IFullName",1},{"Goods",2},{"Money",3},
		{"EqList ^{",4},
		{"ArtsList ^{",5},{"EqShop ^{",6},{"Storage ^{",7},{"ShopGoods",8},
		{"ShopGoodsSale",9},{"ShopGoodsBuy",10},{"IType",11}
	};

	int nesting=0;
	QString line = stream.readLine().trimmed();;
	while (!line.isNull())
	{
		nesting+=line.contains('{');
		nesting-=line.contains('}');
		if(line.startsWith("ShipId"))
		{
			nesting-=1;
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			_id=idRef.toInt();

		}
		if(line.startsWith("WarriorId"))
		{
			nesting-=1;
			QStringRef idRef(&line,9,line.indexOf(' ')-9);
			_id=idRef.toInt();
		}
		if(nesting<0)
		{
			break;
		}
		const QString& varname=line.section('=',0,0);
		const QString& value=line.section('=',-1);
		switch(shipOptions.value(varname,-1))
		{
		case 0://ICurStarId
			_starId=value.toInt();
			break;

		case 1://IFullName
			_fullName=value;
			break;

		case 2://Goods
			_goodsQuantity=GoodsArr(value);
			break;

		case 3://Money
			_money=value.toInt();
			break;

		case 4: //EqList
		case 5: //ArtsList
		{
			auto eqListAppend=readEqlist(stream, galaxy, Equipment::kShipEq,_id);
			_eqIdList.insert(eqListAppend.begin(),eqListAppend.end());
			--nesting;
		}
			break;

		case 6: //EqShop
		{
			auto eqListAppend=readEqlist(stream, galaxy, Equipment::kShipShop,_id);
			_eqIdList.insert(eqListAppend.begin(),eqListAppend.end());
			--nesting;
		}
			break;

		case 7: //Storage
		{
			auto eqListAppend=readEqlist(stream, galaxy, Equipment::kShipStorage,_id);
			_eqIdList.insert(eqListAppend.begin(),eqListAppend.end());
			--nesting;
		}
			break;

		case 8://ShopGoods
			_goodsShopQuantity=GoodsArr(value);
			break;

		case 9://ShopGoodsSale
			_goodsSale=GoodsArr(value);
			break;

		case 10://ShopGoodsBuy
			_goodsBuy=GoodsArr(value);
			break;

		case 11://IType
			_type=value;
			break;

		default:
			//skip record
			break;
		}
		line = stream.readLine().trimmed();
	}
	compactifyName();
	galaxy.addShip(std::move(*this));
}

void Ship::compactifyName()
{
	if(_type=="RC")
	{
		_fullName.replace("Ranger Center","RC");
		_fullName.replace("Центр рейнджеров","ЦР");
	}
	else if(_type=="SB")
	{
		_fullName.replace("Research Station","SB");
		_fullName.replace("Научная база","НБ");
	}
	else if(_type=="MC")
	{
		_fullName.replace("Medical Center","MC");
		_fullName.replace("Медицинский центр","МЦ");
	}
	else if(_type=="PB")
	{
		_fullName.replace("Pirate Base","PB");
		_fullName.replace("Пиратская база","ПБ");
	}
	else if(_type=="BK")
	{
		_fullName.replace("Business Center","BC");
		_fullName.replace("Бизнес-центр","БЦ");
	}
	else if(_type=="WB")
	{
		_fullName.replace("Military Base","MB");
		_fullName.replace("Военная база","ВБ");
	}
	else if(_type=="")
	{
		_fullName.replace("","");
	}

}


void readShiplist(QTextStream &stream, Galaxy &galaxy, unsigned starId)
{
	QString line = stream.readLine().trimmed();
	while (!line.isNull())
	{
		if(line.startsWith("ShipId"))
		{
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			unsigned id=idRef.toInt();
			Ship(stream, galaxy, id,starId);
		}
		else if(line.startsWith("WarriorId"))
		{
			QStringRef idRef(&line,9,line.indexOf(' ')-9);
			unsigned id=idRef.toInt();
			Ship(stream, galaxy, id,starId);
		}
		else if(line.contains('}'))
		{
			break;
		}
		line = stream.readLine().trimmed();
	}
}
