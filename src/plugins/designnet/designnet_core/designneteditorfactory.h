#ifndef DESIGNNETEDITORFACTORY_H
#define DESIGNNETEDITORFACTORY_H

#include "coreplugin/ieditorfactory.h"
using namespace Core;
namespace Core{
class IEditor;
}
namespace DesignNet{
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
private slots:
	void designNetModeClicked();
private:
	DesignNetEditorFactoryPrivate *d;
};

}

#endif // DESIGNNETEDITORFACTORY_H
