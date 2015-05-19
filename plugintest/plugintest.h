#ifndef PLUGINTEST_H
#define PLUGINTEST_H

#include <QtWidgets/QWidget>

class plugintest : public QWidget
{
	Q_OBJECT

public:
	plugintest(QWidget *parent = 0);
	~plugintest();

};

#endif // PLUGINTEST_H
