#include "tooltipgraphicsitem.h"
#include "data/idata.h"
#include "idatawidget.h"
#include "data/datamanager.h"
#include "graphicsitem/portgraphicsitem.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"

#include "designnetbase/port.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <QToolButton>
#include <QGraphicsProxyWidget>
#include <QPalette>
#include <QPainter>
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsScene>
#include <QApplication>
#include <QDebug>
namespace DesignNet {
const float TITLE_HEIGHT  = 20;
const float MINIMUM_WIDTH = 100;
const float MINIMUM_HEIGHT = 100;

const QString buttonStyleSheet = "QToolButton{background-color:rgba(0,0,0,0);"
        "border: none;"
        "image: url(:/media/untopmost_normal.png);"
        "}"
        "QToolButton:hover{background-color:rgba(0,0,0,0);"
        "image: url(:/media/untopmost_hover.png);"
        "}"

        "QToolButton:checked:hover{background-color:rgba(0,0,0,0);"
        "    image: url(:/media/topmost_hover.png);"
        "}"

        "QToolButton:checked:!hover{background-color:rgba(0,0,0,0);"
        "    image: url(:/media/topmost_normal.png);"
        "}";

class ToolTipGraphicsItemPrivate
{
public:
    ToolTipGraphicsItemPrivate(ToolTipGraphicsItem *item);
    QGraphicsProxyWidget*                   m_anchorButton; //!< 钉子
    GraphicsUI::GraphicsAutoShowHideItem*   m_closeItem;    //!< 关闭
    QGraphicsTextItem *                     m_textItem;
    ToolTipGraphicsItem*                    m_toolTipItem;
    IData*                                  m_data;         //!< 数据
    IDataWidget*                            m_customWidget; //!< 数据相关预览窗口
};

ToolTipGraphicsItemPrivate::ToolTipGraphicsItemPrivate(ToolTipGraphicsItem* item)
    : m_toolTipItem(item)
{
    m_closeItem = new GraphicsUI::GraphicsAutoShowHideItem(item);
    m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
    m_closeItem->setSize(QSize(16, 16));
    m_closeItem->animateShow(true);
    m_textItem  = new QGraphicsTextItem(item);
    QToolButton* toolButton = new QToolButton;
    toolButton->setCheckable(true);
    toolButton->setFixedSize(16, 16);
    QObject::connect(toolButton, SIGNAL(clicked(bool)), item, SLOT(onTopMost(bool)));
    m_anchorButton = new QGraphicsProxyWidget(item);
    m_anchorButton->setWidget(toolButton);
    toolButton->setStyleSheet(buttonStyleSheet);
    m_customWidget = 0;
}


ToolTipGraphicsItem::ToolTipGraphicsItem(PortGraphicsItem *parent)
    : QGraphicsObject(parent),
      d(new ToolTipGraphicsItemPrivate(this))
{
    connect(d->m_closeItem, SIGNAL(clicked()), this, SLOT(onClosed()));
	connect(this, SIGNAL(visibleChanged()), this, SLOT(onVisibleChanged()));
	setData(parent->getPort()->data());
	setAcceptHoverEvents(true);
	setFlags(ItemIsSelectable);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

void ToolTipGraphicsItem::setText(const QString &text)
{
    d->m_textItem->setHtml(text);
}

QRectF ToolTipGraphicsItem::boundingRect() const
{
    QRectF rectF(0, 0, 0, 0);
    QRectF customRect;
    if(d->m_customWidget)
    {
        customRect = d->m_customWidget->boundingRect();
    }
    else
        customRect = QRectF(0, 0, IDataWidget::DEFAULT_WIDTH, IDataWidget::DEFAULT_HEIGHT);
    QRectF rectText = d->m_textItem->boundingRect();
    float width = qMax((float)customRect.width(), MINIMUM_WIDTH);
    width = qMax(width, (float)rectText.width()) + 10;
    rectF.setWidth(width);
    float height = customRect.height() + 10 + rectText.bottom();
    height = qMax((float)height, MINIMUM_HEIGHT) + 20;
    rectF.setHeight(height);
    return rectF;
}

void ToolTipGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *item,
                                QWidget *w)
{
    relayout();
    QPalette palette =  QApplication::palette();
    painter->save();
    painter->setBrush(palette.toolTipBase());
    painter->drawRoundedRect(boundingRect(), 2, 2);
    painter->restore();
}

QVariant ToolTipGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemVisibleChange)
    {
        QToolButton* toolButton = qobject_cast<QToolButton*>(d->m_anchorButton->widget());
        if(toolButton)
        {
            toolButton->setChecked(false);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

bool ToolTipGraphicsItem::topmost() const
{
    QToolButton* toolButton = qobject_cast<QToolButton*>(d->m_anchorButton->widget());
    if(toolButton)
    {
        return toolButton->isChecked();
    }
    return false;
}

void ToolTipGraphicsItem::setData(IData *data)
{
	prepareGeometryChange();
    if(d->m_customWidget && d->m_data != data)
    {
        delete d->m_customWidget;
        d->m_customWidget = 0;
    }
    d->m_data = data;
    if(d->m_data)
    {
        d->m_customWidget = DataManager::instance()->createWidget(data, this);
        d->m_customWidget->setVisible(true);
    }
    update();
}

void ToolTipGraphicsItem::relayout()
{
	prepareGeometryChange();
    QRectF rect = boundingRect();
	qDebug() << "relayout";
    QSizeF closeSize = d->m_closeItem->boundingRect().size();
    d->m_closeItem->setPos(QPointF(rect.right() - closeSize.width(), rect.top()));
    d->m_anchorButton->setPos(QPointF(rect.right() - closeSize.width() - d->m_anchorButton->boundingRect().width(),
                                      rect.top()));
    d->m_textItem->setPos(QPointF(rect.left(), TITLE_HEIGHT));
    if(d->m_customWidget)
    {

        d->m_customWidget->setPos(QPointF(rect.left() + 5,
                              TITLE_HEIGHT + d->m_textItem->boundingRect().height()));
		d->m_customWidget->setVisible(true);
    }
	scene()->update();
}

void ToolTipGraphicsItem::onClosed()
{
    this->setVisible(false);
}

void ToolTipGraphicsItem::onTopMost(bool topmost)
{
    if(topmost)
    {
		this->setZValue(DesignNet::Constants::ZValue_Tooltip);
    }
    else
    {
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Normal);
    }
}

void ToolTipGraphicsItem::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	event->accept();
}

void ToolTipGraphicsItem::onVisibleChanged()
{
	if(this->isVisible())
	{
		if(d->m_customWidget)
		{
			connect(d->m_customWidget, SIGNAL(geometryChanged()), this, SLOT(relayout()));
			d->m_customWidget->setVisible(true);
		}
	}
	else
	{
		if(d->m_customWidget)
		{
			disconnect(d->m_customWidget, SIGNAL(geometryChanged()), this, SLOT(relayout()));
			d->m_customWidget->setVisible(false);
		}
	}
}

}
