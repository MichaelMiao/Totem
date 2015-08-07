#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
	void readSettings();						//!< 读取配置文件并设置大小等信息
	void writeSettings();						//!< 写配置文件，保存窗口位置

	// override
	void closeEvent(QCloseEvent *) override;

private:

	
	QSettings*			m_settings;				//!< 数据库文件

};
#endif // MAINWINDOW_H
