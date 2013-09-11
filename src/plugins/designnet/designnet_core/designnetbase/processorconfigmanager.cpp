#include "processorconfigmanager.h"
#include "processor.h"
#include "widgets/processorconfigwidget.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
#include <QHash>
using namespace Core;
namespace DesignNet{
class ProcessorConfigManagerPrivate
{
public:
	ProcessorConfigManagerPrivate();
	QHash<QString, ProcessorConfigWidget*> m_processorConfigWidgets;
};

ProcessorConfigManagerPrivate::ProcessorConfigManagerPrivate()
{

}


ProcessorConfigManager *ProcessorConfigManager::m_instance = 0;
ProcessorConfigManager::ProcessorConfigManager(QObject *parent)
	: QObject(parent),
	d(new ProcessorConfigManagerPrivate())
{

}

ProcessorConfigManager::~ProcessorConfigManager()
{
	delete d;
	d = 0;
}

ProcessorConfigManager* ProcessorConfigManager::instance()
{
	if (!m_instance)
	{
		m_instance = new ProcessorConfigManager; 
	}
	return m_instance;
}

void ProcessorConfigManager::Release()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = 0;
	}
}

void ProcessorConfigManager::registerConfigWidget( const QString &processorId, ProcessorConfigWidget *widget )
{
	if(d->m_processorConfigWidgets.find(processorId) == d->m_processorConfigWidgets.end())
	{
		d->m_processorConfigWidgets.insert(processorId, widget);
	}
	else
	{
		LOGOUT(tr("Processor %1 has been registered!").arg(processorId));
	}
}

ProcessorConfigWidget* ProcessorConfigManager::createConfigWidget( Processor* processor, QWidget *parent )
{
	ProcessorConfigWidget* widget = d->m_processorConfigWidgets.value(processor->typeID().toString(), 0);
	if (widget)
	{
		widget = widget->create(processor, parent);
		widget->setWindowTitle(tr("%1-%2-ID:%3").arg(widget->windowTitle()).arg(processor->name()).arg(processor->id()));
		widget->setAttribute(Qt::WA_DeleteOnClose);
		widget->init();
		return widget;
	}
	return 0;
}

}
