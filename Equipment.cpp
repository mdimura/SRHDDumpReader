#include "Equipment.h"
#include "Ship.h"
#include "Galaxy.h"

#include <QRegularExpression>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

const QMap<QString,QString> Equipment::specialTypesDescriptions=Equipment::loadDescriptions("bonus_descriptions.json");
const QMap<QString,QString> Equipment::specialHullsDescriptions=Equipment::loadDescriptions("bonus_descriptions_hulls.json");
const QMap<QString,QString> Equipment::micromodulesDescriptions=Equipment::loadDescriptions("bonus_descriptions_micromodules.json");
const QMap<QString,QString> Equipment::artifactsDescriptions=Equipment::loadDescriptions("bonus_descriptions_artifacts.json");

Equipment::Equipment(QTextStream &stream, Galaxy &galaxy, LocationType locationType, unsigned locationId, unsigned id):
	_id(id),_size(0),_cost(0),_durability(0.0),_techLevel(0),_locationType(locationType),_locationId(locationId)
{
	const static QMap<int,QString> intToLandType={{0,"water"},{1,"plain"},{2,"mountains"}};
	const static QMap<QString,int> eqOptions={
		{"IName",0},{"IType",1},{"Owner",2},{"Size",3},{"Cost",4},
		{"Durability",5},{"TechLevel",6},{"ISpecialName",7},
		{"LandType",8},{"Depth",9}

	};

	QString line = stream.readLine().trimmed();;
	while (!line.isNull())
	{
		if(line[0]=='}')
		{
			break;
		}
		else if(line.startsWith("ShipId"))
		{
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			unsigned shipId=idRef.toInt();
			Ship(stream,galaxy,shipId,0);
		}
		else if(line.startsWith("ItemId"))
		{
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			_id=idRef.toInt();
		}


		int breakPosition=line.indexOf('=');
		QString varname=line.left(breakPosition);
		QString value=line.mid(breakPosition+1);

		switch(eqOptions.value(varname,-1))
		{
		case 0://IName
			_name=std::move(value.remove("</color>"));
			_name.remove('"');
			_name.remove(QRegularExpression("<color=([0-9]*,*)*>"));
			break;

		case 1://Itype
			_type=std::move(value);
			break;

		case 2://Owner
			_owner=std::move(value);
			break;

		case 3://Size
			_size=value.toInt();
			break;

		case 4://Cost
			_cost=value.toInt();
			break;

		case 5://Durability
			value.replace(',','.');
			_durability=value.toDouble();
			break;

		case 6://TechLevel
			_techLevel=value.toInt();
			break;

		case 7://ISpecialName
			_specialName=std::move(value);
			break;
		case 8://LandType
			extraFields.insert(std::move(varname),intToLandType.value(value.toInt()));
			break;
		case 9://Depth
			extraFields.insert(std::move(varname),value);
			break;
		default:
			//extraFields[varname]=value;
			break;
		}
		line = stream.readLine().trimmed();
	}
	galaxy.addEquipment(std::move(*this));
}

QString Equipment::locationTypeString() const
{
	switch (_locationType)
	{
	case kShipEq:
		return QObject::tr("Ship/equipment");
	case kShipStorage:
		return QObject::tr("Ship/storage");
	case kShipShop:
		return QObject::tr("Shop");
	case kJunk:
		return QObject::tr("Space");
	case kPlanetShop:
		return QObject::tr("Shop");
	case kPlanetStorage:
		return QObject::tr("Planet/storage");
	case kPlanetTreasure:
		return QObject::tr("treasure");
	}
	return "";
}
unsigned Equipment::size() const
{
	return _size;
}
unsigned Equipment::cost() const
{
	return _cost;
}
unsigned Equipment::techLevel() const
{
	return _techLevel;
}
double Equipment::durability() const
{
	return _durability;
}
QString Equipment::type() const
{
	return _type;
}
QString Equipment::owner() const
{
	return _owner;
}

