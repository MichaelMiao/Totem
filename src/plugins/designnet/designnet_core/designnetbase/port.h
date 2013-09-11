#ifndef PORT_H
#define PORT_H

#include <QObject>
#include "../designnet_core_global.h"
#include "processor.h"
#include "../data/idata.h"
#include <QReadWriteLock>
namespace DesignNet{
/**
 * @brief The Port class
 *
 * 端口，在Processor中有输入输出两种端口，一个输入端口可以使用多个输出端口数据
 *（可以通过设置MultiInput属性使该功能enble or not）
 */

class DESIGNNET_CORE_EXPORT Port : public QObject
{
	Q_OBJECT
public:
    enum PortType{
        IN_PORT,    //!< 输入端口
        OUT_PORT    //!< 输出端口
    };

    explicit Port(IData *data, PortType portType, const QString &name = "", QObject *parent = 0);
	virtual ~Port();
    PortType portType() const{return m_portType;}
    void setPortType(const PortType &portType){m_portType = portType;}

    Processor* processor() const{return m_processor;}
    void setProcessor(Processor* processor){m_processor = processor;}
    /*!
     * \brief connect
     *
     * 连接两个端口，该函数应该由输出端口调用，否则返回false
     * \param[in] port 当前端口所要连接的目标端口
     * \return 是否连接成功，函数内部通过canConnectTo()判断
     */
    bool connect(Port* port);//!< 当前端口将会作为输出端口连接到\e inputPort.

    bool disconnect(Port* port = 0);
    bool canConnectTo(Port* inputPort);     //!< 判断是否能够连接到\e inputPort

    bool isConnectedTo(const Port* port) const;   //!< 是否已经连接到了端口\e port
    bool isConnected() const;               //!< 是否处于连接状态
    bool isConnectedTo(const Processor* processor) const;
    bool isMultiInputSupported() const;
	void setMultiInputSupported(const bool &bSupported = true); //!< 设置端口是否支持多输入
    QList<Processor*> connectedProcessors() const;  //!< 返回该端口所连接的所有处理器
    QList<Port*>      connectedPorts() const;        //!< 返回所有连接的端口
    int connectedCount() const;

    QString name() const;
    void setName(const QString &name);
	
	QString marks() const { return m_portMarks; }
	void setMarks(const QString &strMark) { m_portMarks = strMark; }

    /*!
     * \brief addConnectedPort
     *
     * 内部使用该函数
     * \note 该函数没有进行类型检查，需要调用\a canConnect
     * \param port
     */
    void addConnectedPort(Port* port);
    /*!
     * \brief addConnectedPort
     *
     * 内部使用该函数
     * \note 该函数没有进行类型检查，需要调用\a canConnect
     * \param port
     */
    void removeConnectedPort(Port* port);//!< \note

    void notifyStateChanged();  //!< 端口状态改变
    //////////////////////////////////////////////////////
    ///
    ///
    void addData(IData* data);  //!< 向端口添加数据
    IData* data() const;        //!< 端口中存放的数据
	QVector<IData*> getInputData();

signals:

	void disconnectFromPort(Port* port);
protected:

    bool        m_bMultiInput;  //!< 是否支持多份输入
    PortType    m_portType;     //!< 端口类型
    IData*      m_data;         //!< 数据
    QString     m_name;         //!< 端口名称（一个Processor中名称唯一）
	QString		m_portMarks;		//!< 端口说明
    Processor*  m_processor;    //!< 该端口所属的处理器
    QList<Port*> m_portsConnected;//!< 当前端口所连接的所有Port
	QReadWriteLock m_dataLocker;
};
}

#endif // PORT_H
