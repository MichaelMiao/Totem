#include "processor.h"
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
#include <QtAlgorithms>
#include <QVector>

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

void Processor::pushData(const ProcessData &pd, QString strLabel)
{
	QList<Port*>::Iterator itr = m_outputPort.begin();
	for (; itr < m_outputPort.end(); itr++)
	{
		if ((*itr)->data()->dataType == pd.dataType && (*itr)->name() == strLabel)
		{
			(*itr)->data()->variant = pd.variant;
			break;
		}
	}
}

void Processor::pushData(QVariant &var, DataType dataType, QString strLabel /*= ""*/, int iProcessId /*= -1*/)
{
	ProcessData pd(dataType);
	pd.variant	= var;
	pd.processorID = iProcessId == -1 ? m_id : iProcessId;
	pushData(pd, strLabel);
}

void Processor::pushData(IData* data, QString strLabel /*= ""*/, int iProcessId /*= -1*/)
{
	Port* pPort = getPort(Port::OUT_PORT, strLabel);
	if (pPort)
		pPort->data()->variant.setValue<IData*>(data);
}

QList<ProcessData> Processor::getOutputData(DataType dt)
{
	QList<ProcessData> res;
	QList<Port*>::Iterator itr = m_outputPort.begin();
	for (; itr < m_outputPort.end(); itr++)
		if ((*itr)->data()->dataType == dt || dt == DATATYPE_INVALID)
			res << *((*itr)->data());

	return res;
}

QList<ProcessData> Processor::getInputData(DataType dt)
{
	QList<ProcessData> res;
	QList<Processor*> fathers = getInputProcessor();
	foreach (Processor* father, fathers)
	{
		QList<ProcessData> &datas = father->getOutputData(dt);
		for(QList<ProcessData>::iterator itr = datas.begin(); itr != datas.end(); itr++)
		{
			if (dt ==  (*itr).dataType || dt == DATATYPE_INVALID)
				res << *itr;
		}
	}
	return res;
}

int Processor::indegree(QList<Processor *> exclusions) const
{
    int degree = 0;
	const QList<Processor*> processors = getInputProcessor();

	foreach(Processor *processor, processors)
	{
		if(!exclusions.contains(processor))
			degree++;
	}
    return degree;
}

bool Processor::beforeProcess(QFutureInterface<ProcessResult> &future)
{
	ProcessResult pr;
	pr.m_bNeedLoop = false;
	pr.m_bSucessed = true;
	future.reportResult(pr, 0);
	return true;
}

void Processor::afterProcess(bool status)
{
	emit logout(tr("%1 id: %2 processing finished.").arg(name()).arg(id()));
}

void Processor::run( QFutureInterface<ProcessResult> &future )
{
	ProcessResult pr;
	future.reportResult(pr, 0);
	if(!beforeProcess(future) || !process(future))
	{
		pr = future.future().result();
		pr.m_bSucessed = false;
		future.reportResult(pr, 0);
	}
	pr = future.future().resultAt(0);
	afterProcess(future.future().resultAt(0).m_bSucessed);
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

bool Processor::connectionTest(Port* pOutput, Port* pInput)
{
	return pOutput->data()->dataType == pInput->data()->dataType;
}

bool Processor::connectionTest(Processor* father)
{
	QList<Port*> portsNeed		= getPorts(Port::IN_PORT);
	QList<Port*> fatherProvided	= father->getPorts(Port::OUT_PORT);
	foreach (Port* p, portsNeed)
	{
		foreach(Port* provide, fatherProvided)
		{
			if (p->data()->dataType == provide->data()->dataType)
				return true;//!< 有可以提供的数据
		}
	}
	return false;//!< 如果有提供的数据早就返回了
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

bool Processor::connectTo(Processor* child)
{
	if (!child->connectionTest(this))
		return false;

	QList<Port*> ports = child->getPorts(Port::IN_PORT);
	foreach (Port* pOutput, m_outputPort)
	{
		foreach(Port* pInput, ports)
		{
			if (pOutput->data()->dataType == pInput->data()->dataType)
				pOutput->connect(pInput);
		}
	}

	emit connected(this, child);
	return true;
}

bool Processor::disconnect(Processor* pChild)
{
	foreach (Port* portOut, m_outputPort)
	{
		QList<Port*> ports = portOut->connectedPorts();
		foreach (Port* p, ports)
		{
			if (p->processor() == pChild)
				portOut->disconnect(p);
		}
		qDebug() << portOut->connectedPorts().size();
	}
	
	emit disconnected(this, pChild);
	return true;
}

void Processor::detach()
{
	QList<Processor*> inputProcessors = getInputProcessor();
	QList<Processor*> outputProcessors = getOutputProcessor();

	foreach(Processor* p, inputProcessors)
		p->disconnect(this);

	foreach(Processor* p, outputProcessors)
		disconnect(p);
}

bool Processor::addPort(Port::PortType pt, DataType dt, QString sLabel)
{
	Port *pPort = new Port(pt, dt, sLabel, this);
	pPort->setProcessor(this);
	if (pt == Port::IN_PORT)
		m_inputPort.push_back(pPort);
	else if (pt == Port::OUT_PORT)
		m_outputPort.push_back(pPort);

	return true;
}

Port* Processor::getPort(Port::PortType pt, DataType dt)
{
	QList<Port*> &ports = pt == Port::IN_PORT ? m_inputPort : m_outputPort;
	foreach (Port* p, ports)
	{
		if (p->data()->dataType == dt)
			return p;
	}
	return NULL;
}

Port* Processor::getPort(Port::PortType pt, QString sLabel)
{
	QList<Port*> &ports = pt == Port::IN_PORT ? m_inputPort : m_outputPort;
	foreach (Port* p, ports)
	{
		if (p->name() == sLabel)
			return p;
	}
	return NULL;
}

QList<DesignNet::ProcessData*> Processor::getData(QString sLabel)
{
	QList<DesignNet::ProcessData*> res;
	Port *pPort = getPort(Port::IN_PORT, sLabel);
	if (pPort)
	{
		QList<Port*> portsConnected = pPort->connectedPorts();
		for (int i = 0; i < portsConnected.size(); i++)
			res << portsConnected[i]->data();
	}
	return res;
}

QList<Port*> Processor::getPorts(Port::PortType pt) const
{
	return pt == Port::IN_PORT ? m_inputPort : m_outputPort;
}

QList<Processor*> Processor::getInputProcessor() const
{
	QList<Port*> ports = getPorts(Port::IN_PORT);
	QList<Processor*> res;
	foreach(Port* p, ports)
		res << p->connectedProcessors();
	return res;
}

QList<Processor*> Processor::getOutputProcessor() const
{
	QList<Port*> ports = getPorts(Port::OUT_PORT);
	QList<Processor*> res;
	foreach(Port* p, ports)
		res << p->connectedProcessors();
	return res;
}

}
