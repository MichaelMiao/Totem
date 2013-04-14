#ifndef TEST_H
#define TEST_H

#include <QtGui/QDialog>
#include "ui_test.h"

class test : public QDialog
{
	Q_OBJECT

public:
	test(QWidget *parent = 0, Qt::WFlags flags = 0);
	~test();

private:
	Ui::testClass ui;
};

#endif // TEST_H
