#include "arrowlinkitem.h"
#include "arrowlinkcontrolitem.h"
#include <QPainter>
#include <qmath.h>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>
#include <QGraphicsItem>
#include "ArrowLinkControlItem.h"

namespace GraphicsUI{

ArrowLinkItem::ArrowLinkItem(QGraphicsItem *parent) :
    QGraphicsPathItem(parent), m_arrowLinkEndPoint(this)
{
    m_controlPoint_1 = new ArrowLinkControlItem(this);

    m_controlPoint_2 = new ArrowLinkControlItem(this);
    connect(m_controlPoint_1, SIGNAL(positionChanged()),
            this, SLOT(controlItemPositionChanged()));
    connect(m_controlPoint_2, SIGNAL(positionChanged()),
            this, SLOT(controlItemPositionChanged()));

    setAcceptHoverEvents(true);
    setFlags(ItemIsSelectable);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	m_colorHover = m_colorSelect = Qt::red;
	m_colorNormal= Qt::green;
}

ArrowLinkItem::~ArrowLinkItem()
{
	if (m_controlPoint_1->scene())
		m_controlPoint_1->scene()->removeItem(m_controlPoint_1);
	if (m_controlPoint_2->scene())
		m_controlPoint_2->scene()->removeItem(m_controlPoint_2);
}

QPainterPath ArrowLinkItem::shape() const
{
    QPainterPath path = m_path;
    QPainterPathStroker pathStroker;
    pathStroker.setWidth(10);
    return pathStroker.createStroke(path);
}

QPainterPath ArrowLinkItem::originalShape() const
{
    QPointF c1 = getControlItemPosFirst();
    QPointF c2 = getControlItemPosSecond();
    QPainterPath path(getStartPoint());
    path.cubicTo(c1, c2, getEndPoint());
    return path;
}

QRectF ArrowLinkItem::boundingRect() const
{
    qreal extra = 10;
    QRectF rect = shape().boundingRect();
    QPointF c1 = getControlItemPosFirst();
    QPointF c2 = getControlItemPosSecond();
    qreal minX = qMin(c1.x(), c2.x());
    qreal minY = qMin(c1.y(), c2.y());
    qreal maxX = qMax(c1.x(), c2.x());
    qreal maxY = qMax(c1.y(), c2.y());
    rect.setLeft(qMin(minX, rect.left()));
    rect.setTop(qMin(minY, rect.top()));
    rect.setRight(qMax(maxX, rect.right()));
    rect.setBottom(qMax(maxY, rect.bottom()));
    rect = rect.adjusted(-extra, -extra, extra, extra);
    return rect;
}

void ArrowLinkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor penColor;
    QPointF c1 = getControlItemPosFirst();
    QPointF c2 = getControlItemPosSecond();
    QPointF startPos = getStartPoint();
    QPointF endPos = getEndPoint();

