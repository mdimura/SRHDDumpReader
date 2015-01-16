#include "MultiFilterProxyModel.h"

MultiFilterProxyModel::MultiFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),timer(this)
{
    timer.setInterval(300);
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(invalidate()));
}

QVariant MultiFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal && role==Qt::UserRole)
    {
        QVariant var;
        var.setValue(filterWidgets[section]);
        return var;
    }
    return QSortFilterProxyModel::headerData(section,orientation,role);
}

bool MultiFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    int colCnt=sourceModel()->columnCount();
    QVector<QModelIndex> indices;
    indices.reserve(colCnt);
    for(int col=0; col<colCnt; col++)
    {
        indices.push_back(sourceModel()->index(sourceRow, col, sourceParent));
    }

    for(unsigned col:minDoubleSpinBoxes.keys())
    {
        double cell=sourceModel()->data(indices.at(col)).toDouble();
        if(cell<minDoubleSpinBoxes.value(col)->value()){
            return false;
        }
    }
    for(unsigned col:maxDoubleSpinBoxes.keys())
    {
        double cell=sourceModel()->data(indices.at(col)).toDouble();
        double max=maxDoubleSpinBoxes.value(col)->value();
        if(max==0.0){
            continue;
        }
        if(cell>max){
            return false;
        }
    }

    for(unsigned col:minSpinBoxes.keys())
    {
        int cell=sourceModel()->data(indices.at(col)).toInt();
        if(cell<minSpinBoxes.value(col)->value()){
            return false;
        }
    }
    for(unsigned col:maxSpinBoxes.keys())
    {
        int cell=sourceModel()->data(indices.at(col)).toInt();
        int max=maxSpinBoxes.value(col)->value();
        if(max==0){
            continue;
        }
        if(cell>max){
            return false;
        }
    }

    for(unsigned col:regExpEdits.keys())
    {
        const QString& cell=sourceModel()->data(indices.at(col)).toString();
        if(! cell.contains(QRegularExpression(regExpEdits.value(col)->text(),QRegularExpression::CaseInsensitiveOption))){
            return false;
        }
    }

    return true;
}

void MultiFilterProxyModel::initialiseFilterWidgets()
{
    filterWidgets.resize(columnCount());

    for(int i:{0,1,3,6,7,8,10,12})
    {
        regExpEdits[i]=new QLineEdit();//RegExp filter
        connect(regExpEdits.value(i),SIGNAL(editingFinished()),this,SLOT(invalidate()));
        connect(regExpEdits.value(i),SIGNAL(textChanged(QString)),&timer,SLOT(start()));

        filterWidgets[i]=regExpEdits[i];
    }

    for(int i:{2,4,5})//Size,Cost,TL
    {
        auto& currentWidget=filterWidgets[i]=new QWidget();

        auto& currentMinBox=minSpinBoxes[i]=new QSpinBox(currentWidget);
        auto& currentMaxBox=maxSpinBoxes[i]=new QSpinBox(currentWidget);
        currentMinBox->setMaximum(std::numeric_limits<int>::max());
        currentMaxBox->setMaximum(std::numeric_limits<int>::max());

        connect(currentMinBox,SIGNAL(editingFinished()),this,SLOT(invalidate()));
        connect(currentMaxBox,SIGNAL(editingFinished()),this,SLOT(invalidate()));
        connect(currentMinBox,SIGNAL(valueChanged(int)),&timer,SLOT(start()));
        connect(currentMaxBox,SIGNAL(valueChanged(int)),&timer,SLOT(start()));

        QVBoxLayout *vl=new QVBoxLayout();
        vl->setContentsMargins(0,0,0,0);
        vl->addWidget(currentMinBox);//min
        vl->addWidget(currentMaxBox);//max
        currentWidget->setLayout(vl);
    }
    minSpinBoxes[5]->setMaximum(8);//TL
    maxSpinBoxes[5]->setMaximum(8);//TL
    for(int i:{9,11})//Distance, Durability
    {
        auto& currentWidget=filterWidgets[i]=new QWidget();//Durability min/max filter

        auto& currentMinBox=minDoubleSpinBoxes[i]=new QDoubleSpinBox(currentWidget);
        auto& currentMaxBox=maxDoubleSpinBoxes[i]=new QDoubleSpinBox(currentWidget);
        currentMinBox->setMaximum(std::numeric_limits<double>::max());//no Limit
        currentMaxBox->setMaximum(std::numeric_limits<double>::max());//no Limit

        connect(currentMinBox,SIGNAL(editingFinished()),this,SLOT(invalidate()));
        connect(currentMaxBox,SIGNAL(editingFinished()),this,SLOT(invalidate()));
        connect(currentMinBox,SIGNAL(valueChanged(double)),&timer,SLOT(start()));
        connect(currentMaxBox,SIGNAL(valueChanged(double)),&timer,SLOT(start()));

        QVBoxLayout *vl=new QVBoxLayout();
        vl->setContentsMargins(0,0,0,0);
        vl->addWidget(currentMinBox);//min
        vl->addWidget(currentMaxBox);//max
        currentWidget->setLayout(vl);
    }
    minDoubleSpinBoxes[11]->setMaximum(100.0);//Durability
    maxDoubleSpinBoxes[11]->setMaximum(100.0);//Durability
}
