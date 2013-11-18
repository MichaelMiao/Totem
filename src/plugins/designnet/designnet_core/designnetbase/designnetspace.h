#ifndef DESIGNNETSPACE_H
#define DESIGNNETSPACE_H

#include <QObject>
#include "processor.h"
#include "Utils/XML/xmlserializable.h"

#include <QList>
#include <QHash>
#include <QFutureWatcher>
namespace DesignNet{

class Connection : public Utils::XmlSerializable
{
public:
	DECLARE_SERIALIZABLE_NOTYPE(Connection)
	Connection(){m_srcProcessor = -1; m_targetProcessor = -1;}
	Connection(const Connection &c);
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s);
	QString m_srcPort;		//!< 输出端口
	int		m_srcProcessor;
	QString m_targetPort;	//!< 输入端口
	int		m_targetProcessor;
};

class DesignNetSpace : public Processor
{
    Q_OBJECT
public:
	DECLARE_SERIALIZABLE(DesignNetSpace, DesignNetSpace)
    explicit DesignNetSpace(DesignNetSpace *space = 0, QObject *parent = 0);

    void addProcessor(Processor* processor, bool bNotifyModify = false);    //!< 添加处理器
    void removeProcessor(Processor* processor, bool bNotifyModify = false); //!< 移除处理器,并delete
	void detachProcessor(Processor* processor); //!< 取消该处理器所关联的Space

    bool contains(Processor* processor);        //!< 是否包含该Processor
    virtual Processor* create(DesignNetSpace *space = 0) const;          //!< 创建
    virtual QString name() const;               //!< DesignNetSpace名称


	bool connectProcessor(Processor* father, Processor* child);
	bool disconnectProcessor(Processor* father, Processor* child);

    int generateUID();
    virtual void propertyRemoving(Property* prop);//!< 属性移除前调用
    virtual void propertyRemoved(Property* prop); //!< 属性移除完成
	virtual void propertyAdded(Property* prop);

	virtual bool prepareProcess();
    virtual bool process(QFutureInterface<ProcessResult> &future);                     //!< 处理函数
    virtual bool finishProcess();

	virtual Core::Id typeID() const;            //!<
    virtual QString category() const;//!< 返回种类
	QList<Processor*> processors();
	Processor* findProcessor(const int &id);
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
signals:
    void processorAdded(Processor* processor);
    void processorRemoved(Processor* processor);

    void connectionAdded(Processor* father, Processor* pChild);
    void connectionRemoved(Processor* father, Processor* pChild);
	void modified();
	void loadFinished();//!< 载入完成
public slots:
	void onPropertyChanged_internal();
	void onShowMessage(QString log);

protected:
	virtual QList<ProcessData> dataProvided() { return QList<ProcessData>(); }
	
	virtual void propertyChanged(Property *prop);
	bool sortProcessors(QList<Processor*> &processors);// 拓扑排序

    QList<Processor*> m_processors;
	QHash<Processor*, QFutureWatcher<bool>* > m_processorWatchers;//!< 监控着所有正在执行的Processor。
};
}

#endif // DESIGNNETSPACE_H
