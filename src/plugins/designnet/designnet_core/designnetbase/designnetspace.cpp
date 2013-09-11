#include "designnetspace.h"
#include "utils/totemassert.h"
#include "port.h"
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
	{
		return;
	}
	s.serialize("src_processor", m_srcProcessor);
	s.serialize("src_port", m_srcPort);
	s.serialize("target_processor", m_targetProcessor);
	s.serialize("target_port", m_targetPort);
}

void Connection::deserialize( XmlDeserializer& s )
{
	s.deserialize("src_processor", m_srcProcessor);
	s.deserialize("src_port", m_srcPort);
	s.deserialize("target_processor", m_targetProcessor);
	s.deserialize("target_port", m_targetPort);
}

Connection::Connection( const Connection &c )
{
	m_srcPort		= c.m_srcPort;
	m_srcProcessor	= c.m_srcProcessor;
	m_targetPort	= c.m_targetPort;
	m_targetProcessor	= c.m_targetProcessor;
}


DesignNetSpace::DesignNetSpace(DesignNetSpace *space, QObject *parent) :
    Processor(space)
{

}

void DesignNetSpace::addProcessor(Processor *processor)
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
    emit processorAdded(processor);
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

bool DesignNetSpace::connectPort(Port *inputPort, Port *outputPort)
{
    if(outputPort->connect(inputPort))
    {
        emit connectionAdded(inputPort, outputPort);
        return true;
    }
    return false;
}

bool DesignNetSpace::disconnectPort(Port *inputPort, Port *outputPort)
{
    if(outputPort->disconnect(inputPort))
    {
        emit connectionRemoved(inputPort, outputPort);
        return true;
    }
    return false;
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

void DesignNetSpace::removeProcessor(Processor *processor)
{
    QList<Processor*>::const_iterator itr = (qFind(m_processors, processor));
    TOTEM_ASSERT(itr != m_processors.end(), qDebug()<< "can't not remove the processor");
    m_processors.removeOne(processor);
    emit processorRemoved(processor);
    delete processor;
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
	bool bNeedLoop = true;
	while (bNeedLoop)
	{
		bNeedLoop = false;

		QList<Processor*> tempNet;
		foreach(Processor* processor, exclusions)
		{
			QList<Processor*> processors;
			if(processor->indegree(tempNet) == 0)
			{
				processor->start();
				processors.push_back(processor);
				tempNet.push_back(processor);
			}
			foreach(Processor* processor, processors)
			{
				processor->waitForFinish();
				if (!processor->result().m_bSucessed)
				{
					emit logout(tr("Processor %d run faild").arg(processor->id()));
					return false;
				}
				else
				{
					bNeedLoop |= processor->result().m_bNeedLoop;
				}
			}
		}
	}
	
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
	QList<Connection> connections;
	foreach(Processor *processor, m_processors)
	{
		foreach(Port* outputport, processor->getOutputPorts())
		{
			foreach (Port* inputport, outputport->connectedPorts())
			{
				Connection c;
				c.m_srcPort			= outputport->name();
				c.m_srcProcessor	= outputport->processor()->id();
				c.m_targetPort		= inputport->name();
				c.m_targetProcessor = inputport->processor()->id();
				connections.push_back(c);
			}
		}
	}
	s.serialize("Processors", m_processors, "Processor");
	s.serialize("Connections", connections, "Connection");
}

void DesignNetSpace::deserialize( Utils::XmlDeserializer& s )
{
	Processor::deserialize(s);
	QList<Processor*> processors;
	s.deserializeCollection("Processors", processors, "Processor");
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
			Port *srcPort = srcProcessor->getPort(c.m_srcPort);
			Port *targetPort = targetProcessor->getPort(c.m_targetPort);
			if (!srcPort)
			{
				emit logout(tr("Cannot add the connection from port [%1] to port [%2] because source port is empty.")
					.arg(c.m_srcPort).arg(c.m_targetPort));
				continue;
			}
			if (!targetPort)
			{
				emit logout(tr("Cannot add the connection from port [%1] to port [%2]  because target port is empty")
					.arg(c.m_srcPort).arg(c.m_targetPort));
				continue;
			}
			if(!connectPort(targetPort, srcPort))
			{
				emit logout(tr("Cannot add the connection from port [%1] to port [%2] ")
					.arg(c.m_srcPort).arg(c.m_targetPort));
				continue;
			}
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
		{
			return p;
		}
	}
	return 0;
}

void DesignNetSpace::detachProcessor( Processor* processor )
{
	QList<Processor*>::const_iterator itr = (qFind(m_processors, processor));
	TOTEM_ASSERT(itr != m_processors.end(), qDebug()<< "can't not remove the processor");
	m_processors.removeOne(processor);
	emit processorRemoved(processor);
}

bool DesignNetSpace::sortProcessors( QList<Processor*> &processors )
{
	Q_ASSERT(processors.size() == 0);

	QList<Processor*> tempNet = m_processors;
	bool dirty = true;
	while(dirty)
	{
		dirty = false;
		foreach(Processor* processor, tempNet)
		{
			int indegree = processor->indegree(processors);
			if(indegree == 0)
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