#include "port.h"
#include "utils/totemassert.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
namespace DesignNet{

Port::Port(IData *data, PortType portType,const QString &name) :
    m_bMultiInput(false),
    m_portType(portType),
    m_processor(0),
    m_data(data),
	m_name(name)
{
}
/*!
 * \brief Port::connect
 *
 * 1、自己不能连接自己
 * 2、要么this可以和port连接，要么port可以和this连接
 * \param[in] port 要连接的端口
 * \return 是否连接成功
 *
 */
bool Port::connect(Port *port)
{
    TOTEM_ASSERT(port != 0, return false);
    if(port == this)
        return false;
    if(!canConnectTo(port))
        return false;
    m_portsConnected.push_back(port);
    port->addConnectedPort(this);/// 直接将自己放到inputPort的列表中
    ///
    /// 两端口通知状态改变
    this->notifyStateChanged();
    port->notifyStateChanged();
    return true;
}

bool Port::disconnect(Port *port)
{
    if(port == 0)/// 移除所有链接的端口
    {
        foreach(Port* p, m_portsConnected)
        {
           p->removeConnectedPort(this);
           p->notifyStateChanged();
        }
        m_portsConnected.clear();
        this->notifyStateChanged();
    }
    else
    {
        this->removeConnectedPort(port);
        port->removeConnectedPort(this);
        this->notifyStateChanged();
        port->notifyStateChanged();
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
bool Port::canConnectTo(Port *inputPort)
{
    TOTEM_ASSERT(inputPort != 0, return false);
    ///
    /// 本端口需要是\e OUT_PORT 类型的，inputPort的类型应该是输入端口
    if(m_portType!= OUT_PORT || inputPort->portType() != IN_PORT)
    {
        return false;
    }
    ///
    /// 该端口不能是已经连接过的
    if(isConnectedTo(inputPort))
        return false;
    ///
    /// 检查该端口是否是支持多输入的,若不支持，并已经有链接的，返回false
    if(inputPort->isMultiInputSupported() == false
            && inputPort->connectedCount() > 0)
    {
        return false;
    }
	return inputPort->processor()->connectionTest(this, inputPort);
}

bool Port::isConnectedTo(const Port *port) const
{
    QList<Port*>::const_iterator itr = qFind(m_portsConnected, port);
    if(itr != m_portsConnected.constEnd())
        return true;
    return false;
}

bool Port::isConnected() const
{
    return m_portsConnected.count()>0;
}

bool Port::isConnectedTo(const Processor *processor) const
{
    foreach(Port* port, m_portsConnected)
    {
        if(port->processor() == processor)
            return true;
    }
    return false;
}

bool Port::isMultiInputSupported() const
{
    return m_bMultiInput;
}

QList<Processor *> Port::connectedProcessors() const
{
    QList<Processor *> result;
    foreach(Port* port, m_portsConnected)
    {
        if(!result.contains(port->processor()))
        {
            result.append(port->processor());
        }
    }
    return result;
}

QList<Port *> Port::connectedPorts() const
{
    return m_portsConnected;
}

int Port::connectedCount() const
{
    return m_portsConnected.count();
}

void Port::addConnectedPort(Port *port)
{
    if(m_portsConnected.contains(port))
        return;
    m_portsConnected.push_back(port);
}

void Port::removeConnectedPort(Port *port)
{
    m_portsConnected.removeOne(port);
}

QString Port::name() const
{
    return m_name;
}

void Port::setName(const QString &name)
{
    m_name = name;
}

void Port::notifyStateChanged()
{
    processor()->stateChanged(this);
}

void Port::notifyDataArrive()
{
    processor()->dataArrived(this);
}

void Port::addData(IData *data)
{
	if (m_portType == IN_PORT)
	{
		m_processor->waitForFinish();/// 等待该端口所在的处理器完成任务后，才能写数据
		m_data->copy(data);
		notifyDataArrive();
	}
	if(m_portType == OUT_PORT)
	{
		m_data->copy(data);
		foreach(Port* port, m_portsConnected)
		{
			port->addData(data);
		}
	}
}

IData *Port::data() const
{
    return m_data;
}

Port::~Port()
{
	if (m_data)
	{
		delete m_data;
		m_data = 0;
	}
}

}
