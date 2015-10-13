#pragma once
#include "../../coreplugin/document/ieditorfactory.h"


using namespace Core;
namespace Core{
class IEditor;
}
class DesignNetEditorFactoryPrivate;
class DesignNetEditorFactory : public Core::IEditorFactory
{
	Q_OBJECT

public:

	DesignNetEditorFactory(QObject *parent = 0);
	~DesignNetEditorFactory();
	virtual IEditor *createEditor(QWidget *parent);
	virtual QStringList suffixTypes() const;
	virtual Id id() const;
	virtual QString displayName() const;
	virtual IDocument *open(const QString &fileName);

private:

	DesignNetEditorFactoryPrivate *d;
};