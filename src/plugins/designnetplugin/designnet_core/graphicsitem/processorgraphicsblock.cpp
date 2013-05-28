
#include "../designnetbase/block.h"
#include "../designview.h"
#include "../designnetbase/designnetspace.h"
#include "../data/matrixdata.h"


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
#include <QMenu>
#include <QFont>
#include <QTimer>
using namespace GraphicsUI;
namespace DesignNet{
const int PORT_MARGIN = 15;


void Position::serialize(Utils::XmlSerializer& s) const
{
	s.serialize("X", m_x);
	s.serialize("Y", m_y);
}

void Position::deserialize(Utils::XmlDeserializer& s) 
{
	s.deserialize("X", m_x);
	s.deserialize("Y", m_y);
}
ProcessorGraphicsBlock::ProcessorGraphicsBlock(DesignNetSpace *space, QGraphicsItem *parent)
    : QGraphicsItem(parent), Processor(space)
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
	///
	/// close按钮
	m_closeItem = new GraphicsAutoShowHideItem(this);
	m_closeItem->setSize(QSize(16, 16));
	m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
	m_statusBlock = new TextAnimationBlock(this);
	m_statusBlock->setSizeF(QSizeF(STATUSBAR_HEIGHT, DEFAULT_WIDTH));
	QObject::connect(m_closeItem, SIGNAL(clicked()), this, SIGNAL(closed()));

	QTimer::singleShot(0, this, SLOT(relayout()));

	QObject::connect(this, SIGNAL(logout(QString)),
		Core::ICore::messageManager(),
		SLOT(printToOutputPanePopup(QString)));
	m_icon = QIcon(":/media/default_processor.png");
	m_block = 0;
	m_layoutDirty = true;
	setPortCountResizable(false);
}

ProcessorGraphicsBlock::~ProcessorGraphicsBlock()
{

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
    if(m_inputPortItems.count() != 0)
    {
        foreach(PortGraphicsItem *portItem, m_inputPortItems)
        {
            portItem->setPos(posX, posY);
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
        foreach(PortGraphicsItem *portItem, m_outputPortItems)
        {
            portItem->setPos(posX, posY);
			posY += PortGraphicsItem::DEFAULT_PORT_HEIGHT + ITEMSPACING;
			portItem->showTypeImage(PortGraphicsItem::LEFT);
        }
    }
	m_statusBlock->setTextColor(Qt::white);
	m_statusBlock->setPos(rectF.left(), rectF.bottom() + 1);
	m_statusBlock->setSizeF(QSizeF(rectF.width(), STATUSBAR_HEIGHT));
}

QString ProcessorGraphicsBlock::title() const
{
    return tr("General Processor");
}

QIcon ProcessorGraphicsBlock::getIcon() const
{
    return m_icon;
}
void ProcessorGraphicsBlock::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

Processor *ProcessorGraphicsBlock::processor()
{
    return (Processor *)this;
}

void ProcessorGraphicsBlock::createPortItems()
{
    foreach(Port* port, getInputPorts())
    {
        PortGraphicsItem *pItem = new PortGraphicsItem(port,this);
        if (m_portCountResizable)
        {
			pItem->setFlag(ItemIsMovable);
        }
		
		QObject::connect(this, SIGNAL(selectionChanged(bool)),
                pItem, SLOT(processorSelectionChanged(bool)));

        m_inputPortItems.append(pItem);
    }
    foreach(Port* port, getOutputPorts())
    {
        PortGraphicsItem *pItem = new PortGraphicsItem(port, this);
        QObject::connect(this, SIGNAL(selectionChanged(bool)),
                pItem, SLOT(processorSelectionChanged(bool)));
        m_outputPortItems.append(pItem);
    }
}

bool ProcessorGraphicsBlock::process()
{
    emit logout(tr("process"));
    return true;
}

void ProcessorGraphicsBlock::init()
{
    createPortItems();
    layoutItems();
	if (scene())
	{
		this->setPos(QPointF(m_pos.m_x, m_pos.m_y));
	}
	setTitle(title());
}

bool ProcessorGraphicsBlock::connect(PortGraphicsItem *inputPort, PortGraphicsItem *outputPort)
{
    m_space->connectPort(inputPort->getPort(), outputPort->getPort());
    return true;
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

Core::Id ProcessorGraphicsBlock::typeID() const
{
    return Core::Id(QString(category() + "/" + name()));
}

QString ProcessorGraphicsBlock::category() const
{
    return tr("DefaultCategory");
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

void ProcessorGraphicsBlock::dataArrived(Port *port)
{
    Q_UNUSED(port)
    PortGraphicsItem *item = getPortGraphicsItem(port);
    if(item)
        item->updateData();
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
	setTitle(title());
	m_titleItem->setPos(boundingRect().left() + ITEMSPACING, boundingRect().top() + ITEMSPACING);
	QRectF rectF = boundingRect();

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
	painter->drawLine(mainRect().topLeft(), mainRect().topRight());
	painter->drawLine(mainRect().bottomLeft(), mainRect().bottomRight());
	painter->setRenderHints(QPainter::Antialiasing);
	painter->restore();
}

void ProcessorGraphicsBlock::setTitle( const QString &title )
{
	QString strTitle = "<span style=\"font-size:10pt;color:white\"> %1 </span> <span style=\"font-size:7pt; color:white\">ID:%2</span>";
	m_titleItem->setHtml(strTitle.arg(title).arg(id()));
	setLayoutDirty(true);
}

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
	Processor::serialize(s);
	Position p(this->scenePos());
	s.serialize("Pos", p);
}

void ProcessorGraphicsBlock::deserialize( Utils::XmlDeserializer& s )
{
	Processor::deserialize(s);
	
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

void ProcessorGraphicsBlock::setPortCountResizable( const bool &bResizable /*= true*/ )
{
	m_portCountResizable = bResizable;
	m_addInputPortItem->animateShow(bResizable);
	m_addInputPortItem->setVisible(bResizable);
	
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
	m_inputPorts.removeOne(port->getPort());
	delete port;
	setLayoutDirty(true);
	relayout();
}

void ProcessorGraphicsBlock::onAddPort()
{
	blockSignals(true);
	QString portName = "Port %1";
	Port *port = new Port(new MatrixData(0), Port::IN_PORT, portName.arg(m_inputPorts.count()), this);
	addPort(port);
	PortGraphicsItem *pItem = new PortGraphicsItem(port,this);
	if (m_portCountResizable)
	{
		pItem->setFlag(ItemIsMovable);
	}

	QObject::connect(this, SIGNAL(selectionChanged(bool)),
		pItem, SLOT(processorSelectionChanged(bool)));

	m_inputPortItems.append(pItem);
	blockSignals(false);
	setLayoutDirty(true);
	relayout();
}

void ProcessorGraphicsBlock::showStatus( const QString &msg )
{
	m_statusBlock->showText(msg);
}

bool ProcessorGraphicsBlock::beforeProcess()
{
//	m_statusBlock->showText("beforeProcess");
	return Processor::beforeProcess();
}

void ProcessorGraphicsBlock::afterProcess( bool status /*= true*/ )
{
//	m_statusBlock->showText("afterProcess");
	Processor::afterProcess();
}


}
