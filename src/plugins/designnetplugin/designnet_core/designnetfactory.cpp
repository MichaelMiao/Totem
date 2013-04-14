#include "designnetfactory.h"
#include "designnetconstants.h"
#include "designneteditor.h"
#include "coreplugin/editormanager.h"
#include "designnetformmanager.h"
namespace DesignNet{
struct DesignNetFactoryPrivate
{
    QStringList m_suffexTypes;
};

DesignNetFactory::DesignNetFactory(QObject *parent) :
    Core::IEditorFactory(parent),
    d(new DesignNetFactoryPrivate)
{
    d->m_suffexTypes << Constants::NETEDITOR_FILETYPE;
}

DesignNetFactory::~DesignNetFactory()
{
    if(d)
    {
        delete d;
        d = 0;
    }
}

Core::IEditor *DesignNetFactory::createEditor(QWidget *parent)
{
    Core::IEditor * editor = DesignNetFormManager::instance()->createEditor(parent);
    return editor;
}

QStringList DesignNetFactory::suffixTypes() const
{
    return d->m_suffexTypes;
}

Core::Id DesignNetFactory::id() const
{
    return DesignNet::Constants::NETEDITOR_ID;
}

QString DesignNetFactory::displayName() const
{
    return DesignNet::Constants::NETEDITOREDITOR_DISPLAYNAME;
}
}
