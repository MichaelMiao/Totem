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
    
	void readSettings();						//!< ��ȡ�����ļ������ô�С����Ϣ
	void writeSettings();						//!< д�����ļ������洰��λ��

	// override
	void closeEvent(QCloseEvent *) override;

private:

	
	QSettings*			m_settings;				//!< ���ݿ��ļ�

};
#endif // MAINWINDOW_H
