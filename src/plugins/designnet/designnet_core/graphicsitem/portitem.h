#pragma once
#include "../designnet_core_global.h"
#include "../designnetconstants.h"
#include "graphicsui/graphicstoolbutton.h"


namespace DesignNet{

class Port;
class ProcessorGraphicsBlock;
class ToolTipGraphicsItem;
class DESIGNNET_CORE_EXPORT PortItem : public GraphicsUI::GraphicsToolButton
{
	Q_OBJECT

public:

	TYPE(PortItem, GraphicsItemType_Port);

	PortItem(Port* pPort, ProcessorGraphicsBlock* pBlock);
	virtual ~PortItem();
	
	Port* port() { return m_pPort; }
	int processorId();
	int index();

	QImage getImage();

	//////////////////////////////////////////////////////////////////////////

	QRectF  boundingRect() const override;
	void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;

	void mousePressEvent(QGraphicsSceneMouseEvent * event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected:

	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

public slots:

	void onPortConnected(Port* src, Port* target);
	void onPortDisconnected(Port* src, Port* target);

	void onPortDataChanged();

private:

	Port* m_pPort;							//!< ¶Ë¿Ú
	ProcessorGraphicsBlock* m_pBlock;		//!< Block
	ToolTipGraphicsItem*	m_pToolTip;
};

}
