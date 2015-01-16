#ifndef EQUIPMENT_H
#define EQUIPMENT_H
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QVector>
#include <unordered_set>
#include <unordered_map>
//#include <initializer_list>

class Galaxy;
class Equipment
{
public:
    enum LocationType {kShipEq, kShipStorage, kShipShop, kJunk, kPlanetShop, kPlanetStorage, kPlanetTreasure};
    Equipment(QTextStream &stream, Galaxy &galaxy, LocationType locationType, unsigned locationId, unsigned id=0);
    unsigned id() const
    {
        return _id;
    }
    QString locationTypeString() const;
    LocationType locationType() const
    {
        return _locationType;
    }
    unsigned locationId() const
    {
        return _locationId;
    }
    QString name() const
    {
        return _name;
    }
    QString extra(const QString& key) const
    {
        return extraFields.value(key,"none");
    }

    unsigned size() const;
    unsigned cost() const;
    unsigned techLevel() const;
    double durability() const;

    QString type() const;

    QString owner() const;

    QString bonusNote() const;
private:
    QString _name;
    QString _type;
    QString _owner;
    unsigned _id;
    unsigned _size;
    unsigned _cost;
    double _durability;
    unsigned _techLevel;
    LocationType _locationType;
    unsigned _locationId;
    QMap<QString,QString> extraFields;
    QString _specialName;

    //static const QMap<unsigned,QString> specialCodes;
    static QMap<QString,QString> loadDescriptions(const QString& filename);
    static const QMap<QString,QString> specialTypesDescriptions;//type_name:lvl, description
    static const QMap<QString,QString> specialHullsDescriptions;//type_name:lvl, description
    static const QMap<QString,QString> artifactsDescriptions;//type,description
    static const QMap<QString,QString> micromodulesDescriptions;//name,description
};

std::unordered_set<unsigned> readEqlist(QTextStream &stream, Galaxy &galaxy, Equipment::LocationType locationType, unsigned locationId);

#endif // EQUIPMENT_H
