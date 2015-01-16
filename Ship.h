#ifndef SHIP_H
#define SHIP_H
#include "GoodsArr.h"
#include <QString>
#include <QTextStream>
#include <unordered_set>
#include <unordered_map>
class Galaxy;

class Ship
{
public:
    Ship(QTextStream &stream, Galaxy& galaxy, unsigned id, unsigned starId);
    unsigned id() const
    {
        return _id;
    }
    bool hasMarket() const
    {
        return !(_goodsSale.empty());
    }
    QString name() const
    {
        return _fullName;
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

private:
    void compactifyName();
private:
    unsigned _id;
    QString _type;
    QString _fullName;
    GoodsArr _goodsQuantity;
    GoodsArr _goodsShopQuantity, _goodsSale, _goodsBuy;
    unsigned _relation;
    unsigned _money;
    std::unordered_set<unsigned> _eqIdList;
    //QString _place;
    //unsigned _placeId;
    unsigned _starId;
    unsigned _special;

    //enum Akrin { kNone, kMyoplasmic, kPygamore, kTranscendental, kUpgraded, kBiogenic, kNanobrolite, kUltraalloy, kHybrid};
};

void readShiplist(QTextStream &stream, Galaxy &galaxy, unsigned starId);

#endif // SHIP_H
