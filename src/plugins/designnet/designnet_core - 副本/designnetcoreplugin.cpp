#include "designnetcoreplugin.h"
#include <QtPlugin>
#include "../../coreplugin/editormanager.h"
#include "../../coreplugin/icore.h"
#include "data/datamanager.h"
#include "extensionsystem/pluginmanager.h"
#include "designneteditorfactory.h"
#include "designnetformmanager.h"
#include "designnetmode.h"
#include "designnetsolutionwizard.h"
#include "DesignNetUserMode.h"
#include "normaldatafactory.h"
#include "normalpropertywidgetfactory.h"
#include "processorfactory.h"
#include "propertymanager.h"
#include "toolmodel.h"



using namespace ExtensionSystem;
using namespace Core;
namespace DesignNet{
class DesignNetCorePluginPrivate
{
public:
	DesignNetCorePluginPrivate();
	~DesignNetCorePluginPrivate();
	DesignNetMode *				m_mode;
	DesignNetUserMode*			m_userMode;
	DesignNetFormManager *		m_designNetFormMgr;
	DataManager*				m_dataManager;
	ToolModel*					m_toolModel;
	PropertyManager*			m_propertyManager;
};

DesignNetCorePluginPrivate::DesignNetCorePluginPrivate()
{
	m_mode			= 0;
	m_userMode		= 0;
	m_dataManager	= new DataManager();
	m_propertyManager = new PropertyManager;
}

DesignNetCorePluginPrivate::~DesignNetCorePluginPrivate()
{
	delete m_dataManager;
	delete m_propertyManager;
}


DesignNetCorePlugin::DesignNetCorePlugin()
	: d(new DesignNetCorePluginPrivate)
{
	d->m_toolModel = new ToolModel(this);
}

DesignNetCorePlugin::~DesignNetCorePlugin()
{
	delete d;
	DesignNetFormManager::Release();
}

bool DesignNetCorePlugin::initialize( const QStringList &arguments, QString *errorMessage /*= 0*/ )
{
	d->m_mode = new DesignNetMode(this);
	d->m_userMode = new DesignNetUserMode(this);
	NormalPropertyWidgetFactory *normalPropertyWidgetFactory = new NormalPropertyWidgetFactory(this);
	NormalDataFactory* normalDataFactory = new NormalDataFactory(this);
	addAutoReleasedObject(normalPropertyWidgetFactory);
	addAutoReleasedObject(normalDataFactory);

	Core::BaseFileWizardParameters param;
	param.setIcon(QIcon(":/media/DesignNet.png"));
	param.setCategory(tr("DesignNet"));
	param.setDescription(tr("DesignNet about pattern recognition."));
	param.setDisplayCategory(tr("DesignNet File"));
	param.setDisplayName(tr("miao"));
	addAutoReleasedObject(d->m_mode);
	addAutoReleasedObject(d->m_userMode);
	addAutoReleasedObject(new ProcessorFactory(this));
	addAutoReleasedObject(new DesignNetSolutionWizard(param, this));
	addAutoReleasedObject(new DesignNetEditorFactory);
	// Core
	connect(ICore::instance(), SIGNAL(saveSettingsRequested()), this, SLOT(writeSettings()));
	d->m_designNetFormMgr = DesignNetFormManager::instance();
	d->m_designNetFormMgr->startInit();
	return true;
}

void DesignNetCorePlugin::extensionsInitialized()
{
	
}

bool DesignNetCorePlugin::delayedInitialize()
{
	return true;
}

IPlugin::ShutdownFlag DesignNetCorePlugin::aboutToShutdown()
{
	return IPlugin::SynchronousShutdown;
}

void DesignNetCorePlugin::writeSettings()
{

}

}