#pragma once
#include <QGraphicsObject>
#include "designnet_core_global.h"


namespace DesignNet
{
class Port;
class DESIGNNET_CORE_EXPORT PortItemDataViewer : public QGraphicsObject
{
	Q_OBJECT

public:
	
	explicit PortItemDataViewer(Port* pItem);
	~PortItemDataViewer();

	QRectF boundingRect() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget);

public slots:
	
	void onDataChanged();

private:

	Port*	m_pItem;

};
}
