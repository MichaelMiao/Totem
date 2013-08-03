#ifndef ARROWLINKITEM_H
#define ARROWLINKITEM_H

#include <QGraphicsPathItem>
#include "graphicsui_global.h"
namespace GraphicsUI{

class ArrowLinkControlItem;
class TOTEM_GRAPHICSUI_EXPORT ArrowLinkItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
	enum {
		SELECT_STATE	= 0x01,
		HOVER_STATE		= 0x01 << 1,
		NORMAL_STATE	= 0x00
	};
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

	virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );

	void setColor(const QColor &color, const int &state = NORMAL_STATE);
public slots:
    void controlItemPositionChanged();
protected:
    ArrowLinkControlItem* m_controlPoint_1; //!< 控制点1
    ArrowLinkControlItem* m_controlPoint_2; //!< 控制点2
    QPointF m_startPoint;               //!< 开始点坐标
    QPointF m_endPoint;                 //!< 结束点坐标
	bool	m_bHoverOver;				//!< 鼠标是否hover
	QColor	m_colorSelect;				//!< link颜色
	QColor	m_colorHover;				//!< hover颜色
	QColor	m_colorNormal;				//!< normal颜色
};
}

#endif // ARROWLINKITEM_H
