#include "portarrowlink.h"

using namespace GraphicsUI;
namespace DesignNet
{

PortArrowLink::PortArrowLink(QGraphicsItem *parent)
	: ArrowLinkItem(parent)
{

}

PortArrowLink::~PortArrowLink()
{

}

void PortArrowLink::connectPort(PortItem* pSrc, PortItem* pTarget)
{

}

void PortArrowLink::onControlItemPostionChanged()
{

}

void PortArrowLink::updatePosition()
{

}

void PortArrowLink::onProcessorPosChanged()
{

}

QVariant PortArrowLink::itemChange(GraphicsItemChange change, const QVariant & v)
{
	return v;
}

}

