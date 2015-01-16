#ifndef WIDGETHEADERVIEW_H
#define WIDGETHEADERVIEW_H

#include <QHeaderView>
#include <QTableView>
#include <QMap>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QCoreApplication>

class HeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderWidget(QWidget *inner,QWidget *parent):QWidget(parent)
    {
        setCursor(Qt::ArrowCursor);

        nameLbl=new QLabel("",this);
        nameLbl->setWordWrap(true);
        nameLbl->setAlignment(Qt::AlignCenter);
        sortLbl=new QLabel(" ",this);//▲▼▶◆◊⧫◈◉●
        sortLbl->setStyleSheet("font: small Courier");

        QVBoxLayout *vl = new QVBoxLayout();
        QHBoxLayout *hl = new QHBoxLayout();

        hl->setContentsMargins(0,0,0,0);
        vl->setContentsMargins(0,1,0,2);

        hl->addStretch(1);
        hl->addWidget(nameLbl,0,Qt::AlignHCenter);
        hl->addWidget(sortLbl,0,Qt::AlignHCenter);
        hl->addStretch(1);
        vl->addLayout(hl,0);
        if(inner)
        {
            vl->addWidget(inner,1);
        }

        setLayout(vl);
    }
    void setSorting(const Qt::SortOrder &order)
    {
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
        sortLbl->setText(c);
    }
    void unsetSorting()
    {
        sortLbl->setText(" ");
    }
    void setHeader(const QString& str)
    {
        nameLbl->setText(str);
    }

protected:
    void mousePressEvent(QMouseEvent * event)
    {
        event->ignore();
    }
    void mouseReleaseEvent(QMouseEvent * event)
    {
        event->ignore();
    }

private:
    //int _section;
    QLabel *sortLbl, *nameLbl;
};

class WidgetHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit WidgetHeaderView(Qt::Orientation orientation, QTableView *parent = 0);
    QSize sizeHint() const;
protected:

    bool event( QEvent *event );
signals:

private:
    void insertWidgets(int logicalFirst, int logicalLast);
    void updateGeometry(int logical) const;
    HeaderWidget* makeWidget(int section);

private slots:
    void setSortIndicator(int logicalIndex, Qt::SortOrder order);
    void updateHeaderCaption(int section);
    void updateWidgetPositions() const
    {
        for (int i: widgets.keys())
        {
            updateGeometry(i);
        }
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
    QMap<int, HeaderWidget *> widgets;
    int lastSortSection;
    int _height;
};

#endif // WIDGETHEADERVIEW_H
