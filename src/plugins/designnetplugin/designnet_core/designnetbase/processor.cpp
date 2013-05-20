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

}

void ProcessorWorker::run()
{
	m_processor->run();
}

Processor::Processor(DesignNetSpace *space, QObject* parent)
	: QObject(parent), m_space(space), m_worker(this)
{
    m_name = "";
	m_bReady = false;
	m_id = 0;
	m_thread = new QThread(this);
	m_worker.moveToThread(m_thread);
	QObject::connect(m_thread, SIGNAL(started()), &m_worker, SLOT(run()));
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
    TOTEM_ASSERT(port != 0, return)
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
	qDebug() << name();
	if(!beforeProcess())
		return ;
	{
		QMutexLocker lock(&m_mutexReady);
		m_bRunning = true;
	}
	bool ret = process();
	{
		QMutexLocker lock(&m_mutexReady);
		m_bRunning = false;
	}
	if(!ret)
	{
		emit logout(tr("Failed to process Processor(id: %1)").arg(id()));
	}
	afterProcess(ret);
}

void Processor::setDataReady( const bool &bReady /*= true*/ )
{
	QMutexLocker lock(&m_mutexReady);
	
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







}
