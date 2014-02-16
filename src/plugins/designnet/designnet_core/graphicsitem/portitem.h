#pragma once
#include "../designnet_core_global.h"
#include "../designnetconstants.h"
#include <QGraphicsObject>


namespace DesignNet{

class Port;
class ProcessorGraphicsBlock;
class DESIGNNET_CORE_EXPORT PortItem : public QGraphicsObject
{
	Q_OBJECT

public:

	TYPE(PortItem, GraphicsItemType_Port);

	PortItem(Port* pPort, ProcessorGraphicsBlock* pBlock);
	virtual ~PortItem();
	
	Port* port() { return m_pPort; }
	//////////////////////////////////////////////////////////////////////////

	QRectF  boundingRect() const override;
	void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;

	void mousePressEvent(QGraphicsSceneMouseEvent * event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:

	void portConnected(Port* src, Port* target);
	void portDisconnected(Port* src, Port* target);

private:

	Port* m_pPort;							//!< ¶Ë¿Ú
	ProcessorGraphicsBlock* m_pBlock;		//!< Block
};

}
