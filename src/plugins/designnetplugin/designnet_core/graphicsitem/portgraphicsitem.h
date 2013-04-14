#ifndef PORTGRAPHICSITEM_H
#define PORTGRAPHICSITEM_H


#include "../designnet_core_global.h"
#include "../designnetconstants.h"
#include <QGraphicsObject>
#include <QList>

namespace DesignNet{
class Port;
class PortArrowLinkItem;
class Processor;
class ProcessorGraphicsBlock;
class ToolTipGraphicsItem;
class DesignNetSpace;
class DESIGNNET_CORE_EXPORT PortGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
    friend class PortArrowLinkItem;
public:
    enum{DEFAULT_WIDTH = 16,
        DEFAULT_HEIGHT = 16};
    enum { Type = UserType + PortGraphicsItemType };
    int type() const;
    explicit PortGraphicsItem(Port*port, QGraphicsItem *parent = 0);
    virtual ~PortGraphicsItem();
    virtual QRectF boundingRect() const;

    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget * widget = 0);
    /**
     * @brief getter/setter Port
     * @return
     */
    Port* getPort() const   {return m_port;}
    void setPort(Port* port){m_port = port;}

    void setSize(const QSize &size){m_size = size;}
    QSize size() const{return m_size;}
    void setMovingArrow(PortArrowLinkItem* item);
    ProcessorGraphicsBlock *processor();
    void removeAllConnection();//!< 断开连接

    DesignNetSpace* designNetSpace() const;
    void updateData();
protected slots:
    void processorSelectionChanged(bool value);
protected:
    virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QPixmap m_imageConnected;
    QPixmap m_imageNotConnected;
    bool    m_bPressed;
    QSize   m_size;
    Port*   m_port;
    PortArrowLinkItem*      m_movingArrow;
    ToolTipGraphicsItem*    m_toolTipItem;
};
}

#endif // PORTGRAPHICSITEM_H
