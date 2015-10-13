#pragma once
#include "../../coreplugin/document/ieditor.h"
#include "designnetdocument.h"
#include "designnetview.h"


class DesignNetMainWindow;
class DesignNetEditor : public Core::IEditor
{
	Q_OBJECT
public:

	DesignNetEditor(QObject* pObj, DesignNetMainWindow* pMainWindow);
	~DesignNetEditor() {}

	bool createNew(const QString &contents = QString()) override;
	bool open(QString *errorString, const QString &fileName, const QString &realFileName) override;

	Core::IDocument *document() override;
	Core::Id id() const override;
	QString displayName() const override;
	void setDisplayName(const QString &title) override;

	QByteArray saveState() const override;
	bool restoreState(const QByteArray &state) override;

	bool isTemporary() const override;

	QWidget *toolBar() override { return 0; }

	DesignNetView* view() { return m_pView; }

private:

	DesignNetView*		m_pView;
	DesignNetDocument*	m_pDoc;
};
