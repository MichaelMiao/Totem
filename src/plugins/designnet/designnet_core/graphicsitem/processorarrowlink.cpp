#include "processorarrowlink.h"
#include <QEvent>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QLineF>
#include "../designnetbase/designnetspace.h"
#include "../designnetbase/processor.h"
#include "GraphicsUI/arrowlinkcontrolitem.h"
#include "processorgraphicsblock.h"


using namespace GraphicsUI;
namespace DesignNet{

enum ControlPointEdge
{
	ControlPointEdge_Left	= 0x00000001,
	ControlPointEdge_Top	= 0x00000002,
	ControlPointEdge_Right	= 0x00000003,
	ControlPointEdge_Bottom = 0x00000004,
};

enum ControlPointDir
{
	ControlPointDir_Left	= 0x00010000,
	ControlPointDir_Top		= 0x00020000,
	ControlPointDir_Right	= 0x00030000,
	ControlPointDir_Bottom	= 0x00040000,
};

#define GETCONTROL_DIR(x)	(0xffff0000 & x)
#define GETCONTROL_EDGE(x)	(0x0000ffff & x)
#define SETCONTROL_DIR(x, v) (x = (x & 0x0000ffff) | v))
#define SETCONTROL_EDGE(x, v) (x = (x & 0xffff0000) | v))

//////////////////////////////////////////////////////////////////////////
//          \        /
//       \|------------|/
// 		  |			   |
// 		  |			   |
// 		 /|------------|\
//			/		 \
//////////////////////////////////////////////////////////////////////////
ProcessorArrowLink::ProcessorArrowLink(QGraphicsItem *parent)
	: ArrowLinkItem(parent), m_srcProcessor(0), m_targetProcessor(0)
{

}

ProcessorArrowLink::~ProcessorArrowLink()
{
}

void ProcessorArrowLink::connectProcessor( ProcessorGraphicsBlock* pSrc, ProcessorGraphicsBlock* pTarget )
{
	m_srcProcessor = pSrc;
	m_targetProcessor = pTarget;
	m_controlPoint_1->setParentItem(m_srcProcessor);
	m_controlPoint_2->setParentItem(m_targetProcessor);


	connect(m_srcProcessor, SIGNAL(positionChanged()), this, SLOT(onProcessorPosChanged()));
	connect(m_targetProcessor, SIGNAL(positionChanged()), this, SLOT(onProcessorPosChanged()));
	
	QPointF posStartProcessor = m_srcProcessor->scenePos();
	QPointF posTargetProcessor = m_targetProcessor->scenePos();
	QRectF rcSrc(mapRectToScene(m_srcProcessor->boundingRect()));
	QRectF rcTarget(mapRectToScene(m_targetProcessor->boundingRect()));
	QPointF ptTemp1(posStartProcessor.x(), posStartProcessor.y() - 10);
	QPointF ptTemp2(posTargetProcessor.x(), posTargetProcessor.y() - 10);
	QLineF lineTemp1(posStartProcessor, ptTemp1);
	QLineF lineTemp2(posTargetProcessor, ptTemp2);
	lineTemp1.setAngle(30);
	lineTemp2.setAngle(30);
	lineTemp1.setLength(200);
	lineTemp2.setLength(200);
	m_controlPoint_1->setPos(m_controlPoint_1->mapFromScene(lineTemp1.p2()));
	m_controlPoint_2->setPos(m_controlPoint_2->mapFromScene(lineTemp2.p2()));
	QLineF lineSrc(posStartProcessor, m_controlPoint_1->scenePos());
	m_startPoint = mapFromScene(m_srcProcessor->getCrossPoint(lineSrc));
	QLineF lineTarget(posTargetProcessor, m_controlPoint_2->scenePos());
	m_endPoint = mapFromScene(m_targetProcessor->getCrossPoint(lineTarget));
	m_arrowLinkEndPoint.setPos(m_startPoint);
}

void ProcessorArrowLink::onProcessorPosChanged()
{
	prepareGeometryChange();
	updatePosition();
	update();
}

void ProcessorArrowLink::onControlItemPostionChanged()
{
	updatePosition();
	m_srcProcessor->processor()->space()->setModified();
}

void ProcessorArrowLink::updatePosition()
{
	QPointF posStartProcessor = m_srcProcessor->scenePos();
	QPointF posTargetProcessor = m_targetProcessor->scenePos();
	QLineF lineSrc(posStartProcessor, m_controlPoint_1->scenePos());
	setPoint_Internal(m_srcProcessor->getCrossPoint(lineSrc), true);
	QLineF lineTarget(posTargetProcessor, m_controlPoint_2->scenePos());
	setPoint_Internal(m_targetProcessor->getCrossPoint(lineTarget), false);
}

QVariant ProcessorArrowLink::itemChange( GraphicsItemChange change, const QVariant & value )
{
	if (change == ItemSelectedHasChanged)
	{
		if (m_srcProcessor && m_targetProcessor)
		{
			m_srcProcessor->setEmphasized(value.toBool());
			m_targetProcessor->setEmphasized(value.toBool());
		}
		if (value.toBool())
		{
			QList<QGraphicsItem *> list = collidingItems();
			for (QList<QGraphicsItem*>::iterator itr = list.begin(); itr != list.end(); itr++)
				(*itr)->stackBefore(this);
			list = m_controlPoint_1->collidingItems();
			for (QList<QGraphicsItem*>::iterator itr = list.begin(); itr != list.end(); itr++)
				(*itr)->stackBefore(m_controlPoint_1);
			list = m_controlPoint_2->collidingItems();
			for (QList<QGraphicsItem*>::iterator itr = list.begin(); itr != list.end(); itr++)
				(*itr)->stackBefore(m_controlPoint_2);
		}
	}
	return ArrowLinkItem::itemChange(change, value);
}

}
