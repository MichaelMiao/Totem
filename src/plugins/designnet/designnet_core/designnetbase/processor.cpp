#include "processor.h"
#include "port.h"
#include "../data/idata.h"
#include "utils/totemassert.h"
#include "designnetspace.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"

#include <QMutexLocker>
#include <QDebug>
#include <QThread>
namespace DesignNet{


ProcessorWorker::ProcessorWorker( Processor *processor )
	: m_processor(processor)
{
	m_bWorking = false;
}

void ProcessorWorker::run()
{
	m_processor->run();
}

bool ProcessorWorker::isWorking()
{
	QMutexLocker locker(&m_mutex);
	return m_bWorking;
}

void ProcessorWorker::stopped()
{
	QMutexLocker locker(&m_mutex);
	m_bWorking = false;
}

void ProcessorWorker::started()
{
	QMutexLocker locker(&m_mutex);
	m_bWorking = true;
}

Processor::Processor(DesignNetSpace *space, QObject* parent)
	: QObject(parent), m_space(space), m_worker(this)
{
    m_name = "";
	m_bReady = false;
	m_id = -1;
	m_portCountResizable = false;
	m_thread = new QThread(this);
	m_worker.moveToThread(m_thread);
	m_icon = QIcon(":/media/default_processor.png");
	QObject::connect(m_thread, SIGNAL(started()), &m_worker, SLOT(run()));
	QObject::connect(m_thread, SIGNAL(finished()), &m_worker, SLOT(stopped()));
	QObject::connect(m_thread, SIGNAL(terminated()), &m_worker, SLOT(stopped()));
}

Processor::~Processor()
{
	m_thread->terminate();
	m_thread->wait();
}

const QList<Port*> & Processor::getInputPorts() const
{
    return m_inputPorts;
}

const QList<Port*> & Processor::getOutputPorts() const
{
    return m_outputPorts;
}

Port *Processor::getPort(const QString &name) const
{
    foreach (Port* port, m_inputPorts) {
        if(port->name() == name)
            return port;
    }
    foreach (Port* port, m_outputPorts) {
        if(port->name() == name)
            return port;
    }
    return 0;
}

void Processor::addPort(Port *port)
{
    TOTEM_ASSERT(port != 0, return)
    port->setProcessor(this);
    if(port->portType() == Port::IN_PORT)
        m_inputPorts.append(port);
    else
        m_outputPorts.append(port);
}

void Processor::removePort(Port *port)
{
    if (port == 0)
    {
		m_inputPorts.clear();
		m_outputPorts.clear();
		return;
    }
    if(port->portType() == Port::IN_PORT)
    {
        m_inputPorts.removeOne(port);
    }
    else
        m_outputPorts.removeOne(port);
}

void Processor::setName(const QString &name)
{
    m_name = name;
}

QString Processor::name() const
{
    return m_name;
}

void Processor::setID(const int &id)
{
    m_id = id;
}

int Processor::id() const
{
    return m_id;
}

void Processor::pushData(IData *data, const QString &portname)
{
    Port *port = getPort(portname);
    if(port)
        port->addData(data);
}

int Processor::indegree(QList<Processor *> exclusions) const
{
    int degree = 0;
    foreach(Port* port, m_inputPorts)
    {
        QList<Processor*> processors = port->connectedProcessors();

        foreach(Processor *processor, processors)
        {
            if(!exclusions.contains(processor))
            {
                degree++;
            }
        }
    }
    return degree;
}

void Processor::stateChanged(Port *port)
{
}

void Processor::dataArrived(Port *port)
{
	emit logout("dataArrived");
	if (checkDataReady())
	{
		setDataReady(true);
	}
}

bool Processor::beforeProcess()
{
	QMutexLocker lock(&m_mutexReady);
	if(!m_bReady)
	{
		emit logout(tr("Data has not been ready!"));
		return false;
	}
	emit logout(tr("%1 id: %2 start processing...").arg(name()).arg(id()));
	return m_bReady;
}

void Processor::afterProcess(bool status)
{
	emit logout(tr("%1 id: %2 processing finished.").arg(name()).arg(id()));
}

void Processor::run()
{
	if(!beforeProcess())
		return ;
	bool ret = process();
	if(!ret)
	{
		emit logout(tr("Failed to process Processor(id: %1)").arg(id()));
	}
	afterProcess(ret);
}

void Processor::setDataReady( const bool &bReady /*= true*/ )
{
	QMutexLocker lock(&m_mutexReady);
	emit logout("set Data Ready");
	m_bReady = bReady;
	if(m_bReady)
		qDebug() << name() << "data is ready";
	if(m_bReady)
	{
		qDebug() << name() << "thread is started";
		m_thread->start();
	}
}

bool Processor::isDataReady() const
{
	QMutexLocker lock(&m_mutexReady);
	return m_bReady;
}

void Processor::setRepickData( const bool &repick /*= true*/ )
{
	m_bRepickData = repick;
}

void Processor::waitForFinish()
{
	if(m_thread->isRunning())
	{
		qDebug() << tr("Waiting %1 for finish").arg(this->name());
		m_thread->quit();
		m_thread->wait();
		qDebug() << tr("%1 finished").arg(this->name());
	}
}

bool Processor::connectionTest( Port* src, Port* target )
{
	if (src->data()->id() == target->data()->id())
	{
		return true;
	}
	return false;
}

void Processor::serialize( Utils::XmlSerializer& s ) const
{
	s.serialize("Name", name());
	s.serialize("ID", id());
	PropertyOwner::serialize(s);
}

void Processor::deserialize( Utils::XmlDeserializer& s )
{
	s.deserialize("Name", m_name);
	s.deserialize("ID", m_id);
}

QString Processor::serializableType() const
{
	return typeID().toString();
}

Utils::XmlSerializable* Processor::createSerializable() const
{
	return (Utils::XmlSerializable*)create();
}

void Processor::setSpace( DesignNetSpace *space ) 
{
	if (m_space)
	{
		m_space->detachProcessor(this);
	}
	m_space = space;
}

bool Processor::checkDataReady()
{
	int i = -1;
	foreach(Port *port, m_inputPorts)
	{
		if (port->data()->isPermanent())
		{
			continue;
		}
		if (i == -1)
		{
			i = port->data()->index();
		}
		if (i == -1 || i != port->data()->index())
		{
			return false;
		}
	}
	return true;
}

QIcon Processor::icon() const
{
	return m_icon;
}

bool Processor::isPortCountResizable() const
{
	return m_portCountResizable;
}

void Processor::setPortCountResizable( const bool &resizable )
{
	m_portCountResizable = true;
}

Core::Id Processor::typeID() const
{
	return Core::Id(QString(category() + "/" + name()));
}

QString Processor::category() const
{
	return tr("Default");
}

void Processor::setIcon( const QString &str )
{
	m_icon = QIcon(str);
}

bool Processor::isRunning()
{
	return m_worker.isWorking();
}

void Processor::showConfig()
{

}

void Processor::propertyAdded( Property* prop )
{
	QObject::connect(prop, SIGNAL(changed()), this, SLOT(onPropertyChanged_internal()));
}

void Processor::propertyChanged( Property *prop )
{

}

void Processor::onPropertyChanged_internal()
{
	Property *prop = qobject_cast<Property*>(sender());
	if(prop)
		propertyChanged(prop);
}

}
