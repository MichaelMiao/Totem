#ifndef DESIGNNETMAINWINDOW_H
#define DESIGNNETMAINWINDOW_H

#include "CustomUI/basemainwindow.h"
namespace Core{
class IMode;
class IEditor;
}

namespace DesignNet{

/**
 * \class	DesignNetMainWindow
 *
 * \brief	the design netÖ÷´°¿Ú
 *
 * \author	Michael_BJFU
 * \date	2013/6/3
 */
class DesignNetMainWindowPrivate;
class DesignNetMainWindow : public CustomUI::BaseMainWindow
{
	Q_OBJECT

public:

	DesignNetMainWindow(QWidget *parent = 0);
	~DesignNetMainWindow();
	void initialize();
	void resetToDefaultLayout();

	void addEditor(Core::IEditor *pEditor);
	void addCenterToolBar(QToolBar* pToolBar);
	CustomUI::DockWindow *createDockWidget(QWidget *widget,
		Qt::DockWidgetArea area = Qt::LeftDockWidgetArea);
	CustomUI::DockWindow *dockWindow(const QString &objectName);
	void addDelayedMenu();
	
public slots:

	void updateDockWidgetSettings();
	void onResetLayout();
	void titleChanged();

private:
	DesignNetMainWindowPrivate *d;
};

}

#endif // DESIGNNETMAINWINDOW_H