    if(option->state & QStyle::State_Selected)
    {
        setControlPointVisible(true);
		QPen pen(Qt::DotLine);
		pen.setWidth(1);
		pen.setColor(Qt::darkYellow);
		painter->save();
        painter->setPen(pen);
        painter->drawLine(c1, startPos);
        painter->drawLine(c2, endPos);
		painter->restore();
		penColor = m_colorSelect;
    }
    else
    {
        setControlPointVisible(false);
        penColor = Qt::blue;
    }
    if(option->state & QStyle::State_MouseOver)
    {
        penColor = penColor.lighter();
    }
    if(option->state & QStyle::State_Sunken)
    {
        penColor.setAlpha(210);
    }
    painter->setPen(QPen(penColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(m_path);

	qreal arrowSize = 10;
    QLineF line(endPos, c2);
    double angle = ::acos(line.dx() / (line.length() <= 0 ? 1 : line.length()));
    if (line.dy() >= 0)
        angle = (M_PI * 2) - angle;

    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                            cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                            cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead << arrowP2 << arrowP1 << line.p1();
    painter->setBrush(penColor);
    painter->drawPolygon(arrowHead);
}

void ArrowLinkItem::setStartPoint(QPointF point)
{
    setPoint_Internal(point, true);
    updateGeometory();
	
}

QPointF ArrowLinkItem::getStartPoint() const
{
    return m_startPoint;
}

void ArrowLinkItem::setEndPoint(QPointF point)
{
    setPoint_Internal(point, false);
    updateGeometory();
}

QPointF ArrowLinkItem::getEndPoint() const
{
    return m_endPoint;
}

void ArrowLinkItem::setControlPointPos(QPointF point, bool bSrc /*= true*/)
{
	QGraphicsItem *pItem = bSrc ? m_controlPoint_1 : m_controlPoint_2;
	QGraphicsItem* p = pItem->parentItem();
	if (p)
		pItem->setPos(p->mapFromScene(point));
	else
		pItem->setPos(point);
}

void ArrowLinkItem::setControlPointVisible(bool bVisible)
{
    m_controlPoint_1->setVisible(bVisible);
    m_controlPoint_2->setVisible(bVisible);
}

QPointF ArrowLinkItem::getControlItemPosFirst() const
{
    return mapFromItem(m_controlPoint_1, 0, 0);
}

QPointF ArrowLinkItem::getControlItemPosSecond() const
{
    return mapFromItem(m_controlPoint_2, 0, 0);
}

void ArrowLinkItem::controlItemPositionChanged()
{
	prepareGeometryChange();
	onControlItemPostionChanged();
	m_path = originalShape();
	update();
}

void ArrowLinkItem::updateGeometory()
{
    prepareGeometryChange();
    QPointF c1;
    QPointF c2;
    QPointF startPoint = getStartPoint();
    QPointF endPoint = getEndPoint();
	int controlDeltaWidth = qAbs(startPoint.x() - endPoint.x()) / 2;
	controlDeltaWidth = qMax(controlDeltaWidth, 50);
	c1.setY(startPoint.y());
	c2.setY(endPoint.y());

	c1.setX(startPoint.x() + controlDeltaWidth);
	c2.setX(endPoint.x() - controlDeltaWidth);
    m_controlPoint_1->setPos(mapToItem(m_controlPoint_1->parentItem(), c1));
    m_controlPoint_2->setPos(mapToItem(m_controlPoint_2->parentItem(), c2));

    update();
}

void ArrowLinkItem::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	m_bHoverOver = true;
	m_arrowLinkEndPoint.setHover(true);
	update();
}

void ArrowLinkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	m_bHoverOver = false;
	m_arrowLinkEndPoint.setHover(false);
	update();
}

void ArrowLinkItem::setColor( const QColor &color, const int &state /*= NORMAL_STATE*/ )
{
	switch (state)
	{
	case SELECT_STATE:
		m_colorSelect = color;
		break;
	case HOVER_STATE:
		m_colorHover = color;
		break;
	case NORMAL_STATE:
		m_colorNormal = color;
		break;
	}
	update();
}

void ArrowLinkItem::setPoint_Internal( const QPointF &pf, bool bStartPoint /*= false*/ )
{
	if (bStartPoint)
	{
		m_startPoint = mapFromScene(pf);
		QPointF p = m_controlPoint_1->scenePos();
		m_arrowLinkEndPoint.setPos(m_startPoint);
	}
	else
	{
		m_endPoint = mapFromScene(pf);
	}
}

QVariant ArrowLinkItem::itemChange(GraphicsItemChange change, const QVariant &v)
{
	if (change == QGraphicsItem::ItemSceneHasChanged && !scene())
	{
		if (m_controlPoint_1->scene())
			m_controlPoint_1->scene()->removeItem(m_controlPoint_1);
		if (m_controlPoint_2->scene())
			m_controlPoint_2->scene()->removeItem(m_controlPoint_2);
		m_controlPoint_1->setParentItem(NULL);
		m_controlPoint_2->setParentItem(NULL);
	}
	else if (change == QGraphicsItem::ItemVisibleChange)
	{
		m_controlPoint_1->setVisible(v.toBool());
		m_controlPoint_2->setVisible(v.toBool());
	}
	return v;
}

QPointF ArrowLinkItem::getControlItemScenePosFirst() const
{
	QPointF p = m_controlPoint_1->scenePos();
	return m_controlPoint_1->scenePos();
}

QPointF ArrowLinkItem::getControlItemScenePosSecond() const
{
	return m_controlPoint_2->scenePos();
}

}
