#include "portarrowlink.h"
#include "../designnetbase/designnetspace.h"
#include "../designnetbase/port.h"
#include "../designnetbase/processor.h"
#include "GraphicsUI/arrowlinkcontrolitem.h"
#include "portitem.h"


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
	m_srcPortItem = pSrc;
	m_targetPortItem = pTarget;
	m_controlPoint_1->setParentItem((QGraphicsItem*)m_srcPortItem);
	m_controlPoint_2->setParentItem((QGraphicsItem*)m_targetPortItem);


	QPointF posPortSrc = m_srcPortItem->scenePos();
	QPointF posPortTarget = m_targetPortItem->scenePos();
	QPointF ptTemp1(posPortSrc.x(), posPortSrc.y() - 10);
	QPointF ptTemp2(posPortTarget.x(), posPortTarget.y() - 10);
	QLineF lineTemp1(posPortSrc, ptTemp1);
	QLineF lineTemp2(posPortTarget, ptTemp2);
	lineTemp1.setAngle(30);
	lineTemp2.setAngle(30);
	lineTemp1.setLength(200);
	lineTemp2.setLength(200);
	m_controlPoint_1->setPos(m_controlPoint_1->mapFromScene(lineTemp1.p2()));
	m_controlPoint_2->setPos(m_controlPoint_2->mapFromScene(lineTemp2.p2()));
	QLineF lineSrc(posPortSrc, m_controlPoint_1->scenePos());
	m_startPoint = mapFromScene(m_srcPortItem->scenePos());
	QLineF lineTarget(posPortTarget, m_controlPoint_2->scenePos());
	m_endPoint = mapFromScene(m_targetPortItem->scenePos());
	m_arrowLinkEndPoint.setPos(m_startPoint);
}

void PortArrowLink::onControlItemPostionChanged()
{
	updatePosition();
	m_srcPortItem->port()->processor()->space()->setModified();
}

void PortArrowLink::updatePosition()
{
	QPointF posStart = m_srcPortItem->scenePos();
	QPointF posTarget = m_targetPortItem->scenePos();
	setPoint_Internal(posStart, true);
	setPoint_Internal(posTarget, false);
}

QVariant PortArrowLink::itemChange(GraphicsItemChange change, const QVariant & v)
{
	if (change == ItemSceneHasChanged)
	{
	}
	return v;
}

void PortArrowLink::onPortVisibleChanged()
{
	PortItem* pItem = (PortItem*)sender();
	this->setVisible(pItem->isVisible());
}

}
