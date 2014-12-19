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
	qDebug() << m_processor->name() << "finished";
	emit m_processor->processFinished();
}

void ProcessorWorker::started()
{
	QMutexLocker locker(&m_mutex);
	m_bWorking = true;
	qDebug() << m_processor->name() << "start";
	emit m_processor->processStarted();
}

Processor::Processor(DesignNetSpace *space, QObject* parent, ProcessorType processorType)
	: QObject(parent),
	m_space(space),
	m_worker(this),
	m_eType(processorType), m_thread(0), m_bResizableInput(false)
{
	m_bDataDirty = true;
    m_name = "";
	m_id = -1;
	if (m_eType == ProcessorType_Permanent)
	{
		m_thread = new QThread(this);
		m_worker.moveToThread(m_thread);
		QObject::connect(m_thread, SIGNAL(started()), &m_worker, SLOT(started()));
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

void Processor::pushData(ProcessData &pd, QString strLabel)
{
	QList<Port*>::Iterator itr = m_outputPort.begin();
	for (; itr < m_outputPort.end(); itr++)
	{
		if ((*itr)->data()->dataType == pd.dataType && (*itr)->name() == strLabel)
		{
			(*itr)->addData(&pd);
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
	ProcessData pd(DATATYPE_USERTYPE);
	pd.variant.setValue<IData*>(data);
	if (pPort)
		pPort->addData(&pd);
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
	ProcessResult *pr = new ProcessResult;
	pr->m_bNeedLoop = false;
	pr->m_bSucessed = true;
	future.reportResult(*pr, 0);
	return true;
}

void Processor::afterProcess(bool status)
{
	m_bDataDirty = false;
	emit logout(tr("%1 id: %2 processing finished.").arg(name()).arg(id()));
}

void Processor::run(QFutureInterface<ProcessResult> &future)
{
 	ProcessResult *pr = new ProcessResult;
 	future.reportResult(pr, 0);
	if(!beforeProcess(future) || !process(future))
	{
		(*pr).m_bSucessed = false;
		future.reportResult(pr, 0);
		emit childProcessFinished();
		if (m_thread)
			m_thread->quit();
		return;
	}
	*pr = future.future().resultAt(0);
	afterProcess(future.future().resultAt(0).m_bSucessed);
	if (getOutputProcessor().size() == 0)
		emit childProcessFinished();
	if (m_thread)
		m_thread->quit();
}

void Processor::start()
{
	m_worker.started();
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
		qDebug() << tr("Waiting %1 for finish").arg(this->name());
		m_watcher.waitForFinished();
		qDebug() << tr("%1 finished").arg(this->name());
	}
}

bool Processor::connectionTest(Port* pOutput, Port* pInput)
{
	if (pOutput->data()->dataType == pInput->data()->dataType)
		return true;
	if (pInput->data()->dataType == DATATYPE_MATRIX
		&& (pOutput->data()->dataType >= DATATYPE_MATRIX || pOutput->data()->dataType <= END_DATATYPE_MATRIX))
	{
		return true;
	}
	return false;
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
	PropertyOwner::deserialize(s);
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
	if (m_icon.isNull())
		return QIcon(":/media/default_processor.png");
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
	emit processorModified();
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
	QList<Port*>::iterator itr = m_outputPort.begin();
	while (itr != m_outputPort.end())
	{
		QList<Port*>::iterator itrInput = ports.begin();
		while (itrInput != ports.end())
		{
			if ((*itr)->data()->dataType == (*itrInput)->data()->dataType)
			{
				(*itr)->connect(*itrInput);
				break;
			}
			itrInput++;
		}
		itr++;
	}
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
	
	return true;
}

bool Processor::isConnectTo(Processor* pChild)
{
	QList<Port*>::iterator itr = m_outputPort.begin();
	while (itr != m_outputPort.end())
	{
		if ((*itr)->connectedProcessors().contains(pChild))
			return true;
		itr++;
	}
	return false;
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

bool Processor::addPort(Port::PortType pt, DataType dt, QString sLabel, bool bRemovable)
{
	Port *pPort = new Port(pt, dt, sLabel, bRemovable, this);
	pPort->setProcessor(this);
	QObject::connect(pPort, SIGNAL(connectPort(Port*, Port*)), this, SLOT(onPortConnected(Port*, Port*)));
	if (pt == Port::IN_PORT)
		m_inputPort.push_back(pPort);
	else if (pt == Port::OUT_PORT)
		m_outputPort.push_back(pPort);
	
	emit portAdded(pPort);
	return true;
}

void Processor::removePort(Port *pPort)
{
	pPort->disconnect();
	m_inputPort.removeAll(pPort);
	m_outputPort.removeAll(pPort);
	emit portRemoved(pPort);
	pPort->deleteLater();
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
Port* Processor::getPort(const int iIndex)
{
	QList<Port*> ports;
	ports << m_inputPort << m_outputPort;
	if (iIndex >= ports.size())
		return 0;
	return ports[iIndex];
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

ProcessData Processor::getOneData(QString sLabel)
{
	if (getData(sLabel).size() == 0)
	{
		QList<DesignNet::ProcessData*> res;
		Port *pPort = getPort(Port::IN_PORT, sLabel);
		if (pPort)
		{
			QList<Port*> portsConnected = pPort->connectedPorts();
			for (int i = 0; i < portsConnected.size(); i++)
			{
				res << portsConnected[i]->data();
			}
		}
		ProcessData data;
		return data;
	}
	else
	{
		return *(getData(sLabel)).at(0);
	}
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
	return res.toSet().toList();
}

QList<Processor*> Processor::getOutputProcessor() const
{
	QList<Port*> ports = getPorts(Port::OUT_PORT);
	QList<Processor*> res;
	foreach(Port* p, ports)
		res << p->connectedProcessors();
	return res.toSet().toList();
}

void Processor::onPortConnected(Port* src, Port* target)
{
	emit processorModified();
}

void Processor::onPortDisconnected(Port* src, Port* target)
{
	emit processorModified();
}

void Processor::notifyDataWillChange()
{
	{
		QWriteLocker lock(&m_workingLock);
		m_bDataDirty	= true;
		qDebug() << name() << id() << "notify data changed";
		
	}
	QList<Processor*> processors = getOutputProcessor();
	for (QList<Processor*>::iterator itr = processors.begin(); itr != processors.end(); itr++)
	{
		(*itr)->waitForFinish();
		(*itr)->notifyDataWillChange();
	}
}

void Processor::notifyProcess()
{
	{
		QWriteLocker lock(&m_workingLock);
		m_bDataDirty = false;
	}
	QList<Processor*> processors = getOutputProcessor();
	m_waitProcessors = processors;
	qDebug() << name() << ":process";
	for (QList<Processor*>::iterator itr = processors.begin(); itr != processors.end(); itr++)
	{
		(*itr)->onNotifyProcess();
		qDebug() <<(*itr)->name() << "id-" << (*itr)->id() << "---father::" << name() << "-id" << id();
	}
}

void Processor::onNotifyDataChanged()
{
	QWriteLocker lock(&m_workingLock);
	m_bDataDirty = true;
	qDebug() << name() << id() << "notify data changed";
}

void Processor::onNotifyProcess()
{
	QWriteLocker lock(&m_workingLock);
	if (isRunning())
		return;
	qDebug() << name() << "  ID:" << id() <<"isRunning" << isRunning();
	for (QList<Port*>::iterator itr = m_inputPort.begin(); itr != m_inputPort.end(); itr++)
	{
		QList<Processor*> processors = (*itr)->connectedProcessors();
		for (QList<Processor*>::iterator itr = processors.begin(); itr != processors.end(); itr++)
		{
			if ((*itr)->isDataDirty())
				return;
		}
	}
	start();
}

void Processor::onChildProcessFinished()
{
	onChildProcessorFinish((Processor*)sender());
}

void Processor::onChildProcessorFinish(Processor* p)
{
	{
//		QWriteLocker lock(&m_workingLock);
		qDebug() << name() << id();
		qDebug() << "removing" << p->name() << p->id();
		if (!m_waitProcessors.contains(p))
		{
			int m = 0;
			qDebug() << "Error";
		}
		m_waitProcessors.removeOne(p);

	}
	if (m_waitProcessors.size() == 0)
		emit childProcessFinished();
}

void Processor::onCreateNewPort(Port::PortType pt)
{
	QString str;
	if (pt == Port::IN_PORT)
		str = tr("Input Port(%1)").arg(m_inputPort.size());
	else
		str = tr("Output Port(%1)").arg(m_outputPort.size());
	addPort(pt, DATATYPE_MATRIX, str, true);
}

bool Processor::isDataDirty()
{
	QReadLocker lock(&m_workingLock);
	return m_bDataDirty;
}

void Processor::waitChildrenFinished()
{
	QList<Processor*> ls = getOutputProcessor();
	for (int i = 0; i < ls.size(); i++)
		ls[i]->waitForFinish();
}

}
