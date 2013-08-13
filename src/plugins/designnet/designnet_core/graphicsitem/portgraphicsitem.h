#ifndef PORTGRAPHICSITEM_H
#define PORTGRAPHICSITEM_H


#include "../designnet_core_global.h"
#include "../designnetconstants.h"
#include <QGraphicsObject>
#include <QList>
namespace GraphicsUI{
class GraphicsAutoShowHideItem;
}
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
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    enum{
		DEFAULT_PORT_WIDTH		= 16,
        DEFAULT_PORT_HEIGHT		= 16,
		DEFAULT_SPACING			= 5
	};
	enum TypeImageDirection{
		LEFT,
		RIGHT
	};
    enum { Type = UserType + PortGraphicsItemType };
    int type() const;
    explicit PortGraphicsItem(Port*port, ProcessorGraphicsBlock* processorBlock);
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
	void setSize(const QSizeF &size){m_size = size;}
	QSizeF size() const{return m_size;}
    ProcessorGraphicsBlock *processor();
    void removeAllConnection();//!< 断开连接

    DesignNetSpace* designNetSpace() const;
    void updateData();

	void showTypeImage(TypeImageDirection direction);
	void setTypeImageVisible(bool bVisible = true);
	void setMouseOver(bool bOver);

	void showTooltip(bool bShow = true);
protected slots:
    void processorSelectionChanged(bool value);
	void onDisconnectFromPort(Port *port);
protected:
    virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QPixmap m_imageConnected;
    QPixmap m_imageNotConnected;
    bool    m_bPressed;
	bool	m_bMouseOver;
	Port*   m_port;
    ToolTipGraphicsItem*		m_toolTipItem;
	GraphicsUI::GraphicsAutoShowHideItem*	m_typeImageItem;//!< 显示类型图像
	ProcessorGraphicsBlock*		m_processorBlock;
	QSizeF  m_size;

};
}

#endif // PORTGRAPHICSITEM_H
