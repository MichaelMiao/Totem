#include "portarrowlink.h"
#include "../designnetbase/designnetspace.h"
#include "../designnetbase/port.h"
#include "../designnetbase/processor.h"
#include "GraphicsUI/arrowlinkcontrolitem.h"
#include "portitem.h"
#include "processorgraphicsblock.h"


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
	int iPosX = (posPortSrc.x() + posPortTarget.x()) / 2;
	int iPosY = (posPortSrc.y() + posPortTarget.y()) / 2;
	QPointF ptTemp1, ptTemp2;
	ptTemp1.setX(iPosX);
	ptTemp1.setY(posPortSrc.y());
	ptTemp2.setX(iPosX);
	ptTemp2.setY(posPortTarget.y());
	QLineF lineTemp1(posPortSrc, ptTemp1);
	QLineF lineTemp2(posPortTarget, ptTemp2);
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
	if (change == ItemSelectedHasChanged)
	{
		ProcessorGraphicsBlock* pBlockSrc = (ProcessorGraphicsBlock*)m_srcPortItem->parentObject();
		pBlockSrc->setEmphasized(v.toBool());
		pBlockSrc = (ProcessorGraphicsBlock*)m_targetPortItem->parentObject();
		pBlockSrc->setEmphasized(v.toBool());
	}
	return v;
}

void PortArrowLink::onPortVisibleChanged()
{
	PortItem* pItem = (PortItem*)sender();
	this->setVisible(pItem->isVisible());
}

void PortArrowLink::relayout()
{
	QPointF ptSrc		= m_srcPortItem->scenePos();
	QPointF ptTarget	= m_targetPortItem->scenePos();
	int iPosX = (ptSrc.x() + ptTarget.x()) / 2;
	m_controlPoint_1->setPos(m_srcPortItem->mapFromScene(iPosX, ptSrc.y()));
	m_controlPoint_2->setPos(m_targetPortItem->mapFromScene(iPosX, ptTarget.y()));
}

}
