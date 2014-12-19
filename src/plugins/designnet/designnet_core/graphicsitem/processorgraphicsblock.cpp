#include "processorgraphicsblock.h"
#include "../designnetbase/designnetspace.h"
#include <QApplication>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QGraphicsBlurEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStyleOptionGraphicsItem>
#include <QTime>
#include <QTimer>
#include "../../../coreplugin/icore.h"
#include "../../../coreplugin/messagemanager.h"
#include "../data/matrixdata.h"
#include "../designnetbase/processorconfigmanager.h"
#include "../graphicsitem/blocktextitem.h"
#include "../property/property.h"
#include "../widgets/processorconfigwidget.h"
#include "../designnetconstants.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "GraphicsUI/graphicstoolbutton.h"
#include "GraphicsUI/loadingicon.h"
#include "GraphicsUI/textanimationblock.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmlserializer.h"
#include "portarrowlink.h"
#include "portitem.h"
#include "designnetview.h"


using namespace GraphicsUI;
namespace DesignNet{
const int PORT_MARGIN = 15;
#define MAX_LENGTH	1000


void Position::serialize(Utils::XmlSerializer& s) const
{
	s.serialize("X", m_x);
	s.serialize("Y", m_y);
	s.serialize("id", m_id);
}

void Position::deserialize(Utils::XmlDeserializer& s) 
{
	s.deserialize("id", m_id);
	s.deserialize("X", m_x);
	s.deserialize("Y", m_y);
}

ProcessorGraphicsBlock::ProcessorGraphicsBlock(Processor *processor /*= 0*/, QGraphicsScene *s /*= 0*/, DesignNetView* pView /*= 0*/,
	const QPointF &pos /*= QPointF(0, 0)*/, QGraphicsItem *parent /*= 0*/)
	: QGraphicsObject(parent), m_processor(processor), m_pNetView(pView), m_bInputPortVisible(false), m_bOutputPortVisible(false)
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);

	//////////////////////////////////////////////////////////////////////////

// 	m_dropdownShadowEffect = new QGraphicsDropShadowEffect(this);
// 	m_dropdownShadowEffect->setBlurRadius(8);
// 	m_dropdownShadowEffect->setOffset(2, 2);
// 	m_dropdownShadowEffect->setColor(Qt::gray);
// 	setGraphicsEffect(m_dropdownShadowEffect);
// 	m_dropdownShadowEffect->setEnabled(false);

	/// title
	m_titleItem = new BlockTextItem(this);
	m_titleItem->setBlockName(m_processor->name());
	///
	/// 显示端口按钮
	m_btnShowPort = new GraphicsToolButton(this);
	m_btnShowPort->setPos(boundingRect().right() - m_btnShowPort->boundingRect().width() - ITEMSPACING, boundingRect().bottom() + m_btnShowPort->boundingRect().width());
	m_btnShowPort->setPixmap(QPixmap(":/media/showport.png"));
	QObject::connect(m_btnShowPort, SIGNAL(clicked()), this, SLOT(onSetPortVisible()));

	QObject::connect(processor, SIGNAL(processStarted()), this, SLOT(onProcessStarted()));
	QObject::connect(processor, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));

	QList<Port*> tempPort = processor->getPorts(Port::IN_PORT);
	for (QList<Port*>::iterator itr = tempPort.begin(); itr != tempPort.end(); itr++)
	{
		QObject::connect(*itr, SIGNAL(connectPort(Port*, Port*)), this, SLOT(onPortConnected(Port*, Port*)));
		QObject::connect(*itr, SIGNAL(disconnectPort(Port*, Port*)), this, SLOT(onPortDisconnected(Port*, Port*)));
		PortItem* portItem = new PortItem(*itr, this);
		m_inputPorts.push_back(portItem);
		portItem->setVisible(false);
	}

	tempPort = processor->getPorts(Port::OUT_PORT);
	for (QList<Port*>::iterator itr = tempPort.begin(); itr != tempPort.end(); itr++)
	{
		PortItem* portItem = new PortItem(*itr, this);
		m_outputPorts.push_back(portItem);
		portItem->setVisible(false);
	} 

	//////////////////////////////////////////////////////////////////////////


	QObject::connect(this, SIGNAL(processorLog(QString)),
		Core::ICore::messageManager(),
		SLOT(printToOutputPanePopup(QString)), Qt::QueuedConnection);
	
	if (m_processor)
	{
		QObject::connect(m_processor, SIGNAL(logout(QString)), this, SLOT(onShowLog(QString)), Qt::BlockingQueuedConnection);
		QObject::connect(m_processor, SIGNAL(portAdded(Port*)), this, SLOT(onAddPort(Port*)));
		QObject::connect(m_processor, SIGNAL(portRemoved(Port*)), this, SLOT(relayoutPort()));
	}

	m_layoutDirty = true;
	init();

	QFont font("Time", 10);
	m_titleItem->setFont(font);
	QRectF rc = m_titleItem->boundingRect();
	m_titleItem->setPos(0, boundingRect().top() - rc.height() - 1);
	m_configWidget = 0;
}

