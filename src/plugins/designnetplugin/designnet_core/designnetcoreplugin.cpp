#include "designnetcoreplugin.h"
#include "designnetfactory.h"
#include "designnetmainwindow.h"
#include "designmode.h"
#include "coreplugin/modemanager.h"
#include "coreplugin/icore.h"
#include "designnetformmanager.h"
#include "designnetbase/processorfactory.h"
#include "propertymanager.h"
#include "data/datamanager.h"
#include "data/normaldatafactory.h"
#include "normalpropertywidgetfactory.h"
#include "toolmodel.h"
#include "widgets/datadetailwidget.h"
#include "designnetsolutionwizard.h"
#include <QAction>
#include <QtPlugin>
#include <QToolBar>
namespace DesignNet{
DesignNetCorePlugin::DesignNetCorePlugin(QObject *parent)
    : m_designNetFormMgr(new DesignNetFormManager(parent)),
      m_propertyManager(new PropertyManager),
      m_dataManager(new DataManager),
      m_processorFactory(new ProcessorFactory(this)),
      m_model(new ToolModel(this))
{
}

DesignNetCorePlugin::~DesignNetCorePlugin()
{
    if(m_dataManager)
    {
        delete m_dataManager;
        m_dataManager = 0;
    }
    if(m_designNetFormMgr)
    {
        delete m_designNetFormMgr;
        m_designNetFormMgr = 0;
    }
    if(m_designNetFormMgr)
    {
        delete m_designNetFormMgr;
        m_designNetFormMgr = 0;
    }
}

bool DesignNetCorePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    m_designNetMode = new DesignNetMode(this);

    NormalPropertyWidgetFactory *normalPropertyWidgetFactory = new NormalPropertyWidgetFactory(this);
    NormalDataFactory* normalDataFactory = new NormalDataFactory(this);

    connect(Core::ICore::instance(), SIGNAL(saveSettingsRequested()),
            this, SLOT(writeSettings()));

    addAutoReleasedObject(normalPropertyWidgetFactory);
    addAutoReleasedObject(m_designNetMode);
    addAutoReleasedObject(normalDataFactory);
	Core::BaseFileWizardParameters param;
	param.setIcon(QIcon(":/media/DesignNet.png"));
	param.setCategory(tr("DesignNet"));
	param.setDescription(tr("asdf"));
	param.setDisplayCategory(tr("nimeia"));
	param.setDisplayName(tr("miao"));
	addAutoReleasedObject(new DesignNetSolutionWizard(param, this));
    return true;
}

void DesignNetCorePlugin::extensionsInitialized()
{
    DesignNetFactory *factory = new DesignNetFactory(this);
    addAutoReleasedObject(factory);
    m_designNetFormMgr->readSettings();
    m_designNetFormMgr->startInit();
    return;
}

void DesignNetCorePlugin::writeSettings()
{
    m_designNetFormMgr->writeSettings();
}
}
Q_EXPORT_PLUGIN2(DesignNet::DesignNetCorePlugin, DesignNet::DesignNetCorePlugin)
