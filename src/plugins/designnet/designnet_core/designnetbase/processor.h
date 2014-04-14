#ifndef PROCESSOR_H
#define PROCESSOR_H


#include "../designnet_core_global.h"
#include "../property/propertyowner.h"
#include "../data/datatype.h"
#include "Utils/XML/xmldeserializer.h"
#include "port.h"
#include "../widgets/processorfrontwidget.h"
#include <QFutureInterface>
#include <QFutureWatcher>
#include <QObject>
#include <QIcon>
#include <QVector>
#include <QList>
#include <QMap>
#include <QReadWriteLock>

#define DECLEAR_PROCESSOR(x) \
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const \
	{ \
		return new x(space); \
	}

#define BEGIN_PROCESS() \
	notifyDataWillChange(); \
	m_bDataDirty = true;
#define END_PROCESS() \
	m_bDataDirty = false;


QT_BEGIN_NAMESPACE
class QThread;
QT_END_NAMESPACE

namespace DesignNet{

class IData;
class DesignNetSpace;
class Processor;

enum ProcessorType
{
	ProcessorType_Permanent,
	ProcessorType_Once
};

class ProcessResult
{
public:
	ProcessResult() : m_bSucessed(true), m_bNeedLoop(false)
	{
	}
	bool m_bSucessed;
	bool m_bNeedLoop;	// 是否需要重复执行
};

class ProcessorWorker : public QObject{
	Q_OBJECT
public:
	ProcessorWorker(Processor *processor);
	bool isWorking();
	Processor* m_processor;
public slots:
	void run();
	void stopped();
	void started();
protected:
	QFutureInterface<ProcessResult> futureInterface;
	bool			m_bWorking;				//!< 是否正在执行
	QMutex			m_mutex;
};

class DESIGNNET_CORE_EXPORT Processor : public QObject, public PropertyOwner
{
	friend class ProcessorWorker;
	Q_OBJECT
public:
    
	explicit Processor(DesignNetSpace *space = 0, QObject* parent = 0, ProcessorType processorType = ProcessorType_Once);
    virtual ~Processor();
	
	virtual void init() { }
	virtual Processor* create(DesignNetSpace *space = 0) const = 0;  //!< 创建Processor

	virtual ProcessorFrontWidget* processorFrontWidget() { return 0; }

	void waitForFinish();
	ProcessResult result() { return m_watcher.result(); }

	int indegree(QList<Processor*> exclusions = QList<Processor*>()) const; //!< 计算入度
	virtual bool connectionTest(Port* pOutput, Port* pInput);
	virtual bool connectionTest(Processor* father);

	QList<Processor*> getInputProcessor() const;
	QList<Processor*> getOutputProcessor() const;

	//////////////////////////////////////////////////////////////////////////

	QIcon	icon() const;
	void	setIcon(const QString &str);
    
    void    setName(const QString& name);	//!< set name
    QString name() const;					//!< get name

    void    setID(const int &id);			//!< 在一个DesignNetSpace中的唯一ID
    int     id() const;						//!< getter

	DesignNetSpace *space() const{ return m_space; }
	void	setSpace(DesignNetSpace *space) ;

    virtual Core::Id typeID() const;		//!< 返回类型ID
    virtual QString category() const;		//!< 返回种类
    
	//////////////////////////////////////////////////////////////////////////

	bool	addPort(Port::PortType pt, DataType dt, QString sLabel, bool bRemovable = false);
	void	removePort(Port *pPort);
	Port*	getPort(Port::PortType pt, DataType dt);
	Port*	getPort(Port::PortType pt, QString sLabel);
	Port*	getPort(const int iIndex);
	QList<Port*> getPorts(Port::PortType pt) const;
	
	QList<ProcessData*> getData(QString sLabel);
	ProcessData			getOneData(QString sLabel);

	template<class T>
	T getPortData(PortData ptData)
	{
		return getOneData(ptData.strName).variant.value<T>();
	}


	void pushData(ProcessData &pd, QString strLabel);
	void pushData(QVariant &var, DataType dataType, QString strLabel = "", int iProcessId = -1);
	void pushData(IData* data, QString strLabel = "", int iProcessId = -1);

	QList<ProcessData>	getOutputData(DataType dt = DATATYPE_INVALID);
	QList<ProcessData>	getInputData(DataType dt = DATATYPE_INVALID);
	//////////////////////////////////////////////////////////////////////////

	void start();

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
	virtual QString serializableType() const;
	virtual Utils::XmlSerializable* createSerializable() const;
	
	bool isRunning();

	virtual bool prepareProcess() { return true; }

	bool connectTo(Processor* child);
	bool disconnect(Processor* pChild);
	bool isConnectTo(Processor* pChild);
	void detach();

	bool isDataDirty() { return m_bDataDirty; }

	bool isResizableInput() { return m_bResizableInput; }

	QReadWriteLock m_workingLock;

signals:

	void logout(QString log);
	void connected(Processor* father, Processor* pChild);
	void disconnected(Processor* father, Processor* pChild);
	void processorModified();
	void childProcessFinished();
	
	void processStarted();
	void processFinished();

	void portAdded(Port* pPort);
	void portRemoved(Port* pPort);

public slots:

	void run(QFutureInterface<ProcessResult> &future);//!< 线程执行该函数

	void onPropertyChanged_internal();

	void onPortConnected(Port* src, Port* target);
	void onPortDisconnected(Port* src, Port* target);

	void onChildProcessFinished();


protected:

	void notifyDataWillChange();	//!< 通知数据有变化
	void notifyProcess();			//!< 通知处理器处理
	virtual void onNotifyDataChanged();
	virtual void onNotifyProcess();
	virtual void onChildProcessorFinish(Processor* p);

	virtual void propertyChanged(Property *prop);
	virtual void propertyAdded(Property* prop);

	virtual bool beforeProcess(QFutureInterface<ProcessResult> &future);		//!< 处理之前的准备,这里会确保数据已经准备好了
	virtual bool process(QFutureInterface<ProcessResult> &future) = 0;			//!< 正式处理
	virtual void afterProcess(bool status = true);		//!< 完成处理
	virtual bool finishProcess() { return true; }
	
	virtual void onCreateNewPort(Port::PortType pt);

	//////////////////////////////////////////////////////////////////////////

	QIcon			m_icon;
	QString			m_title;				//!< 种类title
	int				m_id;
	ProcessorType	m_eType;
	bool			m_bDataDirty;			//!< 数据有更新

	QList<Port*>	m_outputPort;			//!< 所有的输出端口
	QList<Port*>	m_inputPort;			//!< 输入端口

	QList<Processor*> m_waitProcessors;		//!< 还未收到完成通知的处理器列表 
	ProcessorWorker m_worker;
	DesignNetSpace* m_space;				//!< DesignNetSpace

	bool			m_bResizableInput;		//!< 可变数量的输入

	QFutureWatcher<ProcessResult> m_watcher;	//!< 用于控制进度
	QThread*  m_thread;

	friend class Port;
};
}

#endif // PROCESSOR_H
