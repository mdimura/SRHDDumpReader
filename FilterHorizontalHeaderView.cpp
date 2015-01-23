#include "FilterHorizontalHeaderView.h"
#include <QScrollBar>
#include <cassert>

FilterHorizontalHeaderView::FilterHorizontalHeaderView(SortMultiFilterProxyModel *model, QTableView *parent):
    QHeaderView(Qt::Horizontal,parent)
{
    //setModel(parent->model());
    _model=model;

    setSectionsMovable(parent->horizontalHeader()->sectionsMovable());
    setSectionsClickable(parent->horizontalHeader()->sectionsClickable());

    insertColumns(0,model->columnCount()-1);

    updateWidgetPositions();

    connect(parent->horizontalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(updateWidgetPositions()) );
    connect(this,SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this,SLOT(setSortIndicator(int,Qt::SortOrder)));
    connect(this,SIGNAL(sectionResized(int,int,int)),
            this,SLOT(updateWidgetPositions()));

    connect(model,&QAbstractItemModel::columnsInserted,
            [=](const QModelIndex &parent,
            int logicalFirst, int logicalLast) {
        insertColumns(logicalFirst,logicalLast);
    });

    //TODO: add header data update
    //TODO: add sections removal
}

QSize FilterHorizontalHeaderView::sizeHint() const
{
    QSize size=QHeaderView::sizeHint();
    size.setHeight(_height);
    return size;
}

bool FilterHorizontalHeaderView::event(QEvent *event)
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

QSize FilterHorizontalHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    QVariant variant = model()->headerData(logicalIndex, Qt::Horizontal, Qt::SizeHintRole);
    if (variant.isValid())
        return qvariant_cast<QSize>(variant);
    // otherwise use the contents
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.section = logicalIndex;
    QVariant var = model()->headerData(logicalIndex, Qt::Horizontal,
                                       Qt::FontRole);
    QFont fnt;
    if (var.isValid() && var.canConvert<QFont>())
        fnt = qvariant_cast<QFont>(var);
    else
        fnt = font();
    fnt.setBold(true);
    opt.fontMetrics = QFontMetrics(fnt);
    opt.text = model()->headerData(logicalIndex, Qt::Horizontal,
                                   Qt::DisplayRole).toString();
    variant = model()->headerData(logicalIndex, Qt::Horizontal, Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);
    QSize size = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex) {
        int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);
        if (Qt::Horizontal == Qt::Horizontal)
            size.rwidth() += size.height() + margin;
        else
            size.rheight() += size.width() + margin;
    }
    return size;
}

void FilterHorizontalHeaderView::setSortIndicator(int col, const Qt::SortOrder &order)
{
    headerSortIndicators.value(lastSortSection)->setText(" ");
    QString c;
    switch (order)
    {
    case Qt::AscendingOrder:
        c="▲";
        break;
    case Qt::DescendingOrder:
        c="▼";
        break;
    default:
        c=" ";
        break;
    }
    headerSortIndicators.value(col)->setText(c);
    lastSortSection=col;
}

void FilterHorizontalHeaderView::updateWidgetPositions() const
{
    for (int i: headerWidgets.keys())
    {
        updateGeometry(i);
    }
}

void FilterHorizontalHeaderView::updateHeaderData(int first, int last)
{
    for(int i=first; i<=last; i++)
    {
        updateHeaderData(i);
    }
}

void FilterHorizontalHeaderView::updateGeometry(int logical) const
{
    assert(headerWidgets.contains(logical));
    headerWidgets[logical]->setGeometry(sectionViewportPosition(logical)+2, 0,
                                        sectionSize(logical)-5, height());
}

void FilterHorizontalHeaderView::updateHeaderData(int col)
{
    headerNames[col]->setText(_model->headerData(col,Qt::Horizontal).toString());
}

void FilterHorizontalHeaderView::insertColumns(int first, int last)
{
    for(int i=first; i<=last; i++)
    {
        assert(!headerWidgets.contains(i));
        makeWidget(i);
    }
}

