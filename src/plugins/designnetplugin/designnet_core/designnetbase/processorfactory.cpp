#include "processorfactory.h"
#include "utils/totemassert.h"
#include "processor.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
namespace DesignNet{

ProcessorFactory* ProcessorFactory::m_instance = 0;
ProcessorFactory::ProcessorFactory(QObject *parent) :
    QObject(parent)
{
    m_instance = this;
}

ProcessorFactory *ProcessorFactory::instance()
{
    return m_instance;
}

Processor *ProcessorFactory::create(DesignNetSpace *space, const QString &processorName)
{
    QHash<QString, Processor*>::iterator itr = m_processors.find(processorName);
    if(itr == m_processors.end())
    {
        LOGOUT(tr("Processor %1 hasn't been registered!").arg(processorName));
        return 0;
    }
    else
    {
        Processor* processor = itr.value();
        return processor->create(space);
    }
}

void ProcessorFactory::initialize()
{

}

void ProcessorFactory::registerProcessor(Processor *processor) const
{
    TOTEM_ASSERT(processor, qDebug() << "Can't use null processor point");
    QString name = processor->typeID().toString();
    if (m_processors.find(name) == m_processors.end())
    {
        m_processors.insert(name, processor);
    }
    else
    {
       LOGOUT(tr("Processor %1 has been registered!").arg(name));
    }
}

}
