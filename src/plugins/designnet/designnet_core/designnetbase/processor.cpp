#include "processor.h"
#include "port.h"
#include "../data/idata.h"
#include "utils/totemassert.h"
#include "designnetspace.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/runextensions.h"

#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <QtConcurrentRun>

namespace DesignNet{


ProcessorWorker::ProcessorWorker( Processor *processor )
	: m_processor(processor)
{
	m_bWorking = false;
}

void ProcessorWorker::run()
{
	futureInterface.reportStarted();
	QFuture<ProcessResult> future = futureInterface.future();
	m_processor->m_watcher.setFuture(future);
	m_processor->run(futureInterface);
	futureInterface.reportFinished();
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

Processor::Processor(DesignNetSpace *space, QObject* parent, ProcessorType processorType)
	: QObject(parent), m_space(space), m_worker(this), m_eType(processorType), m_thread(0)
{
    m_name = "";
	m_id = -1;
	m_portCountResizable = false;
	if (processorType == ProcessorType_Permanent)
	{
		m_thread = new QThread(this);
		m_worker.moveToThread(m_thread);
		QObject::connect(m_thread, SIGNAL(started()), &m_worker, SLOT(run()));
		QObject::connect(m_thread, SIGNAL(finished()), &m_worker, SLOT(stopped()));
		QObject::connect(m_thread, SIGNAL(terminated()), &m_worker, SLOT(stopped()));
	}
	else
	{
		QObject::connect(&m_watcher, SIGNAL(finished()), &m_worker, SLOT(stopped()));
		QObject::connect(&m_watcher, SIGNAL(started()), &m_worker, SLOT(started()));
	}
}

Processor::~Processor()
{
	if (m_eType == ProcessorType_Permanent)
	{
		Q_ASSERT(m_thread);
		m_thread->terminate();
		m_thread->wait();
	}
	else
	{
		m_watcher.cancel();
		m_watcher.waitForFinished();
	}
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
	Q_ASSERT(port);
    port->addData(data);
}

QVector<IData*> Processor::getData( const QString& portname )
{
	QVector<IData*> res;
	Port *port = getPort(portname);
	Q_ASSERT(port);
	if (port->portType() == Port::OUT_PORT)
	{
		res << port->data();
	}
	else
	{
		QList<Port*> ports = port->connectedPorts();
		foreach (Port* p, ports)
			res << p->data();
	}
	return res;
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
}

bool Processor::beforeProcess(QFutureInterface<ProcessResult> &future)
{
	return true;
}

void Processor::afterProcess(bool status)
{
	emit logout(tr("%1 id: %2 processing finished.").arg(name()).arg(id()));
}

void Processor::run( QFutureInterface<ProcessResult> &future )
{
	ProcessResult pr;
	future.reportResult(pr);
	if(!beforeProcess(future) || !process(future))
	{
		pr.m_bNeedLoop = future.future().result().m_bNeedLoop;
		pr.m_bSucessed = false;
		future.reportResult(pr);
	}
	afterProcess(pr.m_bSucessed);
}

void Processor::setRepickData( const bool &repick /*= true*/ )
{
	m_bRepickData = repick;
}

void Processor::start()
{
	if (m_eType == ProcessorType_Permanent)
		m_thread->start();
	else
		m_watcher.setFuture(QtConcurrent::run(&Processor::run, this));
}

void Processor::waitForFinish()
{
	if(m_eType == ProcessorType_Permanent && m_thread->isRunning())
	{
		qDebug() << tr("Waiting %1 for finish").arg(this->name());
		m_thread->quit();
		m_thread->wait();
		qDebug() << tr("%1 finished").arg(this->name());
	}
	else if (m_eType == ProcessorType_Once)
	{
		m_watcher.waitForFinished();
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
