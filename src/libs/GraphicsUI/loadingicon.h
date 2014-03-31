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

    int     m_iFadeSpeed;               //!< ���뵭���ٶȣ�Ĭ��60ms
    float   m_iconOpacity;              //!< ͼ��͸����(0~1)
    QSize   m_size;                     //!< Item�Ĵ�С
	float	m_radius;
    QParallelAnimationGroup *	m_animGroup;
    QPropertyAnimation *		m_animFade;     //!< ���뵭������
	QPropertyAnimation *		m_animZoomIn;     //!< ������С
	QPropertyAnimation *		m_animZoomOut;     //!< ������С
};
}
