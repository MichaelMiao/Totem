#include "processorarrowlink.h"
#include <QEvent>
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

void ProcessorArrowLink::connectProcessor(ProcessorGraphicsBlock* pSrc, ProcessorGraphicsBlock* pTarget)
{
	m_srcProcessor = pSrc;
	m_targetProcessor = pTarget;
	m_controlPoint_1->setParentItem(m_srcProcessor);
	m_controlPoint_2->setParentItem(m_targetProcessor);


	connect(m_srcProcessor, SIGNAL(positionChanged()), this, SLOT(onProcessorPosChanged()));
	connect(m_targetProcessor, SIGNAL(positionChanged()), this, SLOT(onProcessorPosChanged()));

	relayout();
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
	}
	return ArrowLinkItem::itemChange(change, value);
}

void ProcessorArrowLink::relayout()
{
	QPointF ptSrc		= m_srcProcessor->scenePos();
	QPointF ptTarget	= m_targetProcessor->scenePos();
	int iPosX = (ptSrc.x() + ptTarget.x()) / 2;
	m_controlPoint_1->setPos(m_srcProcessor->mapFromScene(iPosX, ptSrc.y()));
	m_controlPoint_2->setPos(m_targetProcessor->mapFromScene(iPosX, ptTarget.y()));
}

}
