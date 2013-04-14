#include "designnetspace.h"
#include "utils/totemassert.h"
#include "port.h"
#include <QDebug>
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
namespace DesignNet{

DesignNetSpace::DesignNetSpace(DesignNetSpace *space, QObject *parent) :
    QObject(parent),
    Processor(space),
    m_iMaxUID(0),
    m_bProcessing(false)
{
    connect(this, SIGNAL(logout(QString)), Core::ICore::messageManager(), SLOT(printToOutputPanePopup(QString)));
	m_bReady = true;
}

void DesignNetSpace::addProcessor(Processor *processor)
{
    TOTEM_ASSERT(processor != 0, return);
    bool bret = contains(processor);
    if(bret)
    {
        LOGOUT(tr("can't add the exist processor to the space"));
        return;
    }
    int iUID = generateUID();
    processor->setID(iUID);
    m_processors.push_back(processor);
    emit processorAdded(processor);
}


bool DesignNetSpace::contains(Processor *processor)
{
    return (qFind(m_processors, processor) != m_processors.constEnd());
}

Processor *DesignNetSpace::create(DesignNetSpace *space) const
{
    DesignNetSpace *s = new DesignNetSpace(space, parent());
    return s;
}

QString DesignNetSpace::name() const
{
    return QString("DesignNetSpace");
}

bool DesignNetSpace::connectPort(Port *inputPort, Port *outputPort)
{
    if(outputPort->connect(inputPort))
    {
        emit connectionAdded(inputPort, outputPort);
        return true;
    }
    return false;
}

bool DesignNetSpace::disconnectPort(Port *inputPort, Port *outputPort)
{
    if(outputPort->disconnect(inputPort))
    {
        emit connectionRemoved(inputPort, outputPort);
        return true;
    }
    return false;
}

int DesignNetSpace::generateUID()
{
    return ++m_iMaxUID;
}

void DesignNetSpace::propertyRemoving(Property *prop)
{
}

void DesignNetSpace::propertyRemoved(Property *prop)
{
}

void DesignNetSpace::removeProcessor(Processor *processor)
{
    QList<Processor*>::const_iterator itr = (qFind(m_processors, processor));
    TOTEM_ASSERT(itr != m_processors.end(), qDebug()<< "can't not remove the processor");
    m_processors.removeOne(processor);
    emit processorRemoved(processor);
    delete processor;
}

bool DesignNetSpace::process()
{
    m_bProcessing = true;
    ///
    /// \brief 首先进行拓扑排序
    ///
    QList<Processor*> exclusions;
    QList<Processor*> tempNet = m_processors;
    bool dirty = true;
    while(dirty)
    {
        dirty = false;
        foreach(Processor* processor, tempNet)
        {
            int indegree = processor->indegree(exclusions);
            if(indegree == 0)
            {
                exclusions.append(processor);
                tempNet.removeOne(processor);
                dirty = true;
            }
        }
    }
    if(!tempNet.isEmpty())
    {
        emit logout(tr("The designnet space can't be processed. Maybe there are some circle relationships in the space."));
        return false;
    }
    int failedCount = 0;
    foreach(Processor* processor, exclusions)
    {
        if(!processor->run())
        {
            failedCount++;
        }
    }
    QString log = tr("The designnet space has been processed.");
    if(failedCount > 0)
    {
        log+= tr("But there are(is) %1 procedure(s) failed.");
    }
    emit logout(log);
    m_bProcessing = false;
    return failedCount == 0;
}

Core::Id DesignNetSpace::typeID() const
{
    return Core::Id(QLatin1String("DesignNetSpace/") + name());
}

QString DesignNetSpace::category() const
{
    return tr("DesignNetSpace");
}

void DesignNetSpace::onShowMessage(QString log)
{
    emit logout(log);
}

void DesignNetSpace::onPropertyChanged_internal()
{
	Property *prop = qobject_cast<Property*>(sender());
	if(prop)
		propertyChanged(prop);
}

void DesignNetSpace::propertyChanged( Property *prop )
{
	
}

void DesignNetSpace::propertyAdded( Property* prop )
{
	QObject::connect(prop, SIGNAL(changed()), this, SLOT(onPropertyChanged_internal()));	
}

}
