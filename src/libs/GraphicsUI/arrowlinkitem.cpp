#include "arrowlinkitem.h"
#include "arrowlinkcontrolitem.h"
#include <QPainter>
#include <qmath.h>
#include <QStyleOptionGraphicsItem>
#include <QPainterPathStroker>
namespace GraphicsUI{

ArrowLinkItem::ArrowLinkItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
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
}

ArrowLinkItem::~ArrowLinkItem()
{

}

QPainterPath ArrowLinkItem::shape() const
{
    QPainterPath path = originalShape();
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
    path.cubicTo(c2, c1, getStartPoint());
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
        painter->setPen(Qt::green);
        painter->drawLine(c1, startPos);
        painter->drawLine(c2, endPos);
        penColor = Qt::red;
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


    QPainterPath path = originalShape();
    painter->drawPath(path);


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
    painter->setBrush(Qt::red);
    painter->drawPolygon(arrowHead);
}

void ArrowLinkItem::setStartPoint(QPointF point)
{
    m_startPoint = mapFromScene(point);
    updateGeometory();
}

QPointF ArrowLinkItem::getStartPoint() const
{
    return m_startPoint;
}

void ArrowLinkItem::setEndPoint(QPointF point)
{
    m_endPoint = mapFromScene(point);;
    updateGeometory();
}

QPointF ArrowLinkItem::getEndPoint() const
{
    return m_endPoint;
}

void ArrowLinkItem::setControlPointVisible(bool bVisible)
{
    prepareGeometryChange();
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
}
