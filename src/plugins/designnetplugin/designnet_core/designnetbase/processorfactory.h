#ifndef PROCESSORFACTORY_H
#define PROCESSORFACTORY_H

#include <QObject>
#include <QHash>
namespace DesignNet{
class Processor;
class DesignNetSpace;
class ProcessorFactory : public QObject
{
    Q_OBJECT
public:
    explicit ProcessorFactory(QObject *parent = 0);
    /**
     * @brief instance
     *
     * 调用该函数之前需要调用new ProcessFactory;
     * @return m_instance,
     */
    static ProcessorFactory* instance();

    Processor* create(DesignNetSpace *space, const QString &processorName);
    void initialize();                            //!< 初始化
    void registerProcessor(Processor* processor) const; //!< 注册Processor
signals:

public slots:
protected:
    static ProcessorFactory* m_instance;
    mutable QHash<QString, Processor*> m_processors;
};
}

#endif // PROCESSORFACTORY_H
