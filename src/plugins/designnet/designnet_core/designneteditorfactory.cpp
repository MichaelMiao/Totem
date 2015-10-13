#include "stdafx.h"
#include "designneteditorfactory.h"
#include "designnet_core_def.h"
#include "designneteditor.h"
#include "DesignNetManager.h"


class DesignNetEditorFactoryPrivate
{
public:
	DesignNetEditorFactoryPrivate();
	~DesignNetEditorFactoryPrivate();
	QStringList m_suffixList;
};

DesignNetEditorFactoryPrivate::DesignNetEditorFactoryPrivate()
{
	m_suffixList << DesignNet::Constants::NETEDITOR_FILETYPE;
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

IEditor *DesignNetEditorFactory::createEditor(QWidget *parent)
{
	Core::IEditor * editor = DesignNetManager::instance()->createEditor(parent);
	return editor;
}

QStringList DesignNetEditorFactory::suffixTypes() const
{
	return d->m_suffixList;
}

Core::Id DesignNetEditorFactory::id() const
{
	Core::Id id(DesignNet::Constants::NETEDITOR_ID);
	return DesignNet::Constants::NETEDITOR_ID;
}

QString DesignNetEditorFactory::displayName() const
{
	return tr("");
}

IDocument * DesignNetEditorFactory::open(const QString &fileName)
{
	Core::IEditor *iface = Core::EditorManager::openEditor(fileName, id());
	if (!iface)
		return 0;
	if (qobject_cast<DesignNetEditor *>(iface)) 
	{
		
	}
	return iface->document();
}
