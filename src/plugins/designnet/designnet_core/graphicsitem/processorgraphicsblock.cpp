
#include "../designnetbase/block.h"
#include "../designnetbase/designnetspace.h"
#include "../data/matrixdata.h"
#include "../designnetbase/processorconfigmanager.h"
#include "../widgets/processorconfigwidget.h"
#include "processorgraphicsblock.h"
#include "portgraphicsitem.h"
#include "port.h"

#include "property/property.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "GraphicsUI/graphicstoolbutton.h"
#include "GraphicsUI/textanimationblock.h"
#include "designnetconstants.h"
#include "graphicsitem/blocktextitem.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"


#include <QApplication>
#include <QPainter>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsScene>
#include <QMenu>
#include <QFont>
#include <QTimer>
#include <QTime>
#include <QGraphicsBlurEffect>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
using namespace GraphicsUI;
namespace DesignNet{
const int PORT_MARGIN = 15;


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

ProcessorGraphicsBlock::ProcessorGraphicsBlock( Processor *processor /*= 0*/, QGraphicsScene *s /*= 0*/, const QPointF &pos /*= QPointF(0, 0)*/, QGraphicsItem *parent /*= 0*/ )
	: QGraphicsItem(parent), m_processor(processor)
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
	setCacheMode(DeviceCoordinateCache);
	m_addInputPortItem = new GraphicsUI::GraphicsToolButton(this);
	m_addInputPortItem->setPixmap(QPixmap(Core::ICore::resourcePath() + "/" + QLatin1String(DesignNet::Constants::ICON_LIST_ADD)));
	m_addInputPortItem->setVisible(false);
	QObject::connect(m_addInputPortItem, SIGNAL(clicked()), this, SLOT(onAddPort()));
	///
	/// title
	m_titleItem = new BlockTextItem(this);
	m_titleItem->setBlockName(m_processor->name());
	///
	/// close按钮
	m_closeItem = new GraphicsAutoShowHideItem(this);
	m_closeItem->setSize(QSize(16, 16));
	m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
	m_statusBlock = new TextAnimationBlock(this);
	m_statusBlock->setSizeF(QSizeF(STATUSBAR_HEIGHT, DEFAULT_WIDTH));
	QObject::connect(m_closeItem, SIGNAL(clicked()), this, SIGNAL(closed()));

	QTimer::singleShot(0, this, SLOT(relayout()));
	QObject::connect(this, SIGNAL(processorLog(QString)),
		Core::ICore::messageManager(),
		SLOT(printToOutputPanePopup(QString)));
	if (m_processor)
	{
		QObject::connect(m_processor, SIGNAL(logout(QString)),
			this, SLOT(onShowLog(QString)));
		setPortCountResizable(m_processor->isPortCountResizable());
	}
	

	m_layoutDirty = true;
	
	init();
	if (s)
	{
		s->addItem(this);
		this->setPos(pos);
	}
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
    QRectF mainAreaRect = mainRect();
	QRectF rectF = mainAreaRect;
	rectF.setTop(rectF.top() - TITLE_HEIGHT - 1);
	rectF.setBottom(rectF.bottom() + STATUSBAR_HEIGHT + 1);
	return rectF;
}

QSizeF ProcessorGraphicsBlock::minimumSizeHint() const
{
    return QSizeF(DEFAULT_WIDTH, TITLE_HEIGHT + 2 * PORT_MARGIN + 1);
}

void ProcessorGraphicsBlock::layoutItems()
{
    prepareGeometryChange();

	m_closeItem->setPos(boundingRect().right() - m_closeItem->boundingRect().width() - ITEMSPACING, boundingRect().top());
    QRectF rectF = mainRect();
    float posX = rectF.left() + PortGraphicsItem::DEFAULT_PORT_HEIGHT / 2;
    float posY = rectF.top() + PortGraphicsItem::DEFAULT_PORT_HEIGHT / 2 + ITEMSPACING;
	QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    if(m_inputPortItems.count() != 0)
    {
		int i = 0;
        foreach(PortGraphicsItem *portItem, m_inputPortItems)
        {
			QPropertyAnimation *animation = new QPropertyAnimation(portItem, "pos");
			animation->setDuration(750 + i++*15);
			animation->setEasingCurve(QEasingCurve::InOutBack);
			animation->setEndValue(QPointF(posX, posY));
			animationGroup->addAnimation(animation);
			posY += PortGraphicsItem::DEFAULT_PORT_HEIGHT + ITEMSPACING;
			portItem->showTypeImage(PortGraphicsItem::RIGHT);
        }
		
    }
	if (m_addInputPortItem->isVisible())
	{
		m_addInputPortItem->setPos(posX, posY);
	}

	posX = rectF.right() - PortGraphicsItem::DEFAULT_PORT_HEIGHT / 2;
	posY = rectF.top() + PortGraphicsItem::DEFAULT_PORT_HEIGHT /2 + ITEMSPACING;
    if(m_outputPortItems.count() != 0)
    {
		int i = 0;
        foreach(PortGraphicsItem *portItem, m_outputPortItems)
        {
			QPropertyAnimation *animation = new QPropertyAnimation(portItem, "pos");
			animation->setDuration(750 + i++*15);
			animation->setEasingCurve(QEasingCurve::InOutBack);
			animation->setEndValue(QPointF(posX, posY));
			animationGroup->addAnimation(animation);
			posY += PortGraphicsItem::DEFAULT_PORT_HEIGHT + ITEMSPACING;
			portItem->showTypeImage(PortGraphicsItem::LEFT);
        }
    }
	animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
	m_statusBlock->setTextColor(Qt::white);
	m_statusBlock->setPos(rectF.left(), rectF.bottom() + 1);
	m_statusBlock->setSizeF(QSizeF(rectF.width(), STATUSBAR_HEIGHT));
}