QString Equipment::bonusNote() const
{
	if(_type=="Nod") {
		return micromodulesDescriptions.value(_name.section(' ',1),"no description");
	}
	if(_type.startsWith("Art")) {
		return artifactsDescriptions.value(_type,"no description");
	}
	if(_specialName.isEmpty()) {
		return "";
	}
	if(_type.at(0)=='W') {//weapon
		return _specialName;
	}

	int lvl=0;
	if(_name.contains(QRegularExpression(" I($| )"))) {
		lvl=1;
	}
	else if(_name.contains(QRegularExpression(" II($| )"))) {
		lvl=2;
	}
	else if(_name.contains(QRegularExpression(" III($| )"))) {
		lvl=3;
	}
	else if(_name.contains(QRegularExpression(" IV($| )"))) {
		lvl=4;
	}

	if (lvl==0)
	{
		return "error parsing";
	}

	QString specialType;
	QString bonus;
	if(_type=="Hull") {
		specialType=_name;
		specialType.replace("Корпус ","");
		specialType=specialType.section(' ',0,0);
		QString key=specialType+":"+QString::number(lvl);
		bonus=specialHullsDescriptions.value(key,"no description");
	}
	else {//not Hull
		specialType=_name.section(' ',0,0);
		QString key=specialType+":"+QString::number(lvl);
		bonus=specialTypesDescriptions.value(key,"no description");
	}

	return specialType+' '+QString::number(lvl)+": "+bonus;
}

QMap<QString,QString> mapFromJson(const QJsonArray &array)
{
	QMap<QString,QString> map;
	for(const auto& entry:array)
	{
		auto obj=entry.toObject();
		map[obj["key"].toString()]=obj["value"].toString();
	}
	if(map.empty())
	{
		std::cout<<"the Map is empty!"<<std::endl;
	}
	return map;
}
void createDescriptionsHulls();
void createDescriptions();
QMap<QString, QString> Equipment::loadDescriptions(const QString &filename)
{
	//createDescriptions();
	//createDescriptionsHulls();
	QMap<QString,QString> descriptions;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		std::cerr<<"could not open "<<filename.toStdString()<<std::endl;
		return descriptions;
	}
	QByteArray data = file.readAll();
	auto arr=QJsonDocument::fromJson(data).array();
	descriptions=mapFromJson(arr);
	return descriptions;
}


