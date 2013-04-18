#include "processor.h"
#include "port.h"
#include "../data/idata.h"
#include "utils/totemassert.h"
#include "designnetspace.h"
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
	if(port->portType() == Port::OUT_PORT)
	{
		return;
	}
	if(m_thread->isRunning())
	{
		qDebug() << tr("Waiting %1 for finish").arg(this->name());
		m_thread->quit();
		m_thread->wait();
		qDebug() << tr("%1 finished").arg(this->name());
	}
}

bool Processor::beforeProcess()
{
	QMutexLocker lock(&m_mutexReady);
	if(!m_bReady)
	{
		emit logout(tr("Data has not been ready!"));
	}
	
	return m_bReady;
}

void Processor::afterProcess(bool status)
{
	setDataReady(false);
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
	m_thread->quit();
	m_thread->wait();
}







}
