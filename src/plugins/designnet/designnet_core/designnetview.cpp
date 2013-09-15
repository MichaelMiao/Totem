#include "designnetview.h"
#include "designnetspace.h"
#include "designnetbase/processorfactory.h"
#include "designnetbase/processor.h"
#include "designnetconstants.h"
#include "graphicsitem/processorgraphicsblock.h"
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
namespace DesignNet{
class DesignNetViewPrivate
{
public:
	DesignNetViewPrivate();
	~DesignNetViewPrivate();
	DesignNetSpace*								m_designnetSpace;
	QMap<Processor*, ProcessorGraphicsBlock*>	m_processorMaps;
};

DesignNetViewPrivate::DesignNetViewPrivate()
{
	m_designnetSpace = 0;
}

DesignNetViewPrivate::~DesignNetViewPrivate()
{
}


DesignNetView::DesignNetView(DesignNetSpace *space, QWidget *parent)
	: QGraphicsView(parent),
	d(new DesignNetViewPrivate)
{
	d->m_designnetSpace = space;
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
	if (space)
	{
		QObject::connect(space, SIGNAL(logout(QString)), this, SLOT(OnShowMessage(QString)));
		QObject::connect(space, SIGNAL(processorAdded(Processor*)), this, SLOT(onProcessorAdded(Processor*)));
		QObject::connect(space, SIGNAL(processorRemoved(Processor*)), this, SLOT(onProcessorRemoved(Processor*)));
	}
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
	QPointF point = mapToScene(event->pos());

	QString processorName = QString::fromAscii(event->mimeData()->data(Constants::MIME_TYPE_TOOLITEM).data());
	Processor *processor = ProcessorFactory::instance()->create(d->m_designnetSpace, processorName);
	d->m_designnetSpace->addProcessor(processor, true);
	
	scene()->clearSelection();
	ProcessorGraphicsBlock* pBlock = d->m_processorMaps[processor];
	if (pBlock)
	{
		pBlock->setSelected(true);
		pBlock->setPos(point);
	}
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
	if (d->m_designnetSpace)
	{
		d->m_designnetSpace->removeProcessor(0);
		d->m_designnetSpace->QObject::disconnect();
	}
	d->m_designnetSpace = space;
	if (space)
	{
		QObject::connect(space, SIGNAL(logout(QString)), this, SLOT(OnShowMessage(QString)));
		QObject::connect(space, SIGNAL(processorAdded(Processor*)), this, SLOT(onProcessorAdded(Processor*)));
		QObject::connect(space, SIGNAL(processorRemoved(Processor*)), this, SLOT(onProcessorRemoved(Processor*)));
	}
}

DesignNetSpace* DesignNetView::getSpace() const
{
	return d->m_designnetSpace;
}

void DesignNetView::processorClosed()
{
	ProcessorGraphicsBlock *block = qobject_cast<ProcessorGraphicsBlock*>(sender());
	removeProcessor(block);
	delete block;
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

    }
	foreach(QGraphicsItem *item, items)
    {
        if(item->type() == ProcessorGraphicsBlock::Type)
		{
            ProcessorGraphicsBlock *portItem = qgraphicsitem_cast<ProcessorGraphicsBlock*>(item);
            if(portItem)
				removeProcessor(portItem);
		
			items.removeAll(item);
        }
    }
	scene()->blockSignals(false);
}

void DesignNetView::mouseMoveEvent( QMouseEvent * event )
{
	QGraphicsView::mouseMoveEvent(event);
}

void DesignNetView::mousePressEvent( QMouseEvent * event )
{
	QGraphicsView::mousePressEvent(event);
}

void DesignNetView::mouseReleaseEvent( QMouseEvent * event )
{
	event->accept();
	QGraphicsView::mouseReleaseEvent(event);
}

void DesignNetView::addProcessor( ProcessorGraphicsBlock *processor )
{
	d->m_processorMaps[processor->processor()] = processor;
	QObject::connect(processor, SIGNAL(closed()), this, SLOT(processorClosed()));
}

void DesignNetView::removeProcessor( ProcessorGraphicsBlock *item )
{
	if (item)
	{
		Processor *processor = item->processor();
		if (!processor->isRunning())
		{	
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
	foreach(Processor *p, d->m_processorMaps.keys())
	{
		if(p->id() == id)
			return d->m_processorMaps[p];
	}
	return 0;
}

void DesignNetView::serialize( Utils::XmlSerializer &s )
{
	QList<Position> positions;
	QMap<Processor*, ProcessorGraphicsBlock*>::iterator itr = d->m_processorMaps.begin();

	for(; itr != d->m_processorMaps.end(); itr++)
	{
		ProcessorGraphicsBlock *p = itr.value();
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

}

void DesignNetView::OnShowMessage( const QString &strMessage )
{
	Core::ICore::messageManager()->printToOutputPanePopup(sender()->objectName() + ": " + strMessage);
}

void DesignNetView::onProcessorAdded( Processor* processor )
{
	ProcessorGraphicsBlock *pBlock = new ProcessorGraphicsBlock(processor, scene());
	addProcessor(pBlock);
}

void DesignNetView::onProcessorRemoved( Processor* processor )
{

}

}
