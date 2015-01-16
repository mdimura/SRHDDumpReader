#include "TradeTableModel.h"
#include "HierarchicalHeaderView.h"
TradeTableModel::TradeTableModel(const Galaxy *galaxy, QObject *parent) :
    QAbstractTableModel(parent),_galaxy(galaxy)
{
    fillHeaderModel();
}

int TradeTableModel::rowCount(const QModelIndex &parent) const
{
    return _galaxy->marketsCount();
}

int TradeTableModel::columnCount(const QModelIndex &parent) const
{
    return 31;//3+8*3+4
}

QVariant TradeTableModel::data(const QModelIndex &index, int role) const
{
    if(role==HierarchicalHeaderView::HorizontalHeaderDataRole)
    {
        QVariant v;
        v.setValue((QObject*)&_horizontalHeaderModel);
        return v;
    }
    if (role == Qt::DisplayRole)
    {
        int col=index.column();
        if(col==0)
        {
            return _galaxy->marketName(index.row());
        }
        else if(col==1)
        {
            return _galaxy->marketStarName(index.row());
        }
        else if(col==2)
        {
            return std::round(_galaxy->marketDistFromPlayer(index.row())*10.0)/10.0;
        }
        else if(col==27)
        {
            return _galaxy->marketPlanetSize(index.row());
        }
        else if(col==28)
        {
            return _galaxy->marketPlanetTechLevel(index.row());
        }
        else if(col==29)
        {
            return _galaxy->marketPlanetEconomy(index.row());
        }
        else if(col==30)
        {
            return _galaxy->marketPlanetOwner(index.row());
        }
        else
        {
            int quantSellBuy=(col-3)%3;
            int goodNum=(col-3)/3;
            switch(quantSellBuy)
            {
            case 0://quantity is requested
                return _galaxy->marketQuantity(index.row())[goodNum];
                break;

            case 1://sale price is requested
                return _galaxy->marketSale(index.row())[goodNum];
                break;

            case 2://buy price is requested
                return _galaxy->marketBuy(index.row())[goodNum];
                break;

            }
        }

        return index.row()+index.column();
    }
    if (role == Qt::BackgroundColorRole)
    {
        int col=index.column();
        if(col>2 && col<27)
        {
            int quantSellBuy=(col-3)%3;
            if(quantSellBuy==0){
                return QVariant();
            }
            int goodNum=(col-3)/3;
            int maxBuyPrice=_galaxy->maxBuyPrice()[goodNum];
            int minSellPrice=_galaxy->minSellPrice()[goodNum];
            int quantity=_galaxy->marketQuantity(index.row())[goodNum];
            int scale=-1;
            if(quantSellBuy==1) {
                int price=_galaxy->marketSale(index.row())[goodNum];
                if(price>maxBuyPrice || quantity==0) {
                    return QVariant();
                }
                scale=255+255.0*(minSellPrice-price)/(maxBuyPrice-minSellPrice);
                return QColor(0, 163, 22,std::min(scale,255));
            }
            else if(quantSellBuy==2) {
                int price=_galaxy->marketBuy(index.row())[goodNum];
                if(price<minSellPrice) {
                    return QVariant();
                }
                scale=255.0*(price-minSellPrice)/(maxBuyPrice-minSellPrice);
                return QColor(0, 116, 224,std::min(scale,255));
            }
        }

        return QVariant();
    }
    return QVariant();
}

QVariant TradeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> header={tr("Name"),tr("Star"),tr("Dist."),
                                          tr("Fq"), tr("Fs"), tr("Fb"),
                                          tr("Mq"), tr("Ms"), tr("Mb"),
                                          tr("Aq"), tr("As"), tr("Ab"),
                                          tr("Sq"), tr("Ss"), tr("Sb"),
                                          tr("Lq"), tr("Ls"), tr("Lb"),
                                          tr("Tq"), tr("Ts"), tr("Tb"),
                                          tr("Wq"), tr("Ws"), tr("Wb"),
                                          tr("Dq"), tr("Ds"), tr("Db"),
                                          tr("Size"),tr("Tech"),tr("Type"),tr("Owner")};
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return _galaxy->marketId(section);
        }
        else if (orientation == Qt::Horizontal) {
            return header.at(section);
        }
    }
    return QVariant();
}

void TradeTableModel::fillHeaderModel()
{
    _horizontalHeaderModel.setItem(0, 0, new QStandardItem(tr("Name")));
    _horizontalHeaderModel.setItem(0, 1, new QStandardItem(tr("Star")));
    _horizontalHeaderModel.setItem(0, 2, new QStandardItem(tr("Dist.")));

    static const QVector<QString> products={tr("Food"),tr("Meds"),tr("Alcohol"),tr("Minerals"),
                                            tr("Luxury"),tr("Technics"),tr("Weapons"),tr("Drugs")};

    for(int prod=0; prod<8; prod++)
    {
        QStandardItem* rootItem = new QStandardItem(products.at(prod));
        QList<QStandardItem*> l;

        l.push_back(new QStandardItem("#"));
        rootItem->appendColumn(l);
        l.clear();

        l.push_back(new QStandardItem("S"));
        rootItem->appendColumn(l);
        l.clear();

        l.push_back(new QStandardItem("B"));
        rootItem->appendColumn(l);
        l.clear();

        _horizontalHeaderModel.setItem(0, prod+3, rootItem);
    }
}
