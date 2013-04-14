#ifndef ARROWLINKITEM_H
#define ARROWLINKITEM_H

#include <QGraphicsObject>
#include "graphicsui_global.h"
namespace GraphicsUI{

class ArrowLinkControlItem;
class TOTEM_GRAPHICSUI_EXPORT ArrowLinkItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ArrowLinkItem(QGraphicsItem *parent = 0);
    virtual ~ArrowLinkItem();
    virtual QPainterPath shape() const;
    QPainterPath originalShape() const;
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    virtual void updateGeometory();             //!< 更新控制点的位置
    void setStartPoint(QPointF point);  //!< 设置开始点的位置
    virtual QPointF getStartPoint() const;      //!< getter 开始点位置

    void setEndPoint(QPointF point);    //!< 设置结束点的位置
    virtual QPointF getEndPoint() const;        //!< getter 结束点位置

    void setControlPointVisible(bool bVisible = true);

    QPointF getControlItemPosFirst() const;
    QPointF getControlItemPosSecond() const;
public slots:
    void controlItemPositionChanged();
protected:
    ArrowLinkControlItem* m_controlPoint_1; //!< 控制点1
    ArrowLinkControlItem* m_controlPoint_2; //!< 控制点2
    QPointF m_startPoint;               //!< 开始点坐标
    QPointF m_endPoint;                 //!< 结束点坐标

};
}

#endif // ARROWLINKITEM_H
