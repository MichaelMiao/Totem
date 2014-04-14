#ifndef DESIGNNETEDITOR_H
#define DESIGNNETEDITOR_H

#include "coreplugin/ieditor.h"

namespace Utils{
class XmlDeserializer;
class XmlSerializer;
}
namespace DesignNet{
class DesignNetEditorPrivate;
class DesignNetView;
class DesignNetEditor : public Core::IEditor
{
	Q_OBJECT

public:

	DesignNetEditor(QObject *parent);
	~DesignNetEditor();
	// IEditor
	virtual bool createNew(const QString &contents = QString());
	virtual bool open(QString *errorString, const QString &fileName, const QString &realFileName);
	virtual Core::IDocument *document();
	virtual Core::Id id() const;
	virtual QString displayName() const;
	virtual void setDisplayName(const QString &title);
	
	virtual QByteArray saveState() const;
	virtual bool restoreState(const QByteArray &state);

	virtual bool isTemporary() const;

	virtual QWidget *toolBar();

	Core::Id preferredModeType() const;

	DesignNetView *designNetView();

	bool run();

Q_SIGNALS:

	void designNetFinished();

protected:
	void createCommand();

public slots:
	
	void onDeserialized(Utils::XmlDeserializer &x);
	void onSerialized(Utils::XmlSerializer &s);
	void onFinish();

private:
	DesignNetEditorPrivate *d;
};

}

#endif // DESIGNNETEDITOR_H
