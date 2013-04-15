#ifndef DESIGNNETSPACE_H
#define DESIGNNETSPACE_H

#include <QObject>
#include "processor.h"
#include <QList>
#include <QHash>
#include <QFutureWatcher>
namespace DesignNet{

class DesignNetSpace : public QObject, public Processor
{
    Q_OBJECT
public:
    explicit DesignNetSpace(DesignNetSpace *space = 0, QObject *parent = 0);

    void addProcessor(Processor* processor);    //!< 添加处理器
    void removeProcessor(Processor* processor); //!< 移除处理器
    bool contains(Processor* processor);        //!< 是否包含该Processor
    virtual Processor* create(DesignNetSpace *space = 0) const;          //!< 创建
    virtual QString name() const;               //!< DesignNetSpace名称


    bool connectPort(Port* inputPort, Port* outputPort);//!< 连接两个端口
    bool disconnectPort(Port* inputPort, Port* outputPort);//!< 断开两个端口的链接
    int generateUID();
    virtual void propertyRemoving(Property* prop);//!< 属性移除前调用
    virtual void propertyRemoved(Property* prop); //!< 属性移除完成
	virtual void propertyAdded(Property* prop);
    virtual bool process(QFutureInterface<bool> &fi);                     //!< 处理函数
    virtual Core::Id typeID() const;            //!<
    virtual QString category() const;//!< 返回种类
signals:
    void processorAdded(Processor* processor);
    void processorRemoved(Processor* processor);
    void connectionAdded(Port* inputPort, Port* outputPort);
    void connectionRemoved(Port* inputPort, Port* outputPort);
    void logout(QString log);

public slots:
	void onPropertyChanged_internal();
	void onShowMessage(QString log);
protected:
	virtual void propertyChanged(Property *prop);
    QList<Processor*> m_processors;
	QHash<Processor*, QFutureWatcher<bool>* > m_processorWatchers;//!< 监控着所有正在执行的Processor。
    int     m_iMaxUID;          //!< 记录目前的最大ID，用来产生UID
    bool    m_bProcessing;      //!< 是否正在执行
};
}

#endif // DESIGNNETSPACE_H