ProcessorGraphicsBlock::~ProcessorGraphicsBlock()
{
	if (m_configWidget)
	{
		m_configWidget->close();
		delete m_configWidget;
	}
	
}

QRectF ProcessorGraphicsBlock::boundingRect() const
{
	QSizeF size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	return QRectF(-size.width() / 2, -size.height() / 2, size.width(), size.height());
}

QSizeF ProcessorGraphicsBlock::minimumSizeHint() const
{
    return QSizeF(DEFAULT_WIDTH, TITLE_HEIGHT + 2 * PORT_MARGIN + 1);
}

Processor *ProcessorGraphicsBlock::processor()
{
    return (Processor *)m_processor;
}

void ProcessorGraphicsBlock::init()
{
	if (scene())
		this->setPos(QPointF(m_pos.m_x, m_pos.m_y));
}

void ProcessorGraphicsBlock::propertyRemoving(Property *prop)
{
}

void ProcessorGraphicsBlock::propertyRemoved(Property *prop)
{
}

QVariant ProcessorGraphicsBlock::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemSelectedHasChanged && scene())
    {
        if(value.toBool() == false)
            emit selectionChanged(false);
		setEmphasized(value.toBool());
    }
	else if (change == ItemPositionChange && scene())
	{
		emit positionChanged();
	}
	else if (change == ItemChildRemovedChange && scene())
	{
		prepareGeometryChange();
		update();
	}
	
    return QGraphicsItem::itemChange(change,value);
}

void ProcessorGraphicsBlock::propertyAdded( Property* prop )
{
	QObject::connect(prop, SIGNAL(changed()), this, SLOT(onPropertyChanged_internal()));	
}

void ProcessorGraphicsBlock::onPropertyChanged_internal()
{
	Property *prop = qobject_cast<Property*>(sender());
	if(prop)
		propertyChanged(prop);
}

void ProcessorGraphicsBlock::propertyChanged( Property *prop )
{

}

void ProcessorGraphicsBlock::paint(QPainter* painter,
	const QStyleOptionGraphicsItem *option, QWidget* widget)
{
	painter->setRenderHint(QPainter::Antialiasing);
	QRectF rcBounding = boundingRect();
	rcBounding.adjust(2, 2, -2, -2);
	painter->setPen(Qt::NoPen);
	QPainterPath path;
	path.addEllipse(rcBounding);
	painter->fillPath(path, Qt::gray);
	
	QColor clr = Qt::white;
	if (option->state & QStyle::State_MouseOver)
		clr = Qt::lightGray;
	if (option->state & QStyle::State_Selected)
		clr = Qt::red;
	painter->setPen(clr);
	painter->drawEllipse(rcBounding);
	QPixmap iconPixmap = m_processor->icon().pixmap(16, 16);
	QRectF rcIcon(-iconPixmap.width() / 2, -iconPixmap.height() / 2, iconPixmap.width(), iconPixmap.height());
	painter->drawImage(rcIcon, iconPixmap.toImage());
}

void ProcessorGraphicsBlock::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverEnterEvent(event);
}

void ProcessorGraphicsBlock::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverLeaveEvent(event);
}

void ProcessorGraphicsBlock::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	update();
	QGraphicsItem::mousePressEvent(event);
}

void ProcessorGraphicsBlock::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	this->update();
	QGraphicsItem::mouseMoveEvent(event);
}

void ProcessorGraphicsBlock::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
	QGraphicsItem::mouseReleaseEvent(event);
}

void ProcessorGraphicsBlock::serialize( Utils::XmlSerializer& s ) const
{
	m_processor->serialize(s);
	Position p(this->scenePos());
	s.serialize("Pos", p);
}

void ProcessorGraphicsBlock::deserialize( Utils::XmlDeserializer& s )
{
	s.deserialize("Pos", m_pos);
}

DesignNet::Position ProcessorGraphicsBlock::originalPosition() const
{
	return m_pos;
}

void ProcessorGraphicsBlock::createContextMenu( QMenu *parentMenu )
{

}

float ProcessorGraphicsBlock::statusBarHeight()
{
	return (float)STATUSBAR_HEIGHT;
}

