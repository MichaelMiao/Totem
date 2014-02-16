#include "designnetspace.h"
#include "utils/totemassert.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"
#include "utils/runextensions.h"
#include <QDebug>
#include <QFutureSynchronizer>
using namespace Utils;
namespace DesignNet{

void Connection::serialize( XmlSerializer& s ) const
{
	if (m_srcProcessor == -1 || -1 == m_targetProcessor)
		return;

	s.serialize("src_processor", m_srcProcessor);
	s.serialize("target_processor", m_targetProcessor);
}

void Connection::deserialize( XmlDeserializer& s )
{
	s.deserialize("src_processor", m_srcProcessor);
	s.deserialize("target_processor", m_targetProcessor);
}

Connection::Connection( const Connection &c )
{
	m_srcProcessor	= c.m_srcProcessor;
	m_targetProcessor	= c.m_targetProcessor;
}


DesignNetSpace::DesignNetSpace(DesignNetSpace *space, QObject *parent) :
    Processor(space)
{

}

void DesignNetSpace::addProcessor(Processor *processor, bool bNotifyModify)
{
    TOTEM_ASSERT(processor != 0, return);
    bool bret = contains(processor);
    if(bret)
    {
        emit logout(tr("can't add the exist processor to the space"));
        return;
    }
	processor->setSpace(this);
	if (processor->id() == -1)
	{
		int iUID = generateUID();
		processor->setID(iUID);
	}
    m_processors.push_back(processor);
	
	QObject::connect(processor, SIGNAL(connected(Processor*, Processor*)), this, SIGNAL(connectionAdded(Processor*, Processor*)));
	QObject::connect(processor, SIGNAL(disconnected(Processor*, Processor*)), this, SIGNAL(connectionRemoved(Processor*, Processor*)));
	
	emit processorAdded(processor);
	if (bNotifyModify)
		emit modified();
}

void DesignNetSpace::removeProcessor(Processor *processor, bool bNotifyModify)
{
	if (processor == 0)
	{
		foreach (Processor* p, m_processors)
			removeProcessor(processor, bNotifyModify);
		return ;
	}
	processor->detach();
	QList<Processor*>::const_iterator itr = (qFind(m_processors, processor));
	TOTEM_ASSERT(itr != m_processors.end(), qDebug()<< "can't remove the processor");
	m_processors.removeOne(processor);
	emit processorRemoved(processor);
	delete processor;

	if (bNotifyModify)
		emit modified();
}

bool DesignNetSpace::contains(Processor *processor)
{
    return (qFind(m_processors, processor) != m_processors.constEnd());
}

Processor *DesignNetSpace::create(DesignNetSpace *space) const
{
    DesignNetSpace *s = new DesignNetSpace(space, parent());
    return s;
}

QString DesignNetSpace::name() const
{
    return QString("DesignNetSpace");
}

int DesignNetSpace::generateUID()
{
	int maxID = 0;
	foreach(Processor *processor, m_processors)
	{
		maxID = qMax(processor->id(), maxID);
		maxID++;
	}
    return maxID; 
}

void DesignNetSpace::propertyRemoving(Property *prop)
{
}

void DesignNetSpace::propertyRemoved(Property *prop)
{
}

bool DesignNetSpace::prepareProcess()
{
	QList<Processor*> exclusions;

	if(!sortProcessors(exclusions))
	{
		emit logout(tr("The designnet space can't be processed. Maybe there are some circle relationships in the space."));
		return false;
	}
	QList<Processor*> tempNet;
	foreach(Processor* processor, exclusions)
	{
		QList<Processor*> processors;
		bool bHasError = false;
		if(processor->indegree(tempNet) == 0)
		{
			bHasError = !processor->prepareProcess();
			if (bHasError)
				return false;
			processors.push_back(processor);
			tempNet.push_back(processor);
		}
	}
	return true;
}

bool DesignNetSpace::process(QFutureInterface<ProcessResult> &future)
{	
    ///
    /// \brief 首先进行拓扑排序, 检查一下是否可以执行
    ///
    QList<Processor*> exclusions;
    if(!sortProcessors(exclusions))
    {
        emit logout(tr("The designnet space can't be processed. Maybe there are some circle relationships in the space."));
		return false;
    }
	QList<Processor*> tempNet;
	foreach(Processor* processor, exclusions)
	{

	}

// 	bool bNeedLoop = true;
// 	while (bNeedLoop)
// 	{
// 		bNeedLoop = false;
// 
// 		QList<Processor*> tempNet;
// 		foreach(Processor* processor, exclusions)
// 		{
// 			QList<Processor*> processors;
// 			if(processor->indegree(tempNet) == 0)
// 			{
// 				processor->start();
// 				processors.push_back(processor);
// 				tempNet.push_back(processor);
// 			}
// 			foreach(Processor* processor, processors)
// 			{
// 				processor->waitForFinish();
// 				ProcessResult pr = processor->result();
// 				qDebug() << pr.m_bSucessed << pr.m_bNeedLoop;
// 				if (!pr.m_bSucessed)
// 				{
// 					emit logout(tr("Processor %d run faild").arg(processor->id()));
// 					return false;
// 				}
// 				else
// 				{
// 					bNeedLoop |= processor->result().m_bNeedLoop;
// 				}
// 			}
// 		}
// 	}
	
	emit logout(tr("The designnet space has been processed."));
	return true;
}

bool DesignNetSpace::finishProcess()
{
	return true;
}

Core::Id DesignNetSpace::typeID() const
{
    return Core::Id(QLatin1String("DesignNetSpace/") + name());
}

QString DesignNetSpace::category() const
{
    return tr("DesignNetSpace");
}

void DesignNetSpace::onShowMessage(QString log)
{
    emit logout(log);
}

void DesignNetSpace::onPropertyChanged_internal()
{
	Property *prop = qobject_cast<Property*>(sender());
	if(prop)
		propertyChanged(prop);
}

void DesignNetSpace::propertyChanged( Property *prop )
{
	
}

void DesignNetSpace::propertyAdded( Property* prop )
{
	QObject::connect(prop, SIGNAL(changed()), this, SLOT(onPropertyChanged_internal()));	
}

void DesignNetSpace::serialize( Utils::XmlSerializer& s ) const
{
	Processor::serialize(s);
	s.serialize("processors", m_processors, "processor");
}

void DesignNetSpace::deserialize( Utils::XmlDeserializer& s )
{
	Processor::deserialize(s);
	QList<Processor*> processors;
	s.deserializeCollection("processors", processors, "processor");
	foreach(Processor* p, processors)
	{
		p->init();
		addProcessor(p);
	}
	QList<Connection> connections;
	s.deserializeCollection("Connections", connections, "Connection");
	foreach(Connection c, connections)
	{
		Processor *srcProcessor = findProcessor(c.m_srcProcessor);
		Processor *targetProcessor = findProcessor(c.m_targetProcessor);
		if (!srcProcessor || !targetProcessor)
		{
			emit logout(tr("Cannot add the connection from processor %1 to processor %2").arg(c.m_srcProcessor).arg(c.m_targetProcessor));
			continue;
		}
		else
		{

// 			Port *srcPort = srcProcessor->getPort(c);
// 			Port *targetPort = targetProcessor->getPort(c.m_targetPort);
// 			if (!srcPort)
// 			{
// 				emit logout(tr("Cannot add the connection from port [%1] to port [%2] because source port is empty.")
// 					.arg(c.m_srcPort).arg(c.m_targetPort));
// 				continue;
// 			}
// 			if (!targetPort)
// 			{
// 				emit logout(tr("Cannot add the connection from port [%1] to port [%2]  because target port is empty")
// 					.arg(c.m_srcPort).arg(c.m_targetPort));
// 				continue;
// 			}
// 			if(!connectPort(targetPort, srcPort))
// 			{
// 				emit logout(tr("Cannot add the connection from port [%1] to port [%2] ")
// 					.arg(c.m_srcPort).arg(c.m_targetPort));
// 				continue;
// 			}
		}
	}
}

QList<Processor*> DesignNetSpace::processors()
{
	return m_processors;
}

Processor* DesignNetSpace::findProcessor( const int &id )
{
	QList<Processor*> processorList = processors();
	foreach(Processor* p, processorList)
	{
		if(p->id() == id)
			return p;
	}
	return 0;
}

void DesignNetSpace::detachProcessor(Processor* processor)
{
	QList<Processor*>::const_iterator itr = (qFind(m_processors, processor));
	TOTEM_ASSERT(itr != m_processors.end(), qDebug()<< "can't not detach the processor");
	processor->detach();
}

bool DesignNetSpace::sortProcessors(QList<Processor*> &processors)
{
	Q_ASSERT(processors.size() == 0);

	QList<Processor*> tempNet = m_processors;
	bool dirty = true;
	while(dirty)
	{
		dirty = false;
		for(int i = 0; i < tempNet.size(); i++)
		{
			Processor* processor = tempNet.at(i);
			int iTemp = processor->indegree(processors);
			if(iTemp == 0)
			{
				processors.append(processor);
				tempNet.removeOne(processor);
				dirty = true;
			}
		}
	}
	if(!tempNet.isEmpty())
		return false;
	
	return true;
}
}