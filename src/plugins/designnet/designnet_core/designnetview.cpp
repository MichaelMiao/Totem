#include "designnetview.h"
#include "designnetspace.h"
#include "designnetbase/processorfactory.h"
#include "designnetbase/processor.h"
#include "designnetconstants.h"
#include "graphicsitem/processorgraphicsblock.h"
#include "graphicsitem/processorarrowlink.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/totemassert.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
#include "graphicsitem/blocktextitem.h"
#include <QDropEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QMultiHash>
#include <QApplication>
#include <QGraphicsLineItem>
namespace DesignNet{
class DesignNetViewPrivate
{
public:
	DesignNetViewPrivate();
	~DesignNetViewPrivate();
	DesignNetSpace*								m_designnetSpace;
	QMap<Processor*, ProcessorGraphicsBlock*>	m_processorMaps;
	ProcessorGraphicsBlock*						m_tempProcessor;
	ProcessorGraphicsBlock*						m_srcProcessor;
	QGraphicsLineItem*							m_lineItem;
};

DesignNetViewPrivate::DesignNetViewPrivate()
{
	m_designnetSpace = 0;
	m_tempProcessor = 0;
	m_lineItem = 0;
	m_srcProcessor = 0;
}

DesignNetViewPrivate::~DesignNetViewPrivate()
{
}


DesignNetView::DesignNetView(DesignNetSpace *space, QWidget *parent)
	: QGraphicsView(parent),
	d(new DesignNetViewPrivate)
{
	d->m_designnetSpace = space;
	d->m_lineItem = new QGraphicsLineItem(0);
	QPen pen(QBrush(QColor(255, 20, 0, 180)), 3, Qt::DotLine);
	d->m_lineItem->setPen(pen);
	m_bLinking = false;
	m_bPressed = false;
	setObjectName(QLatin1String("DesignView"));
 	setRenderHint(QPainter::Antialiasing);
	setSceneRect(-1000, -1000, 2000, 2000);
	viewport()->setMouseTracking(true);
 	setFocusPolicy(Qt::StrongFocus);
 	setDragMode(QGraphicsView::RubberBandDrag);
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setBackgroundBrush(Qt::gray);
	setScene(scene);
	scene->addItem(d->m_lineItem);
	setAcceptDrops(true);
	if (space)
	{
		QObject::connect(space, SIGNAL(logout(QString)), this, SLOT(OnShowMessage(QString)));
		QObject::connect(space, SIGNAL(processorAdded(Processor*)), this, SLOT(onProcessorAdded(Processor*)));
		QObject::connect(space, SIGNAL(processorRemoved(Processor*)), this, SLOT(onProcessorRemoved(Processor*)));
		QObject::connect(space, SIGNAL(connectionAdded(Processor*, Processor*)), this, SLOT(onConnectionAdded(Processor*, Processor*)));
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
	Processor *processor = ProcessorFactory::instance()->create(0, processorName);
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
		QObject::connect(space, SIGNAL(connectionAdded(Processor*, Processor*)), this, SLOT(onConnectionAdded(Processor*, Processor*)));
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
	else if (keyEvent->key() == Qt::Key_Escape)
	{
		setEditState(EditState_Move);
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

void DesignNetView::mousePressEvent( QMouseEvent * event )
{
	qDebug() << "mouse press";
	QPointF scenePos = mapToScene(event->pos());
	QGraphicsItem *pItem = scene()->itemAt(scenePos);
	if (!pItem || pItem->type() != (QGraphicsItem::UserType + ProcessorGraphicsBlockType))
	{
		event->accept();
	}
	else if(m_eEditState == EditState_Link)
	{
		m_bPressed = true;
		d->m_srcProcessor = (ProcessorGraphicsBlock*)pItem;
		d->m_srcProcessor->setHover(true);
		d->m_lineItem->setVisible(true);
		d->m_lineItem->setLine(scenePos.x(), scenePos.y(), scenePos.x(), scenePos.y());
	}

	QGraphicsView::mousePressEvent(event);
}

void DesignNetView::mouseMoveEvent( QMouseEvent * event )
{
	qDebug() << "mouse move";
	if (m_eEditState == EditState_Link && m_bPressed)
	{
		QPointF scenePos = mapToScene(event->pos());
		QPointF p = d->m_srcProcessor->scenePos();
		QPointF posSrc = d->m_srcProcessor->getCrossPoint(QLineF(p, scenePos));
		d->m_lineItem->setLine(p.x(), p.y(), scenePos.x(), scenePos.y());

		QGraphicsItem *pItem = scene()->itemAt(scenePos);
		if (!pItem || pItem->type() != (QGraphicsItem::UserType + ProcessorGraphicsBlockType))
		{
			if (d->m_tempProcessor && d->m_tempProcessor != d->m_srcProcessor)
				d->m_tempProcessor->setHover(false);
			d->m_tempProcessor = 0;
			return;
		}
		
		d->m_tempProcessor = (ProcessorGraphicsBlock*)pItem;
		d->m_tempProcessor->setEmphasized(true);
	}
	QGraphicsView::mouseMoveEvent(event);
}

void DesignNetView::mouseReleaseEvent( QMouseEvent * event )
{
	if (m_eEditState == EditState_Link && m_bPressed)
	{
		if (d->m_tempProcessor && d->m_tempProcessor != d->m_srcProcessor)
		{
			d->m_tempProcessor->setHover(true);
			d->m_designnetSpace->connectProcessor(d->m_srcProcessor->processor(), d->m_tempProcessor->processor());
		}
	}
	else if (m_eEditState == EditState_Move)
	{
		QPointF scenePos = mapToScene(event->pos());
		QGraphicsItem *pItem = scene()->itemAt(scenePos);
		if (pItem && pItem->type() == (QGraphicsItem::UserType + ProcessorGraphicsBlockType))
			emit showAvailiableData(((ProcessorGraphicsBlock*)pItem)->processor());
	}
	if (m_bPressed)
	{
		m_bPressed = false;
		d->m_tempProcessor = 0;
		d->m_lineItem->setVisible(false);
	}
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
	pBlock->setFlag(QGraphicsItem::ItemIsMovable, m_eEditState == EditState_Move);
	addProcessor(pBlock);
}

void DesignNetView::onProcessorRemoved( Processor* processor )
{
	if (d->m_processorMaps.contains(processor))
		removeProcessor(d->m_processorMaps.value(processor));
}

void DesignNetView::onConnectionAdded( Processor* father, Processor* child )
{
	ProcessorArrowLink* pLink = new ProcessorArrowLink(0);
	scene()->addItem(pLink);
	pLink->connectProcessor(d->m_processorMaps[father], d->m_processorMaps[child]);
	pLink->setSelected(true);
}

void DesignNetView::setEditState( EditState e )
{
	if (m_eEditState != e)
	{
		m_eEditState = e;
		foreach(ProcessorGraphicsBlock* pBlock, d->m_processorMaps.values())
			pBlock->setFlag(QGraphicsItem::ItemIsMovable, m_eEditState == EditState_Move);
		if (m_eEditState == EditState_Link)
			setCursor(Qt::CrossCursor);
		else
			setCursor(Qt::ArrowCursor);
	}
}

}