void ProcessorGraphicsBlock::setLayoutDirty( const bool &dirty /*= true*/ )
{
	m_layoutDirty = dirty;
}

void ProcessorGraphicsBlock::setState( const State &s, const bool &bAdd /*= false*/ )
{
	if (bAdd)
		m_state |= s;
	else
		m_state = s;
}

void ProcessorGraphicsBlock::onShowLog( const QString &log )
{
	m_strLog = QTime::currentTime().toString("hh:mm:ss.zzz");
	m_strLog = m_strLog + "> " + sender()->objectName() + ": " + log;
	emit processorLog(m_strLog);
}

void ProcessorGraphicsBlock::mouseDoubleClickEvent(  QGraphicsSceneMouseEvent * event )
{
	if (!m_configWidget)
	{
		m_configWidget = ProcessorConfigManager::instance()->createConfigWidget(m_processor, 0);
		if (m_configWidget)
			QObject::connect(m_configWidget, SIGNAL(destroyed(QObject *)), this, SLOT(configWidgetClosed()));
	}
	if (m_configWidget)
	{
		m_configWidget->move(event->buttonDownScreenPos(Qt::LeftButton));
		m_configWidget->show();
		
		m_configWidget->activateWindow();
	}
}

void ProcessorGraphicsBlock::configWidgetClosed()
{
	m_configWidget = 0;
}

void ProcessorGraphicsBlock::startDeserialize(Utils::XmlDeserializer& s)
{
	deserialize(s);
}

void ProcessorGraphicsBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent *event )
{
	event->accept();
	QMenu menu;
	QAction* pAction = menu.addAction("Add Input Port");
	pAction->setDisabled(!m_processor->isResizableInput());
	QObject::connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onClickAddPort()));
	QMenu *pMenu = menu.addMenu("Remove Port");
	pMenu->setDisabled(!m_processor->isResizableInput());

	QList<PortItem*> ports;
	ports << m_inputPorts << m_outputPorts;
	for (int i = 0; i < ports.size(); i++)
	{
		QString strPrefix = ports.at(i)->port()->portType() == Port::IN_PORT ? tr("[IN]:") : tr("[OUT]:");
		strPrefix += ports.at(i)->port()->name();
		QAction *pAction = pMenu->addAction(QPixmap::fromImage(ports.at(i)->getImage()), strPrefix);
		pAction->setData(qVariantFromValue<void*>((PortItem *)ports.at(i)));
		pAction->setDisabled(!ports.at(i)->port()->isRemovable());
		QObject::connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onClickRemovePort()));
	}
	menu.exec(event->screenPos());
}

void ProcessorGraphicsBlock::setHover( bool bHovered /*= true*/ )
{
	setState(STATE_MOUSEOVER, bHovered);
//	m_dropdownShadowEffect->setColor(bHovered ? Qt::yellow : Qt::gray);
//	m_dropdownShadowEffect->setEnabled(bHovered);
}

QPointF ProcessorGraphicsBlock::getCrossPoint(const QLineF &line)
{
	QLineF lineSrc(line);
	lineSrc.setLength(MAX_LENGTH);
	QPointF ptTemp(0, 0);
	QPolygonF polygon(mapToScene(boundingRect()));
	int iCount = polygon.count();
	if (iCount > 0)
	{
		QPointF p2;
		QPointF p1 = polygon.at(0);
		p2 = p1;
		for (int i = 1; i < iCount; i++)
		{
			QPointF p2 = polygon.at(i);
			QLineF lineTemp(p2, p1);
			QPointF ptTemp;
			if (lineSrc.intersect(lineTemp, &ptTemp) == QLineF::BoundedIntersection)
				return ptTemp;
			p1 = p2;
		}
	}
	return QPointF(0, 0);
//  	QPointF ptTemp(0, 0);
// 	QRectF r = boundingRect();
// 	QPointF ptCenter = mapToScene(boundingRect().center());
// 	float fWidth	= boundingRect().width() / 2.0;
// 	float fHeight = boundingRect().height() / 2.0;
// 	qDebug() << fHeight << endl;
// 	QLineF lineSrc(line);
// 	lineSrc.setLength(MAX_LENGTH);
// 	qDebug() << "======================= " << lineSrc.p1() << lineSrc.p2();
// 	lineSrc.translate(-ptCenter.x(), -ptCenter.y());
// 	qDebug() << lineSrc.p1() << lineSrc.p2();
// 	if (fabs(lineSrc.dx() - 0) < 0.00001)
// 	{
// 		ptTemp.setY(lineSrc.dy() > 0 ? fHeight : -fHeight);
// 		return ptTemp + ptCenter;
// 	}
// 	if (fabs(lineSrc.dy() - 0) < 0.00001)
// 	{
// 		ptTemp.setX(lineSrc.dx() > 0 ? fWidth : -fWidth);
// 		return ptTemp + ptCenter;
// 	}
// 	float fK = lineSrc.dy() / lineSrc.dx();
// 	ptTemp.setX(fWidth * fHeight / sqrt(fHeight * fHeight + fWidth * fWidth + fK * fK) * (lineSrc.dx() > 0 ? 1 : -1));
// 	ptTemp.setY(fK * ptTemp.x());
// 	qDebug() << "aaaaaaaaaaaaa" << ptTemp;
// 	return ptTemp + ptCenter;
}

