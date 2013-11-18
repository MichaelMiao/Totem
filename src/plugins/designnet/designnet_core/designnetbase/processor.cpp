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
	for (QList<ProcessData>::Iterator itr = m_outputDatas.begin(); itr != m_outputDatas.end(); itr++)
		itr->processorID = id;
}

int Processor::id() const
{
    return m_id;
}

void Processor::pushData(const ProcessData &pd)
{
	QList<ProcessData>::Iterator itr = m_outputDatas.begin();
	for (; itr < m_outputDatas.end(); itr++)
	{
		if ((*itr).dataType == pd.dataType && pd.strLabel == (*itr).strLabel)
			(*itr).variant = pd.variant;
	}
}

QList<ProcessData> Processor::getOutputData(DataType dt)
{
	QList<ProcessData> res;
	QList<ProcessData>::Iterator itr = m_outputDatas.begin();
	for (; itr < m_outputDatas.end(); itr++)
	{
		if ((*itr).dataType == dt || dt == DATATYPE_INVALID)
			res << *itr;
	}
	return res;
}

QList<ProcessData> Processor::getInputData(DataType dt)
{
	QList<ProcessData> res;
	foreach (Processor* father, m_fathers)
	{
		QList<ProcessData> &datas = father->getOutputData(dt);
		for(QList<ProcessData>::iterator itr = datas.begin(); itr != datas.end(); itr++)
		{
			if (dt ==  (*itr).dataType)
				res << *itr;
		}
	}
	return res;
}

int Processor::indegree(QList<Processor *> exclusions) const
{
    int degree = 0;
	QVector<Processor*> processors = m_fathers;

	foreach(Processor *processor, processors)
	{
		if(!exclusions.contains(processor))
			degree++;
	}
    return degree;
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

bool Processor::connectionTest(Processor* father)
{
	QList<ProcessData> datas = datasNeeded();
	QList<ProcessData> datasProvided =  father->getOutputData();
	foreach (const ProcessData &p, datas)
	{
		bool bFind = false;
		foreach(const ProcessData &provide, datasProvided)
		{
			if ( p.strLabel == provide.strLabel &&
				p.dataType == provide.dataType &&
				(p.processorID != -1 && p.processorID == provide.processorID))
			{
				bFind = true;
				break;
			}
		}
		if (bFind == false)
			return false;
	}
	return true;
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

bool Processor::connectTo( Processor* child )
{
	if (!child->connectionTest(this))
		return false;
	
	if (m_children.contains(child) || m_fathers.contains(child)) // 不能连接两次
		return false;

	m_children.append(child);
	emit connected(this, child);
	return true;
}

bool Processor::disconnect( Processor* pChild )
{
	if (!m_children.contains(pChild)) // 不能连接两次
		return false;
	int iIndex = m_children.indexOf(pChild, 0);
	if (iIndex > 0)
		m_children.remove(iIndex);

	emit disconnected(this, pChild);
	return true;
}

}
