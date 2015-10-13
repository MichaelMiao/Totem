#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rebar.h"


QT_BEGIN_NAMESPACE
class QSettings;
class QStackedWidget;
class QVBoxLayout;
QT_END_NAMESPACE

namespace Core {
	class EditorManager;
	class DocumentManager;
namespace Internal {
	class ShortcutSettings;
}
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
	void init();
	void readSettings();						//!< 读取配置文件并设置大小等信息
	void writeSettings();						//!< 写配置文件，保存窗口位置
	void insertCenterWidget(QWidget* pWidget);

	Core::IContext *contextObject(QWidget *widget);
	void addContextObject(Core::IContext *contex);
	void removeContextObject(Core::IContext *contex);
	void resetContext();

	// override
	void closeEvent(QCloseEvent *) override;
	void paintEvent(QPaintEvent *) override;

signals:

	void closed();

public slots:

	void aboutTotem();
	void aboutPlugins();
	void showOptionsDialog() { _showOptionsDialog(); }
	void onNewFile();
	void onOpenFile();
	void onSaveFile();
	void onAddRecentFilesMenu();
	void onOpenLastUnclosed();

	void updateFocusWidget(QWidget *old, QWidget *now);

private:
	
	void openFiles(const QStringList &fileNames);
	bool _showOptionsDialog(const QString &category = QString(),
		const QString &page = QString(),
		QWidget *parent = 0);

	void registerDefaultContainers();
	void registerDefaultActions();
	void updateContextObject(Core::IContext *context);
	void updateContext();
	

	QMap<QWidget*, Core::IContext *> m_contextWidgets;
	Core::IContext *	m_activeContext;

	QSettings*			m_settings;				//!< 数据库文件
	RebarCtrl*			m_rebar;
	QStackedWidget*		m_stack;
	Core::Context		m_additionalContexts;
	Core::Internal::ShortcutSettings*	m_shortcutSettings;
	Core::DocumentManager*	m_pDocManager;
	Core::EditorManager* m_pEditorMgr;
	QVBoxLayout*		m_pMainLayout;
};
#endif // MAINWINDOW_H
