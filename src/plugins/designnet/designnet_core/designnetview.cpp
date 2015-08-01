#include "designnetview.h"
#include "designnetspace.h"
#include "designnetbase/processorfactory.h"
#include "designnetbase/processor.h"
#include "designnetbase/port.h"
#include "designnetconstants.h"
#include "graphicsitem/processorgraphicsblock.h"
#include "graphicsitem/portarrowlinkitem.h"
#include "graphicsitem/portgraphicsitem.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/totemassert.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

#include <QDropEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QMultiHash>
#include <QApplication>
#include <QMimeData>
namespace DesignNet{
class DesignNetViewPrivate
{
public:
	DesignNetViewPrivate();
	~DesignNetViewPrivate();
	DesignNetSpace*				m_designnetSpace;
	PortArrowLinkItem*			m_currentArrowLinkItem;
	QMultiHash<PortGraphicsItem*, PortArrowLinkItem*> m_arrowLinkItems;
	QList<ProcessorGraphicsBlock*> m_processors;
	PortGraphicsItem*			m_pressedPort;//!< 鼠标在该Port上按下
};

DesignNetViewPrivate::DesignNetViewPrivate()
{
	m_designnetSpace = 0;
	m_currentArrowLinkItem = 0;
	m_pressedPort	= 0;
}

DesignNetViewPrivate::~DesignNetViewPrivate()
{
	foreach(DesignNet::PortArrowLinkItem *item, m_arrowLinkItems.values())
	{
		delete item;
	}
	m_arrowLinkItems.clear();
}


DesignNetView::DesignNetView(DesignNetSpace *space, QWidget *parent)
	: QGraphicsView(parent),
	d(new DesignNetViewPrivate)
{
	d->m_designnetSpace = space;
	d->m_currentArrowLinkItem = 0;
	m_bLinking = false;
	setObjectName(QLatin1String("DesignView"));
	setCacheMode(CacheBackground);
	setRenderHint(QPainter::Antialiasing);
	setViewportUpdateMode(FullViewportUpdate);
	setSceneRect(-1000, -1000, 2000, 2000);
	viewport()->setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	setDragMode(QGraphicsView::RubberBandDrag);
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setBackgroundBrush(Qt::gray);
	setScene(scene);
	setAcceptDrops(true);
	QObject::connect(space, SIGNAL(logout(QString)),
		this, SLOT(OnShowMessage(QString)));
}

DesignNetView::~DesignNetView()
{
	delete d;
}

void DesignNetView::dragLeaveEvent(QDragLeaveEvent *event)
{
	QGraphicsView::dragLeaveEvent(event);
}

void DesignNetView::dragMoveEvent(QDragMoveEvent *event)
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
void DesignNetView::dropEvent( QDropEvent * event )
{
	if(!event->mimeData()->hasFormat(Constants::MIME_TYPE_TOOLITEM))
		return;

	QString processorName = QString::fromLatin1(event->mimeData()->data(Constants::MIME_TYPE_TOOLITEM).data());
	Processor *processor = ProcessorFactory::instance()->create(d->m_designnetSpace, processorName);
	d->m_designnetSpace->addProcessor(processor);
	
	QPointF point = mapToScene(event->pos());
	ProcessorGraphicsBlock *pBlock = new ProcessorGraphicsBlock(processor, scene(), point, 0);
	addProcessor(pBlock);
	scene()->clearSelection();
	pBlock->setSelected(true);
	QGraphicsView::dropEvent(event);
}

void DesignNetView::dragEnterEvent( QDragEnterEvent * event )
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

void DesignNetView::setDesignNetSpace( DesignNetSpace *space )
{
	d->m_designnetSpace = space;
}

DesignNetSpace* DesignNetView::getSpace() const
{
	return d->m_designnetSpace;
}

void DesignNetView::processorClosed()
{
	ProcessorGraphicsBlock *block = qobject_cast<ProcessorGraphicsBlock*>(sender());
	removeProcessor(block);
}

void DesignNetView::keyReleaseEvent( QKeyEvent *keyEvent )
{
	if(keyEvent->key() == Qt::Key_Delete)
	{
		QList<QGraphicsItem*> items = scene()->selectedItems();
		removeItems(items);
	}
	keyEvent->accept();
}

void DesignNetView::removeItems( QList<QGraphicsItem*> items )
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

void DesignNetView::mouseMoveEvent( QMouseEvent * event )
{
	if (d->m_pressedPort && d->m_pressedPort->getPort()->portType() == Port::OUT_PORT)
	{
		if(QLineF(mapToScene(event->pos()), d->m_pressedPort->scenePos())
			.length() < QApplication::startDragDistance())
			QGraphicsView::mouseMoveEvent(event);
		else
		{
			if (!d->m_currentArrowLinkItem)
			{
				d->m_currentArrowLinkItem = new PortArrowLinkItem(d->m_pressedPort, 0);
				d->m_currentArrowLinkItem->blockSignals(true);
				d->m_currentArrowLinkItem->setEndPoint(mapToScene(event->pos()));
				scene()->addItem(d->m_currentArrowLinkItem);
				d->m_currentArrowLinkItem->blockSignals(false);
			}
			else
			{
				d->m_currentArrowLinkItem->blockSignals(true);
				d->m_currentArrowLinkItem->setEndPoint(mapToScene(event->pos()));
				d->m_currentArrowLinkItem->blockSignals(false);
			}
		}
		return ;
	}
	QGraphicsView::mouseMoveEvent(event);
}

void DesignNetView::mousePressEvent( QMouseEvent * event )
{
	QGraphicsItem *item = itemAt(mapToScene(event->pos()).toPoint());
	PortGraphicsItem *port = qgraphicsitem_cast<PortGraphicsItem*>(item);
	if (port)
	{
		d->m_pressedPort = port;
	}
	QGraphicsView::mousePressEvent(event);
}

void DesignNetView::mouseReleaseEvent( QMouseEvent * event )
{
	event->accept();
	QGraphicsView::mouseReleaseEvent(event);

	if (d->m_pressedPort)
	{
		if (d->m_pressedPort->getPort()->portType() == Port::OUT_PORT)
		{
			if (d->m_currentArrowLinkItem)
			{
				delete d->m_currentArrowLinkItem;
				d->m_currentArrowLinkItem = 0;
				QGraphicsItem *item = itemAt(mapToScene(event->pos()).toPoint());
				PortGraphicsItem *target = qgraphicsitem_cast<PortGraphicsItem*>(item);

				if (target)
				{
					if(d->m_designnetSpace->connectPort(target->getPort(), d->m_pressedPort->getPort()))
						addArrow(d->m_pressedPort, target);
				}
			}

		}
		d->m_pressedPort = 0;
	}
}

void DesignNetView::addArrow( PortGraphicsItem* source, PortGraphicsItem * target )
{
	PortArrowLinkItem *linkItem = new PortArrowLinkItem(source);
	scene()->addItem(linkItem);
	linkItem->blockSignals(true);
	linkItem->setTargetPort(target);
	d->m_arrowLinkItems.insert(source, linkItem);
	linkItem->blockSignals(false);
}

void DesignNetView::addProcessor( ProcessorGraphicsBlock *processor )
{
	d->m_processors.append(processor);
	QObject::connect(processor, SIGNAL(closed()), this, SLOT(processorClosed()));
}

void DesignNetView::removeArrow( PortArrowLinkItem* arrow )
{
	PortGraphicsItem* sourcePort = arrow->getSourcePort();
	d->m_arrowLinkItems.remove(sourcePort, arrow);
	d->m_designnetSpace->disconnectPort(arrow->getTargetPort()->getPort(), sourcePort->getPort());
 	delete arrow;
}

void DesignNetView::removeArrow( PortGraphicsItem* source, PortGraphicsItem * target )
{
	QMultiHash<PortGraphicsItem*, PortArrowLinkItem *>::iterator itr = d->m_arrowLinkItems.find(source);
	while(itr != d->m_arrowLinkItems.end() && itr.key() == source)
	{
		PortArrowLinkItem* item = itr.value();
		if(item->getTargetPort() == target)
		{
			removeArrow(item);
			break;
		}
		++itr;
	}
}

void DesignNetView::removeArrow( const int& src_processor_id, const QString &src_port_name,
	const int& target_processor_id, const QString &target_port_name )
{
	ProcessorGraphicsBlock * src_processor = getGraphicsProcessor(src_processor_id);
	ProcessorGraphicsBlock * target_processor = getGraphicsProcessor(target_processor_id);
	TOTEM_ASSERT(src_processor!=0 && target_processor!=0, return);
	Port* src = src_processor->processor()->getPort(src_port_name);
	Port* target = target_processor->processor()->getPort(target_port_name);
	removeArrow(src_processor->getPortGraphicsItem(src), target_processor->getPortGraphicsItem(target));
}

void DesignNetView::removeProcessor( ProcessorGraphicsBlock *item )
{
	if (item)
	{
		Processor *processor = item->processor();
		if (!processor->isRunning())
		{	
			/// 移除所有连接
			foreach(Port* src, processor->getOutputPorts())
			{
				foreach(Port*target, src->connectedPorts())
				{
					removeArrow(src->processor()->id(), src->name(), 
						target->processor()->id(), target->name());	
					d->m_designnetSpace->disconnectPort(target, src);
				}
			}
			foreach(Port* target, processor->getInputPorts())
			{
				foreach(Port*src, target->connectedPorts())
				{
					removeArrow(src->processor()->id(), src->name(), 
						target->processor()->id(), target->name());
					d->m_designnetSpace->disconnectPort(target, src);
				}
			}
			scene()->removeItem(item);
			d->m_designnetSpace->removeProcessor(processor);
		}
		
	}
}

void DesignNetView::reloadSpace()
{

}

ProcessorGraphicsBlock * DesignNetView::getGraphicsProcessor( const int &id )
{
	foreach(ProcessorGraphicsBlock *p, d->m_processors)
	{
		if(p->processor()->id() == id)
			return p;
	}
	return 0;
}

void DesignNetView::serialize( Utils::XmlSerializer &s )
{
	QList<Position> positions;
	foreach(ProcessorGraphicsBlock *p, d->m_processors)
	{
		Position pos;
		pos.m_id	= p->processor()->id();
		pos.m_x		= p->scenePos().x();
		pos.m_y		= p->scenePos().y();
		positions.push_back(pos);
	}
	s.serialize(_T("Positions"), positions, _T("Position"));
}

void DesignNetView::deserialize( Utils::XmlDeserializer &x )
{
	QList<Position> positions;
	x.deserializeCollection(_T("Positions"), positions, _T("Position"));

	foreach(Processor *p, d->m_designnetSpace->processors())
	{
		int i = -1;
		foreach(Position pos, positions)
		{
			i++;
			if (pos.m_id == p->id())
			{
				positions.removeAt(i);
				ProcessorGraphicsBlock *pBlock = new ProcessorGraphicsBlock(p, scene(), 
					QPointF((qreal)pos.m_x, (qreal)pos.m_y), 0);
				addProcessor(pBlock);
				break;
			}
		}
	}

	// 添加链接
	foreach(Processor *p, d->m_designnetSpace->processors())
	{
		foreach(Port* srcport, p->getOutputPorts())
		{
			QList<Port*> linkedPorts = srcport->connectedPorts();
			foreach(Port* linkedPort, linkedPorts)
			{
				ProcessorGraphicsBlock * src_processor = getGraphicsProcessor(srcport->processor()->id());
				ProcessorGraphicsBlock * target_processor = getGraphicsProcessor(linkedPort->processor()->id());
				TOTEM_ASSERT(src_processor!=0 && target_processor!=0, return);
				addArrow(src_processor->getPortGraphicsItem(srcport), target_processor->getPortGraphicsItem(linkedPort));

			}
		}
	}
}

void DesignNetView::OnShowMessage( const QString &strMessage )
{
	Core::ICore::messageManager()->printToOutputPanePopup(sender()->objectName() + ": " + strMessage);
}

}
