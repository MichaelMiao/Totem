#include "a.h"
#include <QTimerEvent>
using namespace GraphicsUI;
A::A(QObject *parent)
	: QObject(parent)
{
	m_block = new TextAnimationBlock();
}

A::~A()
{

}

int A::add( int n, int m )
{
	return m + n;
}

void A::timerEvent( QTimerEvent *event )
{
	static int i = 0;
	QString str = "%1";
	if (i == 3)
	{
	//	killTimer(event->timerId());
	}
	m_block->showText(str.arg(i++));
}
