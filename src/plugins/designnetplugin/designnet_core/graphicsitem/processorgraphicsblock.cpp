#include "processorgraphicsblock.h"
#include "portgraphicsitem.h"
#include "port.h"
#include "../designview.h"
#include "../designnetbase/designnetspace.h"
#include "property/property.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

namespace DesignNet{
const int PORT_MARGIN = 15;
ProcessorGraphicsBlock::ProcessorGraphicsBlock(DesignNetSpace *space, QGraphicsItem *parent)
    : GraphicsBlock(DEFAULT_WIDTH, DEFAULT_HEIGHT, parent), Processor(space)
{
    GraphicsBlock::connect(this, SIGNAL(logout(QString)),
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

bool ProcessorGraphicsBlock::process(QFutureInterface<bool> &fi)
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
}

void ProcessorGraphicsBlock::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    GraphicsBlock::mouseMoveEvent(event);
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
    return GraphicsBlock::itemChange(change,value);
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


}
