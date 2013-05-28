#include "designview.h"
#include "designnetbase/designnetspace.h"
#include "graphicsitem/portarrowlinkitem.h"
#include "designnetbase/port.h"

#include "designnetbase/designnetspace.h"
#include "designnetbase/processorfactory.h"
#include "graphicsitem/portgraphicsitem.h"

#include "utils/totemassert.h"
#include "designnetconstants.h"
#include "ProcessorGraphicsBlock.h"

#include "coreplugin/actionmanager/actionmanager.h"
#include <QGLWidget>
#include <QWheelEvent>
#include <QtCore/qmath.h>
#include <QKeyEvent>

#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDropEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QGraphicsTextItem>
#include <QMimeData>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>

using namespace Core;
namespace DesignNet{

DesignView::DesignView(DesignNetSpace *space, QWidget *parent) :
    QGraphicsView(parent),
    m_designnetSpace(space),
	m_currentArrowLinkItem(0),
	m_pressedPort(0)
{
    setObjectName(QLatin1String("DesignView"));
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setBackgroundBrush(Qt::gray);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setDragMode(QGraphicsView::RubberBandDrag);
    setScene(new QGraphicsScene(this));
    setAcceptDrops(true);
    setMinimumSize(QSize(200, 200));
    setSceneRect(-500, -500, 1000, 1000);
	if (m_designnetSpace)
	{
		connect(m_designnetSpace, SIGNAL(connectionAdded(Port*,Port*)),
			this, SLOT(onConnectionAdded(Port*,Port*)));
		connect(m_designnetSpace, SIGNAL(connectionRemoved(Port*,Port*)),
			this, SLOT(onConnectionRemoved(Port*,Port*)));
		connect(m_designnetSpace, SIGNAL(processorAdded(Processor*)), this, SLOT(onProcessorAdded(Processor*)));
		connect(m_designnetSpace, SIGNAL(loadFinished()), this, SLOT(reloadSpace()));

	}
}

void DesignView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat(Constants::MIME_TYPE_TOOLITEM))
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void DesignView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QGraphicsView::dragLeaveEvent(event);
}

void DesignView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat(Constants::MIME_TYPE_TOOLITEM))
    {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
/**
 * @brief DesignView::dropEvent
 *
 * 当发生drop事件时调用，从MimeData中获取Processor的Name,内部创建Processor
 * \sa ProcessorFactory ，并在scene中创建相应的Item
 * @param event
 */
void DesignView::dropEvent(QDropEvent *event)
{
    if(!event->mimeData()->hasFormat(Constants::MIME_TYPE_TOOLITEM))
        return;

    QString processorName = QString::fromAscii(event->mimeData()->data(Constants::MIME_TYPE_TOOLITEM).data());
    Processor *processor = ProcessorFactory::instance()->create(m_designnetSpace, processorName);

	ProcessorGraphicsBlock *pBlock = (ProcessorGraphicsBlock *)processor;
    m_designnetSpace->addProcessor(processor);
    QPointF point = mapToScene(event->pos());

    pBlock->setPos(point);
    scene()->clearSelection();
    pBlock->setSelected(true);
    QGraphicsView::dropEvent(event);
}

void DesignView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_pressedPort && m_pressedPort->getPort()->portType() == Port::OUT_PORT)
	{
		if(QLineF(mapToScene(event->pos()), m_pressedPort->scenePos())
			.length() < QApplication::startDragDistance())
			QGraphicsView::mouseMoveEvent(event);
		else
		{
			if (!m_currentArrowLinkItem)
			{
				m_currentArrowLinkItem = new PortArrowLinkItem(m_pressedPort, 0);
				m_currentArrowLinkItem->blockSignals(true);
				m_currentArrowLinkItem->setEndPoint(mapToScene(event->pos()));
				scene()->addItem(m_currentArrowLinkItem);
				m_currentArrowLinkItem->blockSignals(false);
			}
			else
			{
				m_currentArrowLinkItem->blockSignals(true);
				m_currentArrowLinkItem->setEndPoint(mapToScene(event->pos()));
				m_currentArrowLinkItem->blockSignals(false);
			}
		}
		return ;
	}

    QGraphicsView::mouseMoveEvent(event);
}

