#include "portitemdataviewer.h"
#include <QPainter>


namespace DesignNet
{
PortItemDataViewer::PortItemDataViewer(Port* pItem)
	: m_pItem(pItem)
{
//	connect(pItem, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
}

PortItemDataViewer::~PortItemDataViewer()
{

}

QRectF PortItemDataViewer::boundingRect() const
{
	qreal penWidth = 1;
	return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
		20 + penWidth, 20 + penWidth);
}

void PortItemDataViewer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
}

void PortItemDataViewer::onDataChanged()
{

}


}