std::unordered_set<unsigned> readEqlist(QTextStream &stream, Galaxy& galaxy, Equipment::LocationType locationType, unsigned locationId)
{
	std::unordered_set<unsigned> eqIdList;

	int nesting=0;
	QString line = stream.readLine().trimmed();;
	while (!line.isNull())
	{
		nesting+=line.contains('{');
		nesting-=line.contains('}');
		if(line.startsWith("Storage ^{") || line.startsWith("Treasure ^{") ||
		   line.startsWith("EqShop ^{") || line.startsWith("EqList ^{") ||
		   line.startsWith("EqList ^{") || line.startsWith("ArtsList ^{") || line.startsWith("Junk ^{"))
		{
			nesting-=1;
		}
		if(nesting<0)
		{
			break;
		}
		if(line.startsWith("HiddenItem")) {
			Equipment eq(stream, galaxy, locationType, locationId);
			eqIdList.insert(eq.id());
		}
		if(line.startsWith("ItemId"))
		{
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			unsigned id=idRef.toInt();
			Equipment eq(stream, galaxy, locationType, locationId, id);
			--nesting;//stream wiil be moved after '} ItemId'
			eqIdList.insert(eq.id());
		}
		line = stream.readLine().trimmed();
	}
	return eqIdList;
}
void createDescriptions()
{
	QFile file("bonus_descriptions.json.en.example");
	if(file.exists()){
		return;
	}
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QStringList types={"Myoplasmic","Pygamore","Transcendental",
			   "Upgraded","Biogenic","Nanobrolite","Ultraalloy"};

	QJsonArray ar;
	for(auto& specialType:types)
		for(int lvl=1; lvl<4; lvl++)
		{
			QJsonObject obj;
			obj["key"]=specialType+":"+QString::number(lvl);
			QString bonus;
			if(specialType=="Myoplasmic")
			{
				static const int arr[]={2,3,5};
				bonus=QString("energetic dmg +%1, splinter dmg +%2, speed -%3")
				      .arg(arr[lvl-1]).arg(arr[lvl-1]+1).arg(lvl*10);
			}
			else if(specialType=="Pygamore") {
				static const int arr[]={50,100,250};
				bonus=QString("rocket dmg+%1, defence -%2, radar -%3")
				      .arg(lvl+1).arg(lvl).arg(arr[lvl-1]);
			}
			else if(specialType=="Transcendental") {
				bonus=QString("defence +%1, mass +%2\%")
				      .arg(lvl+1).arg(lvl*10);
			}
			else if(specialType=="Upgraded") {
				bonus=QString("hook range +%1, mobility +1, charizm -2")
				      .arg(lvl*5+5);
			}
			else if(specialType=="Biogenic") {
				static const int arr[]={100,200,200};
				bonus=QString("scaner +%1, radar +%2, technics -2")
				      .arg(lvl*3).arg(arr[lvl-1]);
			}
			else if(specialType=="Nanobrolite") {
				static const int arr[]={3,5,8};
				bonus=QString("energetic dmg +%1, splinter dmg +%2, weapon range -%3, attack +1, trade -2")
				      .arg(lvl*2).arg(arr[lvl-1]).arg(10*lvl);
			}
			else if(specialType=="Ultraalloy") {
				static const int drd[]={7,12,15};
				static const int spd[]={10,25,55};
				bonus=QString("droid +%1, engine speed -%2, attack -1, technics +1")
				      .arg(drd[lvl-1]).arg(spd[lvl-1]);
			}
			obj["value"]=bonus;
			ar.append(obj);
		}
	QJsonDocument doc(ar);
	file.write(doc.toJson());
	file.close();
}
void createDescriptionsHulls()
{
	QFile file("bonus_descriptions_hulls.json.en.example");
	if(file.exists()){
		return;
	}
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QStringList types={"Fast","Invincible","Repair","Myoplasmic",
			   "Pygamore","Ironclad","Conscientious",
			   "Kindhearted","Peaceful"};

	QJsonArray ar;
	for(auto& specialType:types)
		for(int lvl=1; lvl<5; lvl++)
		{
			QJsonObject obj;
			obj["key"]=specialType+":"+QString::number(lvl);
			QString bonus;
			int effLvl=lvl<4?lvl:2;
			if(specialType=="Fast"){
				bonus=QString("speed +%1").arg(effLvl*50+50);
			}
			else if(specialType=="Invincible") {//Неуязвимый
				bonus=QString("defence +%1").arg(effLvl*3);
			}
			else if(specialType=="Repair") {
				bonus=QString("droid +%1").arg((effLvl)*15-5);
			}
			else if(specialType=="Myoplasmic"){
				bonus=QString("energetic dmg +%1, splinter dmg +%2, speed -%3")
				      .arg(3*effLvl+1).arg( 2*(2+pow(2,effLvl)) ).arg(effLvl*20);
			}
			else if(specialType=="Pygamore") {
				bonus=QString("rocket dmg +%1, defence -%2, radar -%3")
				      .arg(effLvl*5).arg(effLvl*2+1).arg(pow(2,effLvl-1)*100+100);
			}
			else if(specialType=="Dandy") {//Молния (Стиляга)
				static const int def[]={3,4,6};
				bonus=QString("radar +%1, jump +%1, defence -%2")
				      .arg(50+effLvl*50).arg(effLvl*5).arg(def[effLvl-1]);
			}
			else if(specialType=="Ironclad") {//Броненосец
				static const int def[]={3,4,6};
				bonus=QString("energetic dmg +%1, splinter dmg +%2, defence +%3, speed -%4, manoeuvrability -2")
				      .arg(effLvl*2).arg(effLvl*2-1).arg(def[effLvl-1]).arg(25+25*effLvl);
			}
			else if(specialType=="") {//Ушастик //TODO: find out
				bonus=QString("rocket dmg +%1, charisma +3")
				      .arg(effLvl*2);
			}
			else if(specialType=="Conscientious") {//Честный ренегал
				bonus=QString("jump +%1, hook +%2, mass -%3\%, trade -3")
				      .arg(effLvl*2+1).arg(effLvl*10).arg(effLvl*10);
			}
			else if(specialType=="Kindhearted") {//Добрый лякуша
				bonus=QString("energetic dmg +%1, splinter dmg +%2, rocket dmg +%3")
				      .arg(effLvl+4).arg(effLvl+3).arg(effLvl+1);
			}
			else if(specialType=="Peaceful") {//Мирный корсар
				bonus=QString("armor +%1, defence +%2, atack -3, technics -3")
				      .arg(effLvl).arg(effLvl*3);
			}
			obj["value"]=bonus;
			ar.append(obj);
		}
	QJsonDocument doc(ar);
	file.write(doc.toJson());
	file.close();
}
