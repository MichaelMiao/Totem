#include "processor.h"
#include "port.h"
#include "../data/idata.h"
#include "utils/totemassert.h"
#include "designnetspace.h"
#include <QMutexLocker>
#include <QDebug>
namespace DesignNet{

Processor::Processor(DesignNetSpace *space): m_space(space)
{
    m_name = "";
	m_bReady = false;
}

Processor::~Processor()
{
	
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
		qDebug() << "Waiting";
		m_dataReadyWait.wait(&m_mutexReady);
		qDebug() << "After Waiting";
	}
	
	return m_bReady;
}

void Processor::afterProcess(bool status)
{
}

void Processor::run(QFutureInterface<bool> &fi)
{
	if(!beforeProcess())
		return ;
	{
		QMutexLocker lock(&m_mutexReady);
		m_bRunning = true;
	}
	bool ret = process(fi);
	{
		QMutexLocker lock(&m_mutexReady);
		m_bRunning = false;
	}
	afterProcess(ret);

	return ;
}

void Processor::setDataReady( const bool &bReady /*= true*/ )
{
	QMutexLocker lock(&m_mutexReady);
	if (m_bReady == bReady)
	{
		return;
	}
	m_bReady = bReady;
	if(m_bReady)
		m_dataReadyWait.wakeAll();
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

}
