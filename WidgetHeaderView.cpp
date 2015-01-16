#include "WidgetHeaderView.h"
#include <QScrollBar>
WidgetHeaderView::WidgetHeaderView(Qt::Orientation orientation,QTableView *parent) :
    QHeaderView(orientation, parent),lastSortSection(0),_height(0)
{
    setModel(parent->model());

    connect(parent->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(updateWidgetPositions()) );

    setSectionsMovable(parent->horizontalHeader()->sectionsMovable());
    setSectionsClickable(parent->horizontalHeader()->sectionsClickable());

    insertWidgets(0,parent->model()->columnCount());
    setMinimumSectionSize(widgets[0]->minimumSizeHint().width());
    setDefaultSectionSize(widgets[0]->minimumSizeHint().width());

    connect(this,SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(setSortIndicator(int,Qt::SortOrder)));
}

QSize WidgetHeaderView::sizeHint() const
{
    QSize size=QHeaderView::sizeHint();
    size.setHeight(_height);
    return size;
}

bool WidgetHeaderView::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress )
    {
        mousePressEvent(static_cast<QMouseEvent *>(event));
        return false;
    }
    if(event->type() == QEvent::MouseButtonRelease )
    {
        mouseReleaseEvent(static_cast<QMouseEvent *>(event));
        return false;
    }
    return QHeaderView::event(event);
}

void WidgetHeaderView::insertWidgets(int logicalFirst, int logicalLast)
{
    for(int i=logicalFirst; i<logicalLast; i++)
    {
        if(!widgets[i]){
            widgets[i]=makeWidget(i);
            _height=std::max(_height,widgets[i]->minimumSizeHint().height());
            updateHeaderCaption(i);
        }
    }
}

void WidgetHeaderView::updateGeometry(int logical) const
{
    widgets[logical]->setGeometry(sectionViewportPosition(logical)+2, 0,
                                  sectionSize(logical)-5, height());
}

HeaderWidget *WidgetHeaderView::makeWidget(int section)
{
    QWidget* inner=model()->headerData(section,Qt::Horizontal,Qt::UserRole).value<QWidget*>();
    return new HeaderWidget(inner,this);
}

void WidgetHeaderView::setSortIndicator(int logicalIndex, Qt::SortOrder order)
{
    widgets[lastSortSection]->unsetSorting();
    lastSortSection=logicalIndex;
    widgets[logicalIndex]->setSorting(order);
}

void WidgetHeaderView::updateHeaderCaption(int section)
{
    widgets[section]->setHeader(model()->headerData(section,Qt::Horizontal).toString());
}

void WidgetHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const {
    if (!rect.isValid())
        return;
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.rect = rect;
    style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
    updateGeometry(logicalIndex);
    //widgets[logicalIndex]->show();
}
