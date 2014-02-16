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
	setCacheMode(DeviceCoordinateCache);
	//////////////////////////////////////////////////////////////////////////

	m_dropdownShadowEffect = new QGraphicsDropShadowEffect(this);
	m_dropdownShadowEffect->setBlurRadius(8);
	m_dropdownShadowEffect->setOffset(2, 2);
	m_dropdownShadowEffect->setColor(Qt::gray);
	setGraphicsEffect(m_dropdownShadowEffect);
	m_dropdownShadowEffect->setEnabled(false);

	/// title
	m_titleItem = new BlockTextItem(this);
	m_titleItem->setBlockName(m_processor->name());
	///
	/// close
	m_closeItem = new GraphicsAutoShowHideItem(this);
	m_closeItem->setSize(QSize(16, 16));
	m_closeItem->setPos(boundingRect().right() - m_closeItem->boundingRect().width() - ITEMSPACING, boundingRect().top());
	m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
	QObject::connect(m_closeItem, SIGNAL(clicked()), this, SIGNAL(closed()));
	///
	/// 显示端口按钮
	m_btnShowPort = new GraphicsToolButton(this);
	m_btnShowPort->setPos(boundingRect().right() - m_btnShowPort->boundingRect().width() - ITEMSPACING, boundingRect().bottom() + m_btnShowPort->boundingRect().width());
	m_btnShowPort->setPixmap(QPixmap(":/media/showport.png"));
	QObject::connect(m_btnShowPort, SIGNAL(clicked()), this, SLOT(onSetPortVisible()));
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
		SLOT(printToOutputPanePopup(QString)));
	
	if (m_processor)
		QObject::connect(m_processor, SIGNAL(logout(QString)), this, SLOT(onShowLog(QString)));

	m_layoutDirty = true;
	
	init();

	QFont font("Time", 10);
	m_titleItem->setPos(boundingRect().left() + ITEMSPACING, boundingRect().top() + ITEMSPACING);
	m_titleItem->setMaxWidth(boundingRect().width() - m_closeItem->boundingRect().width() - (ITEMSPACING<<1));
	m_configWidget = 0;
}

ProcessorGraphicsBlock::~ProcessorGraphicsBlock()
{
	if (m_configWidget)
	{
		m_configWidget->close();
		m_configWidget->deleteLater();
	}
}

QRectF ProcessorGraphicsBlock::boundingRect() const
{
	QSizeF size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	return QRectF(-size.width()/ 2, -size.height()/2, size.width(), size.height());
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

void ProcessorGraphicsBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget* widget /*= 0 */ )
{
	prepareGeometryChange();
	
	QRectF rcBounding = boundingRect();
	painter->setPen(Qt::NoPen);

	painter->fillRect(rcBounding, QColor(102, 175,253));
	if(isSelected())
	{
		painter->save();
		painter->setPen(QColor(255, 255, 255));
		QRectF rect = boundingRect();
		rect.adjust(2, 2, -2, -2);
		painter->drawRect(rect);

		painter->setPen(QPen(Qt::darkBlue, 2));
		painter->drawRect(rcBounding);
		painter->restore();
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Emphasize);
	}
	else
	{
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Normal);
	}
	painter->save();
	painter->setPen(QPen(Qt::white, 1));
	painter->drawLine(QPointF(rcBounding.left(), rcBounding.top() + TITLE_HEIGHT),
		QPointF(rcBounding.right(), rcBounding.top() + TITLE_HEIGHT));
	int index = m_processor->id();
	QFont font("Time", 10);
	QFontMetrics fm(font);
	QRectF indexRect = fm.boundingRect(tr("%1").arg(index));
	qreal tempWidth = qMax(indexRect.width(), indexRect.height());
	QRectF rcIndex(rcBounding.right() - tempWidth - ITEMSPACING, rcBounding.bottom() - tempWidth - ITEMSPACING, 
		tempWidth, tempWidth);
	painter->setBrush(Qt::black);
	painter->drawEllipse(rcIndex);
	painter->drawText(rcIndex, Qt::AlignCenter, tr("%1").arg(index));
	painter->setRenderHints(QPainter::Antialiasing);
	painter->restore();
	QPixmap iconPixmap = m_processor->icon().pixmap(16, 16);
	QRectF rcIcon(rcBounding.left() + ITEMSPACING, rcBounding.bottom() - ITEMSPACING - iconPixmap.height(), iconPixmap.width(), iconPixmap.height());
	painter->drawImage(rcIcon, iconPixmap.toImage());
}

void ProcessorGraphicsBlock::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	m_closeItem->animateShow(true);
	QGraphicsItem::hoverEnterEvent(event);
}

void ProcessorGraphicsBlock::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	m_closeItem->animateShow(false);
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
	{
		m_state |= s;
	}
	else
		m_state = s;
}

void ProcessorGraphicsBlock::onShowLog( const QString &log )
{
	QString strTimeLog = QTime::currentTime().toString("hh:mm:ss.zzz");
	strTimeLog = strTimeLog + "> " + sender()->objectName() + ": " + log;
	emit processorLog(strTimeLog);
}

void ProcessorGraphicsBlock::mouseDoubleClickEvent(  QGraphicsSceneMouseEvent * event )
{
	if (!m_configWidget)
	{
		m_configWidget = ProcessorConfigManager::instance()->createConfigWidget(m_processor, 0);
		QObject::connect(m_configWidget, SIGNAL(destroyed ( QObject * )), this, SLOT(configWidgetClosed()));
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
}

void ProcessorGraphicsBlock::setHover( bool bHovered /*= true*/ )
{
	setState(STATE_MOUSEOVER, bHovered);
	m_dropdownShadowEffect->setColor(bHovered ? Qt::yellow : Qt::gray);
	m_dropdownShadowEffect->setEnabled(bHovered);
}

QPointF ProcessorGraphicsBlock::getCrossPoint( const QLineF &line )
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
}

void ProcessorGraphicsBlock::setEmphasized( bool bEmphasized /*= true*/ )
{
	setState(STATE_EMPHASIZE, bEmphasized);
	m_dropdownShadowEffect->setBlurRadius(bEmphasized ? 35 : 0);
	m_dropdownShadowEffect->setEnabled(bEmphasized);
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
		scene()->addItem(pLink);
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

}
