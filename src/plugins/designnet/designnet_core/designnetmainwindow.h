#ifndef DESIGNNETMAINWINDOW_H
#define DESIGNNETMAINWINDOW_H

#include "CustomUI/basemainwindow.h"
namespace Core{
class IMode;
class IEditor;
}

namespace DesignNet{

enum EditState
{
	EditState_Move,
	EditState_Link,
};
/**
 * \class	DesignNetMainWindow
 *
 * \brief	the design net主窗口
 *
 * \author	Michael_BJFU
 * \date	2013/6/3
 */
class DesignNetMainWindowPrivate;
class Processor;
class DesignNetMainWindow : public CustomUI::BaseMainWindow
{
	Q_OBJECT

public:

	DesignNetMainWindow(QWidget *parent = 0);
	~DesignNetMainWindow();
	
	void		initialize();
	void		resetToDefaultLayout();

	void		addEditor(Core::IEditor *pEditor);
	void		addCenterToolBar(QToolBar* pToolBar);
	CustomUI::DockWindow *createDockWidget(QWidget *widget,
		Qt::DockWidgetArea area = Qt::LeftDockWidgetArea);
	CustomUI::DockWindow *dockWindow(const QString &objectName);
	void		addDelayedMenu();
	EditState	getEditState();

public slots:

	void updateDockWidgetSettings();
	void onResetLayout();
	void titleChanged();

	void onConnectAction(bool bChecked);	//!< 开始连接状态
	void onMoveAction(bool bChecked);	//!< 移动状态

	void onEditorChanged(Core::IEditor* pEditor);
	void onEditorClosed(Core::IEditor* pEditor);
	void onShowAvailiableData(Processor* processor);

	void onTabChanged(int iIndex);
	void onTabClose(int iIndex);

	void resizeEvent(QResizeEvent* event) override;

protected:

	void setEditState(EditState eState);

private:
	DesignNetMainWindowPrivate *d;
};

}

#endif // DESIGNNETMAINWINDOW_H
