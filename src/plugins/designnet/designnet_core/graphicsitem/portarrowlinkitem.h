#ifndef PORTARROWLINKITEM_H
#define PORTARROWLINKITEM_H
#include "GraphicsUI/arrowlinkitem.h"
#include "../designnet_core_global.h"
#include "../designnetconstants.h"
namespace DesignNet{
class PortGraphicsItem;
class DESIGNNET_CORE_EXPORT PortArrowLinkItem : public GraphicsUI::ArrowLinkItem
{
    Q_OBJECT
public:
    enum {Type = UserType + PortArrowLinkItemType};
    int type() const;
    PortArrowLinkItem(PortGraphicsItem* sourcePort, QGraphicsItem *parent = 0);
    virtual ~PortArrowLinkItem();
    void setSourcePort(PortGraphicsItem* sourcePort);
    PortGraphicsItem* getSourcePort() const;
    void setTargetPort(PortGraphicsItem* targetPort);
    PortGraphicsItem* getTargetPort() const;

    virtual QPointF getStartPoint() const;
    virtual QPointF getEndPoint() const;

    virtual QPainterPath shape() const;
    bool detachFromSource();
    bool detachFromTarget();
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
signals:
    void arrowConnected(PortGraphicsItem* input, PortGraphicsItem* output);
    void portDetached();
protected:
	bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    PortGraphicsItem* m_sourcePort;
    PortGraphicsItem* m_targetPort;
    bool              m_bBeginDrag;
};
}

#endif // PORTARROWLINKITEM_H
