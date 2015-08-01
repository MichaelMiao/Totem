#include "designnetmode.h"
#include "designnetconstants.h"
#include "coreplugin/coreconstants.h"
#include "coreplugin/icorelistener.h"
#include "coreplugin/ieditor.h"
#include "coreplugin/icontext.h"
#include "extensionsystem/pluginmanager.h"
#include "coreplugin/editormanager.h"
#include "coreplugin/modemanager.h"
#include "coreplugin/icore.h"
#include <QStackedWidget>
#include <QPushButton>
namespace CC = Core::Constants;


using namespace Core;
namespace DesignNet{
namespace Internal{
class DesignNetModeCoreListener : public Core::ICoreListener
{
public:
    DesignNetModeCoreListener(DesignNetMode* mode);
    bool coreAboutToClose();
private:
    DesignNetMode *m_mode;
};

DesignNetModeCoreListener::DesignNetModeCoreListener(DesignNetMode *mode)
    : m_mode(mode)
{
}

bool DesignNetModeCoreListener::coreAboutToClose()
{
    m_mode->currentEditorChanged(0);
    return true;
}

}//namespace Internal

struct DesignEditorInfo
{
    int widgetIndex;
    QStringList suffixTypes;
    Context context;
    QWidget *widget;
};

//--DesignNetModePrivate

class DesignNetModePrivate
{
public:
    explicit DesignNetModePrivate(DesignNetMode *q);
    ~DesignNetModePrivate();
public:
    Internal::DesignNetModeCoreListener *m_coreListener;
    QWeakPointer<Core::IEditor> m_currentEditor;
    bool m_isActive;
    QList<DesignEditorInfo*> m_editors;
    QStackedWidget *m_stackWidget;
    Context m_activeContext;
};

DesignNetModePrivate::DesignNetModePrivate(DesignNetMode *q)
    : m_coreListener(new Internal::DesignNetModeCoreListener(q)),
      m_isActive(false),
      m_stackWidget(new QStackedWidget)
{

}

DesignNetModePrivate::~DesignNetModePrivate()
{
    delete m_stackWidget;
    m_stackWidget = 0;
}

//-----------------------------------------------
static DesignNetMode *m_instance = 0;
DesignNetMode::DesignNetMode(QObject *parent) :
    Core::IMode(parent),
    d(new DesignNetModePrivate(this))
{
    m_instance = this;
    setObjectName(_T(Constants::DESIGNNET_MODE));
    setIcon(QIcon(_T(Constants::DESIGNNET_ICON)));
    setContext(Context(Constants::DESIGNNET_MODE));
    setEnabled(false);
    setDisplayName(_T(Constants::DESIGNNET_MODE_DISPLAYNAME));
    setPriority(80);
    setId(Constants::DESIGNNET_MODE);
    setType(Constants::DESIGNNET_MODETYPE);
	setWidget(d->m_stackWidget);
    ExtensionSystem::PluginManager::instance()->addObject(d->m_coreListener);

    connect(EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(currentEditorChanged(Core::IEditor*)));
    connect(ModeManager::instance(), SIGNAL(currentModeChanged(Core::IMode*,Core::IMode*)),
            this, SLOT(updateContext(Core::IMode*,Core::IMode*)));

}

DesignNetMode::~DesignNetMode()
{
    ExtensionSystem::PluginManager::instance()->removeObject(d->m_coreListener);
    delete d->m_coreListener;

    qDeleteAll(d->m_editors);
    delete d;
}

DesignNetMode *DesignNetMode::instance()
{
    return m_instance;
}

void DesignNetMode::registerDesignWidget(QWidget *widget, const QStringList &suffixTypes, const Context &context)
{
    int index = d->m_stackWidget->addWidget(widget);
    DesignEditorInfo *info = new DesignEditorInfo;
    info->suffixTypes = suffixTypes;
    info->context = context;
    info->widgetIndex = index;
    info->widget = widget;
    d->m_editors.append(info);
}

void DesignNetMode::unregisterDesignWidget(QWidget *widget)
{
    d->m_stackWidget->removeWidget(widget);
    foreach(DesignEditorInfo *info, d->m_editors)
    {
        if (info->widget == widget)
        {
            d->m_editors.removeAll(info);
            break;
        }
    }
}

QStringList DesignNetMode::registeredMimeTypes() const
{
    QStringList rc;
    foreach(const DesignEditorInfo *i, d->m_editors)
        rc += i->suffixTypes;
    return rc;
}

void DesignNetMode::currentEditorChanged(Core::IEditor *editor)
{
    if(editor && d->m_currentEditor.data() == editor)
        return ;
    bool bEditorAvailable = false;

    if (editor && editor->document())
    {
        const QString suffixType = editor->document()->suffixType();
        if (!suffixType.isEmpty())
        {
            foreach (DesignEditorInfo *editorInfo, d->m_editors)
            {
                foreach (const QString &suffix, editorInfo->suffixTypes)
                {
                    if (suffix == suffixType)
                    {
                        d->m_stackWidget->setCurrentIndex(editorInfo->widgetIndex);
                        setActiveContext(editorInfo->context);
                        bEditorAvailable = true;
                        setEnabled(true);
                        break;
                    }
                } // foreach mime
                if (bEditorAvailable)
                    break;
            } // foreach editorInfo
        }
    }
    //断开原始的信号槽连接
    if (d->m_currentEditor)
        disconnect(d->m_currentEditor.data(), SIGNAL(changed()), this, SLOT(updateActions()));

    if (!bEditorAvailable)
    {
        setActiveContext(Context());
        setEnabled(false);
        d->m_currentEditor = QWeakPointer<Core::IEditor>();
        emit actionsUpdated(d->m_currentEditor.data());
    }
    else
    {
        d->m_currentEditor = QSharedPointer<Core::IEditor>(editor);

        if (d->m_currentEditor)
            connect(d->m_currentEditor.data(), SIGNAL(changed()), this, SLOT(updateActions()));

        emit actionsUpdated(d->m_currentEditor.data());
    }
}

void DesignNetMode::updateContext(IMode *newMode, IMode *oldMode)
{
    if (newMode == this)
    {
        Core::ICore::updateAdditionalContexts(Context(), d->m_activeContext);
    }
    else if (oldMode == this)
    {
        Core::ICore::updateAdditionalContexts(d->m_activeContext, Context());
    }
}

void DesignNetMode::updateActions()
{

}

void DesignNetMode::setActiveContext(const Context &context)
{
    if (d->m_activeContext == context)
        return;

    if (ModeManager::currentMode() == this)
        Core::ICore::updateAdditionalContexts(d->m_activeContext, context);

    d->m_activeContext = context;
}
}
