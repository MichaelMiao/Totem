#include "loadingicon.h"
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>


namespace GraphicsUI
{
	
LoadingICON::LoadingICON(QGraphicsItem *parent /*= 0*/)
	: QGraphicsObject(parent), m_iFadeSpeed(1000)
{
	m_animGroup = new QParallelAnimationGroup(this);
	m_animFade  = new QPropertyAnimation(this, "iconOpacity");
	m_animZoomIn  = new QPropertyAnimation(this, "radius");
	m_animZoomOut  = new QPropertyAnimation(this, "radius");
	QSequentialAnimationGroup *pAniGroup = new QSequentialAnimationGroup(this);
	m_animGroup->addAnimation(m_animFade);
	pAniGroup->addAnimation(m_animZoomIn);
	pAniGroup->addAnimation(m_animZoomOut);
	m_animGroup->addAnimation(pAniGroup);
	m_animZoomIn->setDuration(m_iFadeSpeed);
	m_animZoomOut->setDuration(m_iFadeSpeed);
	m_animFade->setDuration(m_iFadeSpeed);
	m_animFade->setStartValue(150);
	m_animFade->setEndValue(255);
	m_animGroup->setLoopCount(-1);
	m_radius = 0;
	m_iconOpacity = 1;
}

QRectF LoadingICON::boundingRect() const
{
	return QRectF(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
}

void LoadingICON::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */)
{
	QRectF rc = boundingRect();
	QPainterPath p;
	p.addEllipse(rc.center(), m_radius, m_radius);
	QColor clr(Qt::white);
	clr.setAlpha(m_iconOpacity);
	painter->fillPath(p, clr);
	painter->setPen(Qt::white);
	painter->drawEllipse(rc.center(), rc.width() / 2, rc.height() / 2);
}

void LoadingICON::setSpeed(int iSpeed /*= 60*/)
{
	m_animFade->setDuration(m_iFadeSpeed = iSpeed);
}

void LoadingICON::startLoading()
{
	m_animGroup->start();
}

void LoadingICON::stopLoading()
{
	m_animGroup->stop();
	update();
}

void LoadingICON::setSize(QSize size)
{
	m_size = size;
	m_animZoomIn->setStartValue(m_size.height() / 6.0);
	m_animZoomIn->setEndValue(m_size.height() / 4.0);
	m_animZoomOut->setStartValue(m_animZoomIn->endValue());
	m_animZoomOut->setEndValue(m_animZoomIn->startValue());
}

}
