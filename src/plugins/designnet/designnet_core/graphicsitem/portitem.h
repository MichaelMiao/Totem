#pragma once
#include "../designnet_core_global.h"
#include <QGraphicsObject>


namespace DesignNet{

class Port;
class ProcessorGraphicsBlock;
class DESIGNNET_CORE_EXPORT PortItem : public QGraphicsObject
{
public:

	PortItem(Port* pPort, ProcessorGraphicsBlock* pBlock);
	virtual ~PortItem();
	
	//////////////////////////////////////////////////////////////////////////

	QRectF  boundingRect() const override;
	void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;

	void mousePressEvent(QGraphicsSceneMouseEvent * event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

	Port* m_pPort;						// ¶Ë¿Ú
	ProcessorGraphicsBlock* m_pBlock;	// Block
};

}
