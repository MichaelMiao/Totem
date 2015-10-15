#pragma once
#include <QtQuickWidgets/QQuickWidget>
#include <QStackedWidget>
#include "../../../libs/CustomUI/basemainwindow.h"
#include "../../coreplugin/Common/icontext.h"
#include "../../coreplugin/document/editormanager.h"


class DesignNetEditor;
class DesignNetMainWindow : public CustomUI::BaseMainWindow
{
	Q_OBJECT

public:

	DesignNetMainWindow(QWidget *parent);
	~DesignNetMainWindow(){}

	void initialize();

	void addEditor(DesignNetEditor* pEditor);

public slots:

private:

	QTabWidget*		m_pTabWidget;
	QStackedWidget*	m_pStack;
	Core::Context	m_context;
};
