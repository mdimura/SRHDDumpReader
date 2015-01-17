#ifndef GALAXY_H
#define GALAXY_H
#include "Equipment.h"
#include "Ship.h"
#include "Star.h"
#include "Planet.h"
#include "BlackHole.h"
#include "Galaxy.h"
#include <iostream>
#include <unordered_map>

class Galaxy
{
public:
    explicit Galaxy();
    void parseDump(QTextStream& stream);
    void clear();

    unsigned shipCount() const;
    unsigned equipmentCount() const;
    unsigned starCount() const;
    unsigned blackHoleCount() const;
    unsigned planetCount() const;

    void addEquipment(Equipment&& eq);
    void addShip(const Ship&& ship);
    void addStar(const Star&& star);
    void addBlackHole(const BlackHole&& bh);
    void addPlanet(const Planet&& planet);

    unsigned marketsCount() const;
    QString marketName(unsigned row) const;
    QString marketPlanetEconomy(unsigned row) const;
    QString marketPlanetOwner(unsigned row) const;
    unsigned marketPlanetSize(unsigned row) const;
    unsigned marketPlanetTechLevel(unsigned row) const;
    const GoodsArr& marketQuantity(unsigned row) const;
    const GoodsArr& marketSale(unsigned row) const;
    const GoodsArr& marketBuy(unsigned row) const;
    unsigned marketId(unsigned row) const;
    double marketDistFromPlayer(unsigned row) const;
    QString marketStarName(unsigned row) const;

    unsigned equipmentId(unsigned row) const;
    QString equipmentName(unsigned row) const;
    QString equipmentType(unsigned row) const;
    unsigned equipmentSize(unsigned row) const;
    QString equipmentOwner(unsigned row) const;
    unsigned equipmentCost(unsigned row) const;
    unsigned equipmentTechLevel(unsigned row) const;
    QString equipmentLocationType(unsigned row) const;
    QString equipmentLocationName(unsigned row) const;
    QString equipmentStarName(unsigned row) const;
    double equipmentDistFromPlayer(unsigned row) const;
    QString equipmentStarOwner(unsigned row) const;
    double equipmentDurability(unsigned row) const;
    QString equipmentBonus(unsigned row) const;

    unsigned blackHoleId(unsigned row) const;
    QString blackHoleStar1(unsigned row) const;
    float blackHoleStar1Distance(unsigned row) const;
    QString blackHoleStar2(unsigned row) const;
    float blackHoleStar2Distance(unsigned row) const;
    unsigned blackHoleTurnsToClose(unsigned row) const;

    const GoodsArr& maxBuyPrice() const
    {
        return _maxBuyPrice;
    }
    const GoodsArr& minSellPrice() const
    {
        return _minSellPrice;
    }

private:
    unsigned marketStarId(unsigned row) const;
    unsigned equipmentStarId(unsigned row) const;
private:
    std::unordered_map<unsigned,Equipment> eqMap;
    std::unordered_map<unsigned,Ship> shipMap;
    std::unordered_map<unsigned,Star> starMap;
    std::unordered_map<unsigned,Planet> planetMap;
    std::vector<BlackHole> blackHoles;
    std::vector<unsigned> planetMarkets;
    std::vector<unsigned> shipMarkets;
    std::vector<unsigned> eqVec;

    mutable GoodsArr _maxBuyPrice;
    mutable GoodsArr _minSellPrice;
};





#endif // GALAXY_H