void DesignView::mousePressEvent(QMouseEvent *event)
{
	QGraphicsItem *item = scene()->itemAt(mapToScene(event->pos()));
	PortGraphicsItem *port = qgraphicsitem_cast<PortGraphicsItem*>(item);
	if (port)
	{
		m_pressedPort = port;
	}
    QGraphicsView::mousePressEvent(event);
}

void DesignView::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	if (m_pressedPort)
	{
		if (m_pressedPort->getPort()->portType() == Port::OUT_PORT)
		{
			if (m_currentArrowLinkItem)
			{
				delete m_currentArrowLinkItem;
				m_currentArrowLinkItem = 0;
				QGraphicsItem *item = scene()->itemAt(mapToScene(event->pos()));
				PortGraphicsItem *target = qgraphicsitem_cast<PortGraphicsItem*>(item);

				if (target)
				{
					m_designnetSpace->connectPort(target->getPort(), m_pressedPort->getPort());
				}
			}
			
		}
		else
		{
			ProcessorGraphicsBlock* block = (ProcessorGraphicsBlock*)m_pressedPort->processor();
			QRectF portRect = m_pressedPort->sceneBoundingRect();
			QRectF processorRect = block->sceneBoundingRect();
			if (!processorRect.intersects(portRect))
			{
				blockSignals(true);
				block->removePort(m_pressedPort);
				blockSignals(false);
			}
			else
			{
				block->relayout();
			}
		}
		m_pressedPort = 0;
	}
    if(event->button() == Qt::LeftButton)
    {
        updatedSelectedItems();
    }
}

void DesignView::wheelEvent(QWheelEvent *event)
{
    float factor = qPow(2.0, event->delta() / 360.0);
    scale(factor, factor);
}

void DesignView::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_Delete)
    {
        QList<QGraphicsItem*> items = scene()->selectedItems();
        removeItems(items);
    }
    keyEvent->accept();
}

DesignNetSpace *DesignView::getSpace() const
{
    return m_designnetSpace;
}

void DesignView::removeItems(QList<QGraphicsItem *> items)
{
	scene()->blockSignals(true);
    /*!
      \todo 在某些情况下是不允许删除的
    */
    foreach(QGraphicsItem *item, items)
    {
        if(item->type() == PortArrowLinkItem::Type)
		{
            PortArrowLinkItem *portItem = qgraphicsitem_cast<PortArrowLinkItem*>(item);
            if(portItem)
                removeArrow(portItem);
			items.removeAll(item);
        }
    }
	foreach(QGraphicsItem *item, items)
    {
        if(item->type() == ProcessorGraphicsBlock::Type)
		{
            ProcessorGraphicsBlock *portItem = qgraphicsitem_cast<ProcessorGraphicsBlock*>(item);
            if(portItem)
			{
				removeProcessor(portItem);
			}
			items.removeAll(item);
        }
    }
	scene()->blockSignals(false);
}

void DesignView::contextMenuEvent(QContextMenuEvent *event)
{
//    event->accept();
    QMenu *menu = createPopupMenu();
	menu->exec(event->globalPos());
	QGraphicsView::contextMenuEvent(event);
	event->accept();
}

PortGraphicsItem *DesignView::getPortGraphicsItem(Port *port)
{
    if(m_designnetSpace->contains(port->processor()))
    {
        ProcessorGraphicsBlock *processor = (ProcessorGraphicsBlock *)(port->processor());
        return processor->getPortGraphicsItem(port);
    }
    return 0;
}

void DesignView::removeArrow(PortArrowLinkItem *arrow)
{
    m_designnetSpace->disconnectPort(arrow->getTargetPort()->getPort(),
		arrow->getSourcePort()->getPort());
}

void DesignView::updatedSelectedItems()
{
    QList<QGraphicsItem *> selectedItems = this->scene()->selectedItems();
    QList<Processor*> selectedProcessors;
    foreach(QGraphicsItem * item, selectedItems)
    {
        if(item->type() == ProcessorGraphicsBlock::Type)
        {
            ProcessorGraphicsBlock *block = qgraphicsitem_cast<ProcessorGraphicsBlock*>(item);
            selectedProcessors << block->processor();
        }
    }
    emit processorSelected(selectedProcessors);
}

