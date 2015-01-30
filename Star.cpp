#include "Star.h"
#include "Galaxy.h"

void readStars(QTextStream &stream, Galaxy &galaxy)
{

	QString line = stream.readLine().trimmed();
	while (!line.isNull())
	{
		if(line.contains("StarId"))
		{
			QStringRef idRef(&line,6,line.indexOf(' ')-6);
			unsigned id=idRef.toInt();
			galaxy.addStar(Star(stream, galaxy, id));
		}
		else if(line.contains('}'))
		{
			break;
		}
		line = stream.readLine().trimmed();
	}
}

Star::Star(QTextStream &stream, Galaxy &galaxy, unsigned id):_id(id),_x(0.0),_y(0.0)
{
	const static QMap<QString,int> starOptions={
		{{"StarName",0},{"X",1},{"Y",2},{"Owners",3},{"DomSeries",4},{"ShipList ^{",5},{"PlanetList ^{",6},{"Junk ^{",7}}
	};

	int nesting=0;
	QString line = stream.readLine().trimmed();
	while (!line.isNull())
	{
		nesting+=line.contains('{');
		nesting-=line.contains('}');
		if(line.startsWith("StarId"))
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

		switch(starOptions.value(varname,-1))
		{
		case 0://StarName
			_name=value;
			break;

		case 1://X
			value.replace(',','.');
			_x=value.toDouble();
			break;

		case 2://Y
			value.replace(',','.');
			_y=value.toDouble();
			break;

		case 3://Owners
			_owner=value;
			break;

		case 4://DomSeries
			_domSeries=value;
			break;

		case 5://ShipList ^{
			readShiplist(stream,galaxy,_id);
			--nesting;
			break;

		case 6://PlanetList ^{
			readPlanets(stream,galaxy,_id);
			--nesting;
			break;

		case 7://Junk ^{
			readEqlist(stream,galaxy,Equipment::kJunk,_id);
			--nesting;
			break;

		default:
			//skip record
			break;
		}
		line = stream.readLine().trimmed();
	}
}