void ProcessorGraphicsBlock::setEmphasized( bool bEmphasized )
{
	setState(STATE_EMPHASIZE, bEmphasized);
	setZValue(!bEmphasized ? DesignNet::Constants::ZValue_GraphicsBlock_Normal : DesignNet::Constants::ZValue_GraphicsBlock_Emphasize);
//	m_dropdownShadowEffect->setBlurRadius(bEmphasized ? 35 : 0);
//	m_dropdownShadowEffect->setEnabled(bEmphasized);
}

void ProcessorGraphicsBlock::setPortVisible(bool bVisible, int iType)
{
	int iHeight = 0;
	int iTop	= 0;
	if ((iType > 0 && m_bOutputPortVisible == bVisible) || (iType < 0 && m_bInputPortVisible == bVisible)
		|| (iType == 0 && m_bInputPortVisible == bVisible && m_bOutputPortVisible == bVisible))
	{
		return;
	}
	if (iType >= 0)
		m_bOutputPortVisible = bVisible;
	if (iType <= 0)
		m_bInputPortVisible = bVisible;

	int iInputCount = m_inputPorts.size();
	if (iInputCount > 0 && iType <= 0)
	{
		iHeight = iInputCount * (PORTITEM_WIDTH + PORTITEM_SPACE) - PORTITEM_SPACE;
		iTop	= boundingRect().top() + (boundingRect().height() - iHeight) / 2;
		for (QList<PortItem*>::iterator itr = m_inputPorts.begin(); itr != m_inputPorts.end(); itr++)
		{
			PortItem* pItem = *itr;
			QRectF rect = boundingRect();
			pItem->setPos(rect.left() - PORTITEM_WIDTH, iTop);
			pItem->setVisible(bVisible);
			iTop += PORTITEM_WIDTH + PORTITEM_SPACE;
		}
		m_bInputPortVisible = bVisible;
		emit portVisibleChanged(bVisible, -1);
	}
	
	if (m_outputPorts.size() > 0 && iType >= 0)
	{
		iHeight = m_outputPorts.size() * (PORTITEM_WIDTH + PORTITEM_SPACE) - PORTITEM_SPACE;
		qDebug() << m_outputPorts.size() << "-------" << iHeight;
		iTop = boundingRect().top() + (boundingRect().height() - iHeight) / 2;
		for (QList<PortItem*>::iterator itr = m_outputPorts.begin(); itr != m_outputPorts.end(); itr++)
		{
			PortItem* pItem = *itr;
			QRectF rect = boundingRect();
			pItem->setPos(rect.right() + PORTITEM_WIDTH, iTop);
			pItem->setVisible(bVisible);
			iTop += PORTITEM_WIDTH + PORTITEM_SPACE;
		}
		emit portVisibleChanged(bVisible, 1);
	}
}

bool ProcessorGraphicsBlock::isPortVisible(int iType)
{
	return iType < 0 ? m_bInputPortVisible : (iType == 0 ? m_bInputPortVisible && m_bOutputPortVisible : m_bOutputPortVisible);
}

void ProcessorGraphicsBlock::onSetPortVisible()
{
	setPortVisible(!m_bInputPortVisible || !m_bOutputPortVisible, 0);
}

void ProcessorGraphicsBlock::onPortConnected(Port* src, Port* target)
{
	if (scene())
	{
		PortArrowLink* pLink = new PortArrowLink(this);
		PortItem* pSrcPortItem = m_pNetView->getPortItem(src);
		PortItem* pTargetPortItem = m_pNetView->getPortItem(target);
		QObject::connect(pSrcPortItem, SIGNAL(visibleChanged()), pLink, SLOT(onPortVisibleChanged()));
		pLink->connectPort(pSrcPortItem, pTargetPortItem);
		m_portArrowLinks.append(pLink);
		if (src->processor() == this->processor() && m_bOutputPortVisible
			|| target->processor() == this->processor() && m_bInputPortVisible)
		{
			pLink->setVisible(true);
		}
		else
			pLink->setVisible(false);
	}
}

