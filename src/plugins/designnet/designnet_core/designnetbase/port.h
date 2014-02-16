#ifndef PORT_H
#define PORT_H

#include "../designnet_core_global.h"
#include "../data/idata.h"
#include "../data/datatype.h"
#include <QObject>
#include <QReadWriteLock>
#include <QVariant>


namespace DesignNet{


class DESIGNNET_CORE_EXPORT ProcessData
{
public:

	ProcessData(DataType dt = DATATYPE_INVALID);
	QVariant	variant;		//!< ����������
	int			processorID;	//!< ���������ݵĴ�����
	DataType	dataType;		//!< ��������
};

/**
 * @brief The Port class
 *
 * �˿ڣ���Processor��������������ֶ˿ڣ�һ������˿ڿ���ʹ�ö������˿�����
 *������ͨ������MultiInput����ʹ�ù���enble or not��
 */
class Processor;
class DESIGNNET_CORE_EXPORT Port : public QObject
{
	Q_OBJECT
public:
    enum PortType{
        IN_PORT,    //!< ����˿�
        OUT_PORT    //!< ����˿�
    };

    explicit Port(PortType portType, DataType dt, const QString &label = "", QObject *parent = 0);
	virtual ~Port();
    PortType portType() const{return m_portType;}
    void setPortType(const PortType &portType){m_portType = portType;}

    Processor* processor() const{return m_processor;}
    void setProcessor(Processor* processor){m_processor = processor;}
    /*!
     * \brief connect
     *
     * ���������˿ڣ��ú���Ӧ��������˿ڵ��ã����򷵻�false
     * \param[in] port ��ǰ�˿���Ҫ���ӵ�Ŀ��˿�
     * \return �Ƿ����ӳɹ��������ڲ�ͨ��canConnectTo()�ж�
     */
    bool connect(Port* inputPort);//!< ��ǰ�˿ڽ�����Ϊ����˿����ӵ�\e inputPort.

    bool disconnect(Port* port = 0);
    bool canConnectTo(Port* inputPort);     //!< �ж��Ƿ��ܹ����ӵ�\e inputPort

    bool isConnectedTo(const Port* port) const;   //!< �Ƿ��Ѿ����ӵ��˶˿�\e port
    bool isConnected() const;               //!< �Ƿ�������״̬
    bool isConnectedTo(const Processor* processor) const;
    bool isMultiInputSupported() const;
	void setMultiInputSupported(const bool &bSupported = true); //!< ���ö˿��Ƿ�֧�ֶ�����
    QList<Processor*> connectedProcessors() const;  //!< ���ظö˿������ӵ����д�����
    QList<Port*>      connectedPorts() const;        //!< �����������ӵĶ˿�
    int connectedCount() const;
    /*!
     * \brief addConnectedPort
     *
     * �ڲ�ʹ�øú���
     * \note �ú���û�н������ͼ�飬��Ҫ����\a canConnect
     * \param port
     */
    void addConnectedPort(Port* port);
    /*!
     * \brief addConnectedPort
     *
     * �ڲ�ʹ�øú���
     * \note �ú���û�н������ͼ�飬��Ҫ����\a canConnect
     * \param port
     */
    void removeConnectedPort(Port* port);//!< \note

    QString name() const;
    void setName(const QString &name);

    ///
    ///
    void addData(ProcessData* data);  //!< ��˿��������
    ProcessData* data();        //!< �˿��д�ŵ�����

signals:

	void connectPort(Port* src, Port* target);
	void disconnectPort(Port* src, Port* target);

protected:

    bool			m_bMultiInput;  //!< �Ƿ�֧�ֶ������
    PortType		m_portType;     //!< �˿�����
    ProcessData		m_data;         //!< ����
    QString			m_name;         //!< �˿����ƣ�һ��Processor������Ψһ��
    Processor*		m_processor;    //!< �ö˿������Ĵ�����
    QList<Port*>	m_portsConnected;//!< ��ǰ�˿������ӵ�����Port
	QReadWriteLock	m_dataLocker;
};
}

#endif // PORT_H
