#include "FilterHorizontalHeaderView.h"
#include <QScrollBar>
#include <cassert>
#include <QJsonDocument>
#include <QFileDialog>

FilterHorizontalHeaderView::FilterHorizontalHeaderView(SortMultiFilterProxyModel *model, QTableView *parent):
    QHeaderView(Qt::Horizontal,parent)
{
    //setModel(parent->model());
    _model=model;

    setSectionsMovable(parent->horizontalHeader()->sectionsMovable());
    setSectionsClickable(parent->horizontalHeader()->sectionsClickable());

    insertColumns(0,model->columnCount()-1);

    updateWidgetPositions();

    contextMenu.addAction(&saveAct);
    contextMenu.addSeparator();
    connect(&saveAct,&QAction::triggered,this,&FilterHorizontalHeaderView::savePreset);

    connect(parent->horizontalScrollBar(),SIGNAL(valueChanged(int)),
            this,SLOT(updateWidgetPositions()) );
    connect(this,SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this,SLOT(setSortIndicator(int,Qt::SortOrder)));
    connect(this,SIGNAL(sectionResized(int,int,int)),
            this,SLOT(updateWidgetPositions()));

    connect(model,&QAbstractItemModel::columnsInserted,
            [=](const QModelIndex&,
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

QVariantMap FilterHorizontalHeaderView::loadPreset(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::information(0, tr("Unable to open file"),
                                 file.errorString()+"\n"+fileName);
        return QVariantMap();
    }
    QJsonDocument doc=QJsonDocument::fromJson(file.readAll());
    if(doc.isNull()) {
        QMessageBox::information(0, tr("Unable to parse JSON file"),
                                 tr("Could not open the file. "
                                    "There is a syntax error in the provided JSON file."));
        return QVariantMap();
    }
    return doc.toVariant().toMap();
}

void FilterHorizontalHeaderView::setPreset(const QVariantMap &p)
{
    QVariantMap matchFilters=p["match"].toMap();
    QVariantMap notMatchFilters=p["notMatch"].toMap();
    QVariantMap minInts=p["minInt"].toMap();
    QVariantMap maxInts=p["maxInt"].toMap();
    QVariantMap minDoubles=p["minDouble"].toMap();
    QVariantMap maxDoubles=p["maxDouble"].toMap();

    using VarMapCI=QVariantMap::const_iterator;
    for (VarMapCI i = matchFilters.begin(); i != matchFilters.end(); ++i)
    {
        matchEdits[i.key().toInt()]->setText(i.value().toString());
    }
    for (VarMapCI i = notMatchFilters.begin(); i != notMatchFilters.end(); ++i)
    {
        notMatchEdits[i.key().toInt()]->setText(i.value().toString());
    }
    for (VarMapCI i = minInts.begin(); i != minInts.end(); ++i)
    {
        minIntEdits[i.key().toInt()]->setValue(i.value().toInt());
    }
    for (VarMapCI i = maxInts.begin(); i != maxInts.end(); ++i)
    {
        maxIntEdits[i.key().toInt()]->setValue(i.value().toInt());
    }
    for (VarMapCI i = minDoubles.begin(); i != minDoubles.end(); ++i)
    {
        minDoubleEdits[i.key().toInt()]->setValue(i.value().toDouble());
    }
    for (VarMapCI i = maxDoubles.begin(); i != maxDoubles.end(); ++i)
    {
        maxDoubleEdits[i.key().toInt()]->setValue(i.value().toDouble());
    }
    setSortIndicator(p["sortColumn"].toInt(),(Qt::SortOrder)p["sortOrder"].toInt());
    _model->invalidate();
    _model->sort(p["sortColumn"].toInt(),(Qt::SortOrder)p["sortOrder"].toInt());
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
    /*if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex) {
        int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);
        if (Qt::Horizontal == Qt::Horizontal)
            size.rwidth() += size.height() + margin;
        else
            size.rheight() += size.width() + margin;
    }*/
    size.rwidth()=std::max(size.rwidth(),40);
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

void FilterHorizontalHeaderView::savePreset()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Filter"),"",tr("SRHDDR filter (*.dr.json);;Any file (*)"));
    if (fileName.isEmpty()) {
        return;
    }
    QVariantMap p=preset();
    addPreset(p,QFileInfo(fileName).baseName());
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(QJsonDocument::fromVariant(p).toJson());
}

void FilterHorizontalHeaderView::activatePreset(int i)
{
    if(i>=_presets.size()) {
        return;
    }
    setPreset(_presets[i]);
}

QVariantMap FilterHorizontalHeaderView::preset() const
{
    QVariantMap allFilters;

    using MapIntLineEditCI=QMap<int, QLineEdit*>::const_iterator;
    QVariantMap matchFilters;
    for (MapIntLineEditCI i = matchEdits.begin(); i != matchEdits.end(); ++i)
    {
        matchFilters.insert(QString::number(i.key()),i.value()->text());
    }
    allFilters.insert("match",std::move(matchFilters));

    QVariantMap notMatchFilters;
    for (MapIntLineEditCI i = notMatchEdits.begin(); i != notMatchEdits.end(); ++i)
    {
        notMatchFilters.insert(QString::number(i.key()),i.value()->text());
    }
    allFilters.insert("notMatch",std::move(notMatchFilters));

    using MapIntSpinBoxCI=QMap<int, QSpinBox*>::const_iterator;
    QVariantMap minInts;
    for (MapIntSpinBoxCI i = minIntEdits.begin(); i != minIntEdits.end(); ++i)
    {
        minInts.insert(QString::number(i.key()),i.value()->text());
    }
    allFilters.insert("minInt",std::move(minInts));

    QVariantMap maxInts;
    for (MapIntSpinBoxCI i = maxIntEdits.begin(); i != maxIntEdits.end(); ++i)
    {
        maxInts.insert(QString::number(i.key()),i.value()->text());
    }
    allFilters.insert("maxInt",std::move(maxInts));

    using MapIntDoubleSpinBoxCI=QMap<int, QDoubleSpinBox*>::const_iterator;
    QVariantMap minDoubles;
    for (MapIntDoubleSpinBoxCI i = minDoubleEdits.begin(); i != minDoubleEdits.end(); ++i)
    {
        minDoubles.insert(QString::number(i.key()),i.value()->text());
    }
    allFilters.insert("minDouble",std::move(minDoubles));

    QVariantMap maxDoubles;
    for (MapIntDoubleSpinBoxCI i = maxDoubleEdits.begin(); i != maxDoubleEdits.end(); ++i)
    {
        maxDoubles.insert(QString::number(i.key()),i.value()->text());
    }
    allFilters.insert("maxDouble",std::move(maxDoubles));

    allFilters.insert("sortColumn",_model->sortColumn());
    allFilters.insert("sortOrder",_model->sortOrder());

    return allFilters;
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
    widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(widget, &QWidget::customContextMenuRequested,[=](const QPoint& point){
        contextMenu.exec(widget->mapToGlobal(point));
    });
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
    QWidget *inputTop=nullptr, *inputBottom=nullptr;
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
        editTop->setMaximum(std::numeric_limits<int>::max());
        editBottom->setMaximum(std::numeric_limits<int>::max());
        editTop->setFrame(frame);
        editBottom->setFrame(frame);
        inputTop=editTop;
        inputBottom=editBottom;
        connect(editTop,&QSpinBox::editingFinished,[=]() {
            _model->setMin(col,editTop->value());
        });
        connect(editTop,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int v){
            _model->setMinDelayed(col,v);
        });
        connect(editBottom,&QSpinBox::editingFinished,[=]() {
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
        editTop->setMaximum(std::numeric_limits<double>::infinity());
        editBottom->setMaximum(std::numeric_limits<double>::infinity());
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
    //setMinimumSectionSize(headerWidgets[col]->minimumSizeHint().width());
    //setDefaultSectionSize(headerWidgets[col]->minimumSizeHint().width());

    //std::cout<<"col:"<<col<<" width:"<<headerWidgets[col]->minimumSizeHint().width()<<std::endl;
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
