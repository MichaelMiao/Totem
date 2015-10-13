#pragma once
#include <QSettings>
#include "Common/icontext.h"
#include "Common/settingsdatabase.h"


namespace Core{
class IContext;
}

class MainWindow;
class CORE_EXPORT ICore : public QObject
{
	Q_OBJECT

	explicit ICore();

public:

	static ICore* instance()
	{
		static ICore icore;
		return &icore;
	}
	static QSettings *settings(QSettings::Scope scope = QSettings::UserScope);
	static inline Core::SettingsDatabase *settingsDatabase() { return m_pSettingsDatabase; }
	static void saveSettings();

	~ICore() {}
	
	MainWindow* createMainWindow();
	MainWindow* getFirstMainWindow() { return m_pFirstMainWindow; }
	void initialize();
	void extensionsInitialized();
	
	void insertCenterWidget(QWidget* pWidget);

	QString resourcePath();
	QString userResourcePath();
	
	void addContextObject(Core::IContext *context);
	void removeContextObject(Core::IContext *context);

	

signals:

	void quit();
	void contextAboutToChange(Core::IContext *context);
	void contextChanged(Core::IContext *context, const Core::Context &additionalContexts);

public slots:
	
	void onMainWindowClosed();


private:

	MainWindow*				m_pFirstMainWindow;
	static Core::SettingsDatabase* m_pSettingsDatabase;
};