Processor *ProcessorGraphicsBlock::processor()
{
    return (Processor *)m_processor;
}

/**
 * \fn	void ProcessorGraphicsBlock::createPortItems()
 *
 * \brief	Creates port items.
 *
 * \author	Michael_BJFU
 * \date	2013/6/5
 */

void ProcessorGraphicsBlock::createPortItems()
{
    foreach(Port* port, m_processor->getInputPorts())
    {
        PortGraphicsItem *pItem = new PortGraphicsItem(port,this);
		QObject::connect(this, SIGNAL(selectionChanged(bool)),
                pItem, SLOT(processorSelectionChanged(bool)));
		if (m_processor->isPortCountResizable())
		{
			pItem->setFlag(ItemIsMovable);
		}
        m_inputPortItems.append(pItem);
    }

    foreach(Port* port, m_processor->getOutputPorts())
    {
        PortGraphicsItem *pItem = new PortGraphicsItem(port, this);
        QObject::connect(this, SIGNAL(selectionChanged(bool)),
                pItem, SLOT(processorSelectionChanged(bool)));
        m_outputPortItems.append(pItem);
    }
}

void ProcessorGraphicsBlock::init()
{
    createPortItems();
    layoutItems();
	if (scene())
	{
		this->setPos(QPointF(m_pos.m_x, m_pos.m_y));
	}
}

bool ProcessorGraphicsBlock::connect(PortGraphicsItem *inputPort, PortGraphicsItem *outputPort)
{
    return m_processor->space()->connectPort(inputPort->getPort(), outputPort->getPort());
}

PortGraphicsItem *ProcessorGraphicsBlock::getPortGraphicsItem(Port *port)
{
    foreach(PortGraphicsItem *inputPort, m_inputPortItems)
    {
        if(inputPort->getPort() == port)
        {
            return inputPort;
        }
    }
    foreach(PortGraphicsItem *outputPort, m_outputPortItems)
    {
        if(outputPort->getPort() == port)
        {
            return outputPort;
        }
    }
    return 0;

}

void ProcessorGraphicsBlock::propertyRemoving(Property *prop)
{
}

void ProcessorGraphicsBlock::propertyRemoved(Property *prop)
{
}

void ProcessorGraphicsBlock::detach()
{
    foreach(PortGraphicsItem*pItem, m_inputPortItems)
    {
        pItem->removeAllConnection();
    }
    foreach(PortGraphicsItem*pItem, m_outputPortItems)
    {
        pItem->removeAllConnection();
    }
}

void ProcessorGraphicsBlock::stateChanged(Port *port)
{
    foreach(PortGraphicsItem*pItem, m_inputPortItems)
    {
        if(pItem->getPort() == port)
        {
            pItem->update();
            return;
        }
    }
    foreach(PortGraphicsItem*pItem, m_outputPortItems)
    {
        if(pItem->getPort() == port)
        {
            pItem->update();
            return;
        }
    }
}

QVariant ProcessorGraphicsBlock::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemSelectedHasChanged)
    {
        if(value.toBool() == false)
        {
            emit selectionChanged(false);
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

int ProcessorGraphicsBlock::type() const
{
    return ProcessorGraphicsBlock::Type;
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
	QFont font("Time", 10);
	m_titleItem->setPos(boundingRect().left() + ITEMSPACING, boundingRect().top() + ITEMSPACING);
	QRectF rectF = boundingRect();
	QRectF mainReg = mainRect();
	painter->setPen(Qt::NoPen);
	QRectF rectShadow = rectF;
	rectShadow.translate(5, 5);
	painter->fillRect(rectShadow, qApp->palette().shadow().color());

	painter->fillRect(rectF, QColor(102, 175,253));
	if(isSelected())
	{
		painter->save();
		painter->setPen(QColor(255, 255, 255));
		QRectF rect = boundingRect();
		rect.adjust(2, 2, -2, -2);
		painter->drawRect(rect);

		painter->setPen(QPen(Qt::darkBlue, 2));
		rect = boundingRect();
		painter->drawRect(rect);
		painter->restore();
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Emphasize);
	}
	else
	{
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Normal);
	}
	painter->save();
	painter->setPen(QPen(Qt::white, 1));
	painter->drawLine(mainReg.topLeft(), mainReg.topRight());
	int index = m_processor->id();
	QFontMetrics fm(font);
	QRectF indexRect = fm.boundingRect(tr("%1").arg(index));
	qreal tempWidth = qMax(indexRect.width(), indexRect.height());
	QRectF e(mainReg.right() - tempWidth - ITEMSPACING, mainReg.bottom(), 
		tempWidth, tempWidth);
	painter->setBrush(Qt::black);
	painter->drawEllipse(e);
	painter->drawText(e, Qt::AlignCenter, tr("%1").arg(index));
	painter->setRenderHints(QPainter::Antialiasing);
	painter->restore();
}