void ProcessorGraphicsBlock::onPortDisconnected(Port* src, Port* target)
{
	if (scene())
	{
		QList<PortArrowLink*>::iterator itr = m_portArrowLinks.begin();
		PortArrowLink* pLink = 0;
		while (itr != m_portArrowLinks.end())
		{
			if ((*itr)->getSrc()->port() == src && (*itr)->getTarget()->port() == target)
			{
				pLink = (*itr);
				break;
			}
			itr++;
		}
		if (pLink)
		{
			m_portArrowLinks.erase(itr);
			scene()->removeItem(pLink);
			delete pLink;
		}
	}
}

PortItem* ProcessorGraphicsBlock::cvtPortItem(Port* p)
{
	if (p->portType() == Port::IN_PORT)
	{
		QList<PortItem*>::iterator itr = m_inputPorts.begin();
		while (itr != m_inputPorts.end())
		{
			if ((*itr)->port() == p)
				return *itr;
			itr++;
		}
	}
	else
	{
		QList<PortItem*>::iterator itr = m_outputPorts.begin();
		while (itr != m_outputPorts.end())
		{
			if ((*itr)->port() == p)
				return *itr;
			itr++;
		}
	}
	return 0;
}

void ProcessorGraphicsBlock::onProcessStarted()
{
}

void ProcessorGraphicsBlock::onProcessFinished()
{
}

void ProcessorGraphicsBlock::onAddPort(Port* port)
{
	PortItem* portItem = new PortItem(port, this);
	if (port->portType() == Port::OUT_PORT)
	{
		m_outputPorts.push_back(portItem);
	}
	else
	{
		QObject::connect(port, SIGNAL(connectPort(Port*, Port*)), this, SLOT(onPortConnected(Port*, Port*)));
		QObject::connect(port, SIGNAL(disconnectPort(Port*, Port*)), this, SLOT(onPortDisconnected(Port*, Port*)));
		m_inputPorts.push_back(portItem);
	}
	relayoutPort();
}

void ProcessorGraphicsBlock::onClickAddPort()
{
	QString strPortName = tr("Port(%1)").arg(m_processor->getPorts(Port::IN_PORT).size());
	m_processor->addPort(Port::IN_PORT, DATATYPE_MATRIX, strPortName, true);
}

QPainterPath ProcessorGraphicsBlock::shape() const
{
	QRectF rcBounding = boundingRect();
	rcBounding.adjust(2, 2, -2, -2);
	QPainterPath path;
	path.addEllipse(rcBounding);
	return path;
}

void ProcessorGraphicsBlock::onClickRemovePort()
{
	QAction *pAction = qobject_cast<QAction*>(sender());
	PortItem* pItem = (PortItem*)pAction->data().value<void*>();
	if (pItem)
	{
		m_inputPorts.removeOne(pItem);
		m_outputPorts.removeOne(pItem);
		m_processor->removePort(pItem->port());
		delete pItem;
	}
}

void ProcessorGraphicsBlock::relayoutPort()
{
	int iHeight		= 0;
	int iTop		= 0;
	int iInputCount = m_inputPorts.size();
	qDebug() << iInputCount;
	iHeight = iInputCount * (PORTITEM_WIDTH + PORTITEM_SPACE) - PORTITEM_SPACE;
	iTop	= boundingRect().top() + (boundingRect().height() - iHeight) / 2;
	for (QList<PortItem*>::iterator itr = m_inputPorts.begin(); itr != m_inputPorts.end(); itr++)
	{
		PortItem* pItem = *itr;
		QRectF rect = boundingRect();
		pItem->setPos(rect.left() - PORTITEM_WIDTH, iTop);
		pItem->setVisible(m_bInputPortVisible);
		iTop += PORTITEM_WIDTH + PORTITEM_SPACE;
	}
	iHeight = m_outputPorts.size() * (PORTITEM_WIDTH + PORTITEM_SPACE) - PORTITEM_SPACE;
	iTop = boundingRect().top() + (boundingRect().height() - iHeight) / 2;
	for (QList<PortItem*>::iterator itr = m_outputPorts.begin(); itr != m_outputPorts.end(); itr++)
	{
		PortItem* pItem = *itr;
		QRectF rect = boundingRect();
		pItem->setPos(rect.right() + PORTITEM_WIDTH, iTop);
		pItem->setVisible(m_bOutputPortVisible);
		iTop += PORTITEM_WIDTH + PORTITEM_SPACE;
	}
}

}
