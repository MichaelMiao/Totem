#pragma once

#include <QSettings>
#include "mainwindow.h"


class CORE_EXPORT ICore : public QObject
{
	Q_OBJECT

	explicit ICore() {}

public:

	static ICore* instance()
	{
		static ICore icore;
		return &icore;
	}
	~ICore() {}
	
	QSet<QMainWindow*>& getMainWindow() { return m_mainwindowSet; }
	void initialize();
	void extensionsInitialized();
	MainWindow* createMainWindow();

private:

	QSet<QMainWindow*>	m_mainwindowSet;
};