// void ProcessorGraphicsBlock::setTitle( const QString &title )
// {
// 	QString strTitle = "<span style=\"font-size:10pt;color:white\"> %1 </span> <span style=\"font-size:7pt; color:white\">ID:%2</span>";
// 	m_titleItem->setHtml(strTitle.arg(title).arg(id()));
// 	m_titleItem->setBlockName(title);
// 	setLayoutDirty(true);
// }

void ProcessorGraphicsBlock::relayout()
{
	layoutItems();
	update();
}

QRectF ProcessorGraphicsBlock::mainRect() const
{
	QSizeF size = mainSize();
	return QRectF(-size.width()/ 2, -size.height()/2, size.width(), size.height());
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
//	m_processor->deserialize(s);
	
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

QSizeF ProcessorGraphicsBlock::mainSize() const
{
	if (!m_layoutDirty)
	{
		return m_mainSize;
	}
	int inputPortCount = m_inputPortItems.count(); /// 输入端口个数
	if (m_addInputPortItem->isVisible())
	{
		inputPortCount++;
	}
	int outputPortCount = m_outputPortItems.count();  /// 输出端口个数
	int maxCount = qMax(inputPortCount, outputPortCount); 

	float fHeight = 0;
	/// 计算高度
	if(maxCount > 0)
	{
		fHeight = maxCount * (PortGraphicsItem::DEFAULT_PORT_HEIGHT + ITEMSPACING + 1) + ITEMSPACING ;
	}
	else
		fHeight = PortGraphicsItem::DEFAULT_PORT_HEIGHT + ITEMSPACING << 1;
	/// 计算宽度
	float fWidth = m_titleItem->boundingRect().width() + (ITEMSPACING << 1);
	fWidth += m_closeItem->boundingRect().width() * 2;
	m_mainSize.setWidth(fWidth);
	m_mainSize.setHeight(fHeight);
	m_layoutDirty = false;
	return m_mainSize;
}

void ProcessorGraphicsBlock::setLayoutDirty( const bool &dirty /*= true*/ )
{
	m_layoutDirty = dirty;
}

void ProcessorGraphicsBlock::onRemovePort()
{
	PortGraphicsItem *item = qobject_cast<PortGraphicsItem*>(sender());
	if (item)
	{
		m_inputPortItems.removeOne(item);
	}
}

void ProcessorGraphicsBlock::removePort( PortGraphicsItem* port )
{
	port->removeAllConnection();
	m_inputPortItems.removeAll(port);
	m_processor->removePort(port->getPort());
	port->deleteLater();
	setLayoutDirty(true);
	relayout();
}

void ProcessorGraphicsBlock::onAddPort()
{
	blockSignals(true);
	QString portName = "Port %1";
	Port *port = new Port(new MatrixData(0), Port::IN_PORT, portName.arg(m_processor->getInputPorts().count()), this);
	m_processor->addPort(port);
	PortGraphicsItem *pItem = new PortGraphicsItem(port,this);
	if (m_processor->isPortCountResizable())
	{
		pItem->setFlag(ItemIsMovable);
	}

	QObject::connect(this, SIGNAL(selectionChanged(bool)),
		pItem, SLOT(processorSelectionChanged(bool)));

	m_inputPortItems.append(pItem);
	pItem->installSceneEventFilter(this);
	blockSignals(false);
	setLayoutDirty(true);
	relayout();
}

void ProcessorGraphicsBlock::showStatus( const QString &msg )
{
	m_statusBlock->showText(msg);
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

void ProcessorGraphicsBlock::setPortCountResizable( const bool &bResizable /*= true*/ )
{
	m_addInputPortItem->animateShow(bResizable);
	m_addInputPortItem->setVisible(bResizable);
}

bool ProcessorGraphicsBlock::sceneEventFilter( QGraphicsItem * watched, QEvent * event )
{
	if (event->type() == QEvent::GraphicsSceneMouseRelease)
	{
		PortGraphicsItem *item = (PortGraphicsItem *)watched->toGraphicsObject();
		QGraphicsSceneMouseEvent * mevent 
			= dynamic_cast<QGraphicsSceneMouseEvent*>(event);
		QPointF point = mevent->scenePos();
		if (!this->sceneBoundingRect().contains(point))
		{
			removePort(item);
			return true;
		}
		else
		{
			relayout();
		}
	}
	return false;
}

void ProcessorGraphicsBlock::startDeserialize(Utils::XmlDeserializer& s)
{
	deserialize(s);
}

void ProcessorGraphicsBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent *event )
{
	event->accept();
}


}
