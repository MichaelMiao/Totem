
#include "../designnetbase/block.h"
#include "../designview.h"
#include "../designnetbase/designnetspace.h"
#include "processorgraphicsblock.h"
#include "portgraphicsitem.h"
#include "port.h"

#include "property/property.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "designnetconstants.h"
#include "graphicsitem/blocktextitem.h"

#include <QApplication>
#include <QPainter>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include <QFont>
#include <QTimer>
using namespace GraphicsUI;
namespace DesignNet{
const int PORT_MARGIN = 15;
ProcessorGraphicsBlock::ProcessorGraphicsBlock(DesignNetSpace *space, QGraphicsItem *parent)
    : QGraphicsItem(parent), Processor(space)
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
	setCacheMode(DeviceCoordinateCache);
	///
	/// title
	m_titleItem = new BlockTextItem(this);
	///
	/// close按钮
	m_closeItem = new GraphicsAutoShowHideItem(this);
	m_closeItem->setSize(QSize(16, 16));
	m_closeItem->setPos(boundingRect().right() - 16, boundingRect().top());
	m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
	QObject::connect(m_closeItem, SIGNAL(clicked()), this, SIGNAL(closed()));
	
	QTimer::singleShot(0, this, SLOT(relayout()));

    QObject::connect(this, SIGNAL(logout(QString)),
            Core::ICore::messageManager(),
            SLOT(printToOutputPanePopup(QString)));
    m_icon = QIcon(":/media/default_processor.png");
}

ProcessorGraphicsBlock::~ProcessorGraphicsBlock()
{

}

QRectF ProcessorGraphicsBlock::boundingRect() const
{
    int inputPortCount = m_inputPortItems.count();
    int outputPortCount = m_outputPortItems.count();
    int maxCount = qMax(inputPortCount, outputPortCount);

    float fHeight = 0;
    float fWidth = m_titleItem->boundingRect().width() + ITEMSPACING * 2;
    if(maxCount > 0)
    {
        fHeight = PORT_MARGIN + maxCount * PORT_MARGIN;
    }
    else
        fHeight = 2*PORT_MARGIN;
    QSizeF miniSize = minimumSizeHint();
    fHeight = qMax(fHeight,(float) miniSize.height()) + ITEMSPACING * 2;
    fWidth = qMax(fWidth, (float)miniSize.width()) + m_closeItem->boundingRect().width() * 2;
    return QRectF(-fWidth/2, -fHeight/2, fWidth, fHeight);
}

QSizeF ProcessorGraphicsBlock::minimumSizeHint() const
{
    return QSizeF(DEFAULT_WIDTH, TITLE_HEIGHT + 2 * PORT_MARGIN + 1);
}

void ProcessorGraphicsBlock::layoutItems()
{
    prepareGeometryChange();
    QRectF rectF = mainRect();
    float posX = rectF.left();
    float posY = rectF.top();
    if(m_inputPortItems.count() != 0)
    {
        foreach(PortGraphicsItem *portItem, m_inputPortItems)
        {
            posY += PORT_MARGIN;
            posX = rectF.left() + portItem->size().width() / 2;
            portItem->setPos(posX, posY);
        }
    }


    posY = rectF.top();
    if(m_outputPortItems.count() != 0)
    {
        foreach(PortGraphicsItem *portItem, m_outputPortItems)
        {
            posX = rectF.right() - portItem->size().width() / 2;
            posY += PORT_MARGIN;
            portItem->setPos(posX - portItem->size().width() / 2, posY);
        }
    }

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
        PortGraphicsItem *pItem = new PortGraphicsItem(port, this);
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

void ProcessorGraphicsBlock::initialize()
{
    createPortItems();
    layoutItems();
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
	Processor::dataArrived(port);
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
	layoutItems();
	setTitle(title());
	m_titleItem->setPos(boundingRect().left() + ITEMSPACING, boundingRect().top() + ITEMSPACING);
	QRectF rectF = boundingRect();
	rectF.adjust(2, 2, -2, -2);

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
	painter->drawLine(rectF.left(), rectF.top() + TITLE_HEIGHT,
		rectF.right(), rectF.top() + TITLE_HEIGHT);
	painter->setRenderHints(QPainter::Antialiasing);
	painter->restore();
}

void ProcessorGraphicsBlock::setTitle( const QString &title )
{
	QString strTitle = "<span style=\"font-size:10pt;color:white\"> %1 </span>";
	m_titleItem->setHtml(strTitle.arg(title));
}

void ProcessorGraphicsBlock::relayout()
{
	layoutItems();
	update();
}

QRectF ProcessorGraphicsBlock::mainRect() const
{
	QRectF rectF = boundingRect();
	return QRectF(rectF.left(), rectF.top() + TITLE_HEIGHT + 1,
		rectF.width(), rectF.height() - TITLE_HEIGHT - 1);
}

void ProcessorGraphicsBlock::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	m_closeItem->setPos(boundingRect().right() - 16, boundingRect().top());
	m_closeItem->animateShow(true);
}

void ProcessorGraphicsBlock::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	m_closeItem->animateShow(false);
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
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}


}