void DesignView::removeProcessor(ProcessorGraphicsBlock *processor)
{
    ProcessorGraphicsBlock *block = (ProcessorGraphicsBlock *)processor;
    block->detach();
    scene()->removeItem(block);
    m_designnetSpace->removeProcessor(processor->processor());
}

void DesignView::processorClosed()
{
    ProcessorGraphicsBlock *block = qobject_cast<ProcessorGraphicsBlock *>(sender());
    removeProcessor(block);
}

void DesignView::onConnectionAdded(Port *inputPort, Port *outputPort)
{
    PortGraphicsItem *inputPortItem = getPortGraphicsItem(inputPort);
    PortGraphicsItem *outputPortItem = getPortGraphicsItem(outputPort);
    PortArrowLinkItem *linkItem = new PortArrowLinkItem(outputPortItem);
	scene()->addItem(linkItem);
	linkItem->blockSignals(true);
    linkItem->setTargetPort(inputPortItem);
	linkItem->blockSignals(false);
    
    m_arrowLinkItems.insert(outputPort, linkItem);
}

void DesignView::onConnectionRemoved(Port *inputPort, Port *outputPort)
{
    QMultiHash<Port*, PortArrowLinkItem *>::iterator itr = m_arrowLinkItems.find(outputPort);
    while(itr != m_arrowLinkItems.end() && itr.key() == outputPort)
    {
        PortArrowLinkItem* item = itr.value();
        if(item->getTargetPort()->getPort() == inputPort)
        {
            m_arrowLinkItems.remove(outputPort, itr.value());
            scene()->removeItem(item);
            delete item;
            break;
        }
        ++itr;
    }
}

DesignView::~DesignView()
{
	foreach(DesignNet::PortArrowLinkItem *item, m_arrowLinkItems.values())
	{
		delete item;
	}
	m_arrowLinkItems.clear();
}

void DesignView::onProcessorAdded( Processor *processor )
{
	ProcessorGraphicsBlock *pBlock = (ProcessorGraphicsBlock *)processor;
	scene()->addItem(pBlock);
	pBlock->setPos(QPoint(pBlock->originalPosition().m_x, pBlock->originalPosition().m_y));
	connect(pBlock, SIGNAL(closed()), this, SLOT(processorClosed()));
	emit processorAdded(processor);
}

void DesignView::setDesignNetSpace( DesignNetSpace *space )
{
	m_designnetSpace = space;
	if (m_designnetSpace)
	{
		connect(m_designnetSpace, SIGNAL(connectionAdded(Port*,Port*)),
			this, SLOT(onConnectionAdded(Port*,Port*)));
		connect(m_designnetSpace, SIGNAL(connectionRemoved(Port*,Port*)),
			this, SLOT(onConnectionRemoved(Port*,Port*)));
		connect(m_designnetSpace, SIGNAL(processorAdded(Processor*)), this, SLOT(onProcessorAdded(Processor*)));
		connect(m_designnetSpace, SIGNAL(loadFinished()), this, SLOT(reloadSpace()));

	}
}

void DesignView::reloadSpace()
{
	QGraphicsScene *mainScene = scene();
	scene()->clear();
	foreach(Processor *processor, m_designnetSpace->processors())
	{
		ProcessorGraphicsBlock *block = dynamic_cast<ProcessorGraphicsBlock*>(processor);
		mainScene->addItem(block);
		Position pos = block->originalPosition();
		block->setPos(QPointF(pos.m_x, pos.m_y));
	}
}

QMenu* DesignView::createPopupMenu()
{
	QMenu *menu = new QMenu(this);
	QList<QGraphicsItem*> items =  scene()->selectedItems();
	foreach(QGraphicsItem *item, items)
	{
		ProcessorGraphicsBlock *block = qgraphicsitem_cast<ProcessorGraphicsBlock*>(item);
		if (block)
		{
			block->createContextMenu(menu);
		}
	}
	return menu;
}

}
