#pragma once

#include <QGraphicsObject>
#include "graphicsui_global.h"


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QParallelAnimationGroup;
class QPropertyAnimation;
QT_END_NAMESPACE
namespace GraphicsUI{

class TOTEM_GRAPHICSUI_EXPORT LoadingICON : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(float iconOpacity READ iconOpacity WRITE setIconOpacity)
	Q_PROPERTY(float radius READ radius WRITE setRadius)

public:

    explicit LoadingICON(QGraphicsItem *parent = 0);

	void	startLoading();
	void	stopLoading();

    float   iconOpacity() { return m_iconOpacity; }
    void    setIconOpacity(float value) { m_iconOpacity = value; update(); }

	float	radius() { return m_radius; }
	void	setRadius(float v) { m_radius = v; update(); }


    virtual QRectF boundingRect () const;
    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget * widget = 0 ) ;

    void setSpeed(int iSpeed = 60);

    void setSize(QSize size);
    QSize size() { return m_size; }

protected:

    int     m_iFadeSpeed;               //!< 淡入淡出速度，默认60ms
    float   m_iconOpacity;              //!< 图标透明度(0~1)
    QSize   m_size;                     //!< Item的大小
	float	m_radius;
    QParallelAnimationGroup *	m_animGroup;
    QPropertyAnimation *		m_animFade;     //!< 淡入淡出动画
	QPropertyAnimation *		m_animZoomIn;     //!< 扩大、缩小
	QPropertyAnimation *		m_animZoomOut;     //!< 扩大、缩小
};
}
