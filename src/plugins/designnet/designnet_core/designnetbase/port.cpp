#include "stdafx.h"
#include "port.h"
#include "../../../coreplugin/icore.h"
#include "../designnet_core_def.h"
#include "utils/totemassert.h"
#include "processor.h"
#include <QWriteLocker>


namespace DesignNet
{

Port::Port(PortType portType, PortDataType dt, const QString& name, bool bRemovable, QObject* parent) :
	QObject(parent),
	m_bMultiInput(false),
	m_bRemovable(bRemovable),
	m_portType(portType),
	m_processor(0),
	m_name(name),
	m_data(dt)
{
}
/*!
 * \brief Port::connect
 *
 * 1、自己不能连接自己
 * \param[in] port 要连接的端口
 * \return 是否连接成功
 *
 */
bool Port::connect(Port* port)
{
	if (port == this)
		return false;

	if (!canConnectTo(port))
		return false;

// 	bool bConnected = m_processor->isConnectTo(port->processor());
// 	m_portsConnected.push_back(port);
// 	port->addConnectedPort(this);/// 直接将自己放到inputPort的列表中
// 
// 	if (!bConnected)
// 	{
// 		QObject::connect(port->processor(), SIGNAL(childProcessFinished()),
// 		                 m_processor, SLOT(onChildProcessFinished()), Qt::QueuedConnection);
// 		emit connectPort(this, port);
// 		emit m_processor->connected(m_processor, port->processor());
// 	}

	return true;
}

bool Port::disconnect(Port* inputport)
{
	QList<Processor*> procConnectedOld = connectedProcessors();

	if (inputport == 0) /// 移除所有链接的端口
	{
		foreach (Port* p, m_portsConnected)
		{
			p->removeConnectedPort(this);
			emit disconnectPort(this, p);
		}

		m_portsConnected.clear();
	}

	else
	{
		this->removeConnectedPort(inputport);
		inputport->removeConnectedPort(this);
		emit disconnectPort(this, inputport);
	}

	QList<Processor*> procConnected = connectedProcessors();
	QList<Processor*>::iterator itr = procConnectedOld.begin();

	while (itr != procConnectedOld.end())
	{
		if (!procConnected.contains(*itr))
			emit m_processor->onDisconnected(m_processor, *itr);

		itr++;
	}

	return true;
}
/*!
 * \brief Port::canConnectTo
 *
 * 1、本端口需要是\e OUT_PORT 类型的，
 * \e inputPort 需要是\e IN_PORT 类型的。
 * 2、端口不能是已经连接过的
 * 3、inputPort如果是不支持多输入的，那么，如果已经有连接，返回false
 *
 * \param[in] inputPort 目标端口（该端口要为输入端口）
 * \return
 */
bool Port::canConnectTo(Port* inputPort)
{
	///
	/// 本端口需要是\e OUT_PORT 类型的，inputPort的类型应该是输入端口
	if (m_portType != OUT_PORT || inputPort->portType() != IN_PORT)
		return false;

	///
	/// 该端口不能是已经连接过的
	if (isConnectedTo(inputPort))
		return false;

	///
	/// 检查该端口是否是支持多输入的,若不支持，并已经有链接的，返回false
	if (inputPort->isMultiInputSupported() == false
	    && inputPort->connectedCount() > 0)
	{
		return false;
	}

	return inputPort->processor()->connectionTest(this, inputPort);
}

bool Port::isConnectedTo(const Port* port) const
{
	QList<Port*>::const_iterator itr = m_portsConnected.begin();

	while (itr != m_portsConnected.constEnd())
	{
		if (*itr == port)
			return true;

		itr++;
	}

	return false;
}

bool Port::isConnected() const
{
	return m_portsConnected.count() > 0;
}

bool Port::isConnectedTo(const Processor* processor) const
{
	foreach (Port* port, m_portsConnected)
	{
		if (port->processor() == processor)
			return true;
	}

	return false;
}

bool Port::isMultiInputSupported() const
{
	return m_bMultiInput;
}

QList<Processor*> Port::connectedProcessors() const
{
	QList<Processor*> result;
	QList<Port*>::const_iterator itr = m_portsConnected.begin();

	for (; itr != m_portsConnected.end(); itr++)
	{
		if (!result.contains((*itr)->processor()))
			result.append((*itr)->processor());
	}

	return result;
}

QList<Port*> Port::connectedPorts() const
{
	return m_portsConnected;
}

int Port::connectedCount() const
{
	return m_portsConnected.count();
}

void Port::addConnectedPort(Port* port)
{
	if (m_portsConnected.contains(port))
		return;

	m_portsConnected.push_back(port);

	if (m_portType == OUT_PORT)
		emit connectPort(this, port);

	else
		emit connectPort(port, this);
}

void Port::removeConnectedPort(Port* port)
{
	m_portsConnected.removeOne(port);

	if (m_portType == OUT_PORT)
		emit disconnectPort(this, port);

	else
		emit disconnectPort(port, this);
}

QString Port::name() const
{
	return m_name;
}

void Port::setName(const QString& name)
{
	m_name = name;
}

void Port::addData(ProcessData* data)
{
	Q_ASSERT(m_portType == OUT_PORT);
	QWriteLocker locker(&m_dataLocker);
	m_data = *data;
	emit dataChanged();
}

ProcessData* Port::data()
{
	return &m_data;
}

ProcessData* Port::getInputData()
{
	QList<Port*> portsConnected = connectedPorts();

	for (int i = 0; i < portsConnected.size(); i++)
		return portsConnected[i]->data();

	return 0;
}

void Port::setMultiInputSupported(const bool& bSupported /*= true*/)
{
	m_bMultiInput = bSupported;
}

int Port::getIndex()
{
	return -1;
}

void Port::setProcessor(Processor* processor)
{
	m_processor = processor;
}

ProcessData::ProcessData(PortDataType dt) : dataType(dt), m_iIndex(-1)
{
}

}
