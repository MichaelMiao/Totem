#include "designneteditorfactory.h"
#include "designnetconstants.h"
#include "designnetformmanager.h"
#include "designneteditor.h"
#include "coreplugin/ieditor.h"
#include "coreplugin/modemanager.h"
#include "coreplugin/editormanager.h"
#include "coreplugin/infobar.h"
#include "coreplugin/id.h"
namespace DesignNet{
class DesignNetEditorFactoryPrivate
{
public:
	DesignNetEditorFactoryPrivate();
	~DesignNetEditorFactoryPrivate();
	QStringList m_suffixList;
};

DesignNetEditorFactoryPrivate::DesignNetEditorFactoryPrivate()
{
	m_suffixList << _T(Constants::NETEDITOR_FILETYPE);
}

DesignNetEditorFactoryPrivate::~DesignNetEditorFactoryPrivate()
{

}


DesignNetEditorFactory::DesignNetEditorFactory(QObject *parent)
	: Core::IEditorFactory(parent),
	d(new DesignNetEditorFactoryPrivate())
{
	
}

DesignNetEditorFactory::~DesignNetEditorFactory()
{
	delete d;
}

IEditor * DesignNetEditorFactory::createEditor( QWidget *parent )
{
	Core::IEditor * editor = DesignNetFormManager::instance()->createEditor(parent);
	return editor;
}

QStringList DesignNetEditorFactory::suffixTypes() const
{
	return d->m_suffixList;
}

Core::Id DesignNetEditorFactory::id() const
{
	Core::Id id(Constants::NETEDITOR_ID);
	return Constants::NETEDITOR_ID;
}

QString DesignNetEditorFactory::displayName() const
{
	return _T("");
}

IDocument * DesignNetEditorFactory::open( const QString &fileName )
{
	Core::IEditor *iface = Core::EditorManager::openEditor(fileName, id());
	if (!iface)
		return 0;
	if (qobject_cast<DesignNetEditor *>(iface)) 
	{
		Core::InfoBarEntry info((Constants::INFO_READ_ONLY),
			tr("This file can only be edited in <b>Design</b> mode."));
		info.setCustomButtonInfo(tr("Switch mode"), this, SLOT(designNetModeClicked()));
		iface->document()->infoBar()->addInfo(info);
	}
	return iface->document();
}

void DesignNetEditorFactory::designNetModeClicked()
{
	Core::ModeManager::instance()->activateMode(DesignNet::Constants::DESIGNNET_MODE);
}

}