void FilterHorizontalHeaderView::makeWidget(int col)
{
    WidgetType wt;
    wt=(WidgetType)_model->headerData(col,Qt::Horizontal,Qt::UserRole).toInt();
    QWidget* widget=new QWidget(this);
    headerWidgets[col]=widget;
    QVBoxLayout *vl=new QVBoxLayout(widget);
    QHBoxLayout *hl=new QHBoxLayout();

    widget->setCursor(Qt::ArrowCursor);
    QLabel *nameLbl=headerNames[col]=new QLabel(widget);
    QLabel *sortLbl=headerSortIndicators[col]=new QLabel(widget);
    sortLbl->setText(" ");

    nameLbl->setWordWrap(true);
    nameLbl->setAlignment(Qt::AlignCenter);
    sortLbl->setStyleSheet("font: small Courier");
    hl->setContentsMargins(0,0,0,0);
    vl->setContentsMargins(0,0,0,1);
    vl->setSpacing(1);
    hl->setSpacing(0);

    hl->addWidget(nameLbl,1,Qt::AlignHCenter);
    hl->addWidget(sortLbl,0,Qt::AlignRight);
    vl->addLayout(hl,0);
    QWidget *inputTop, *inputBottom;
    switch(wt)
    {
    case wtString:
    {
        auto editTop=new QLineEdit(widget);
        auto editBottom=new QLineEdit(widget);
        matchEdits[col]=editTop;
        notMatchEdits[col]=editBottom;
        editTop->setFrame(frame);
        editBottom->setFrame(frame);
        editTop->setToolTip("contains");
        editBottom->setToolTip("not contains");
        inputTop=editTop;
        inputBottom=editBottom;
        connect(editTop,&QLineEdit::editingFinished,[=](){
            _model->setMatch(col,editTop->text());
        });
        connect(editTop,&QLineEdit::textChanged,[=](const QString& str){
            _model->setMatchDelayed(col,str);
        });
        connect(editBottom,&QLineEdit::editingFinished,[=](){
            _model->setNotMatch(col,editBottom->text());
        });
        connect(editBottom,&QLineEdit::textChanged,[=](const QString& str){
            _model->setNotMatchDelayed(col,str);
        });

        break;
    }
    case wtInt:
    {
        auto editTop=new QSpinBox(widget);
        auto editBottom=new QSpinBox(widget);
        minIntEdits[col]=editTop;
        maxIntEdits[col]=editBottom;
        editTop->setToolTip("minimum value");
        editBottom->setToolTip("maximum value");
        editTop->setFrame(frame);
        editBottom->setFrame(frame);
        inputTop=editTop;
        inputBottom=editBottom;
        connect(editTop,&QSpinBox::editingFinished,[=](){
            _model->setMin(col,editTop->value());
        });
        connect(editTop,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int v){
            _model->setMinDelayed(col,v);
        });
        connect(editBottom,&QSpinBox::editingFinished,[=](){
            int max=editBottom->value();
            if(max==0) {
                _model->unsetMax(col);
            } else {
                _model->setMax(col,max);
            }
        });
        connect(editBottom,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int max){
            if(max==0) {
                _model->unsetMaxDelayed(col);
            } else {
                _model->setMaxDelayed(col,max);
            }
        });
        break;
    }
    case wtDouble:
    {
        auto editTop=new QDoubleSpinBox(widget);
        auto editBottom=new QDoubleSpinBox(widget);
        minDoubleEdits[col]=editTop;
        maxDoubleEdits[col]=editBottom;
        editTop->setToolTip("minimum value");
        editBottom->setToolTip("maximum value");
        editTop->setFrame(frame);
        editBottom->setFrame(frame);
        inputTop=editTop;
        inputBottom=editBottom;
        connect(editTop,&QDoubleSpinBox::editingFinished,[=](){
            _model->setMin(col,editTop->value());
        });
        connect(editTop,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [=](double v){
            _model->setMinDelayed(col,v);
        });
        connect(editBottom,&QDoubleSpinBox::editingFinished,[=](){
            double max=editBottom->value();
            if(max==0.0) {
                _model->unsetMax(col);
            } else {
                _model->setMax(col,max);
            }
        });
        connect(editBottom,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [=](double max){
            if(max==0) {
                _model->unsetMaxDelayed(col);
            } else {
                _model->setMaxDelayed(col,max);
            }
        });
        break;
    }
    case wtNone:
        inputTop=inputBottom=nullptr;
        break;
    }
    if(inputTop!=nullptr) {
        inputTop->setFixedHeight(inputHeight);
        inputBottom->setFixedHeight(inputHeight);
        vl->addWidget(inputTop,0);
        vl->addWidget(inputBottom,0);
    }
    widget->setLayout(vl);

    updateHeaderData(col);
    _height=std::max(_height,headerWidgets[col]->minimumSizeHint().height());
    setMinimumSectionSize(headerWidgets[col]->minimumSizeHint().width());
    setDefaultSectionSize(headerWidgets[col]->minimumSizeHint().width());
}

void FilterHorizontalHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!rect.isValid())
        return;
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.rect = rect;
    style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
    updateGeometry(logicalIndex);
    //widgets[logicalIndex]->show();
}
