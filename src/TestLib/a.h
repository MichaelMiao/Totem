#ifndef A_H
#define A_H

#include <QObject>
#include "GraphicsUI/textanimationblock.h"
#include "GraphicsUI/imagelistautoview.h"
using namespace GraphicsUI;

class A : public QObject
{
	Q_OBJECT

public:
	A(QObject *parent);
	~A();

public slots:
	int add(int n, int m);
protected:
	void timerEvent(QTimerEvent *event);
public:
	TextAnimationBlock *m_block;
	ImageListAutoView *m_autoView;
};

#endif // A_H
