#include "BlackHole.h"
#include "Galaxy.h"

BlackHole::BlackHole(QTextStream &stream, unsigned id):_id(id)
{
    const static QMap<QString,int> bhOptions={
        {"Star1Id",0},
        {"Star2Id",1},
        {"TurnsToClose",2}
    };

    int nesting=0;
    QString line = stream.readLine().trimmed();
    while (!line.isNull())
    {
        nesting+=line.contains('{');
        nesting-=line.contains('}');
        if(line.startsWith("HoleId"))
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

        switch(bhOptions.value(varname,-1))
        {
        case 0://Star1Id
            _star1Id=value.toInt();
        break;

        case 1://Star2Id
            _star2Id=value.toInt();
        break;

        case 2://TurnsToClose
            _turnsToClose=value.toInt();
        break;
        default:
            //skip record
        break;
        }
        line = stream.readLine().trimmed();
    }
}
unsigned BlackHole::id() const
{
    return _id;
}

unsigned BlackHole::star1Id() const
{
    return _star1Id;
}

unsigned BlackHole::star2Id() const
{
    return _star2Id;
}

unsigned BlackHole::turnsToClose() const
{
    return _turnsToClose;
}

void readBlackHoles(QTextStream &stream,Galaxy &galaxy)
{
    QString line = stream.readLine().trimmed();;
    while (!line.isNull())
    {
        if(line.contains("HoleId"))
        {
            QStringRef idRef(&line,6,line.indexOf(' ')-6);
            unsigned id=idRef.toInt();
            galaxy.addBlackHole(BlackHole(stream, id));
        }
        else if(line.contains('}'))
        {
            break;
        }
        line = stream.readLine().trimmed();
    }
}
