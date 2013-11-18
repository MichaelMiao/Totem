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
	QList<Processor*>::const_iterator itr = (qFind(m_processors, processor));
	TOTEM_ASSERT(itr != m_processors.end(), qDebug()<< "can't not remove the processor");
	m_processors.removeOne(processor);
	emit processorRemoved(processor);

	if (bNotifyModify)
		emit modified();
	delete processor;
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

bool DesignNetSpace::connectProcessor( Processor* father, Processor* child )
{
	if (!child->connectionTest(father))
	{
		emit logout(tr("Can't link the processors"));
		return false;
	}

	return father->connectTo(child);//!< 发出connectAdd()信号
}

bool DesignNetSpace::disconnectProcessor(Processor* father, Processor* child)
{
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
}

void DesignNetSpace::deserialize( Utils::XmlDeserializer& s )
{
	Processor::deserialize(s);
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