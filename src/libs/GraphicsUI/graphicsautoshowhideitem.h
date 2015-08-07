#ifndef GRAPHICSAUTOSHOWHIDEITEM_H
#define GRAPHICSAUTOSHOWHIDEITEM_H

#include <QGraphicsObject>
#include <QPixmap>
#include "graphicsui_global.h"
QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QParallelAnimationGroup;
class QPropertyAnimation;
QT_END_NAMESPACE
namespace GraphicsUI{

class TOTEM_GRAPHICSUI_EXPORT GraphicsAutoShowHideItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(float iconOpacity READ iconOpacity WRITE setIconOpacity)
    Q_PROPERTY(bool autoHide READ hasAutoHide WRITE setAutoHide)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)

public:
    explicit GraphicsAutoShowHideItem(QGraphicsItem *parent = 0);

    float   iconOpacity() { return m_iconOpacity; }
    void    setIconOpacity(float value) { m_iconOpacity = value; update(); }

    void    setAutoHide(bool hide) { m_autoHide = hide; }
    bool    hasAutoHide() const { return m_autoHide; }

    void setPixmap(const QPixmap &pixmap);
    QPixmap pixmap();

    void    animateShow(bool visible);

    virtual QRectF boundingRect () const;
    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget * widget = 0 ) ;

    void mousePressEvent( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);



    void setSpeed(int iSpeed = 60);

    void setSize(QSize size){m_size = size;}
    QSize size(){return m_size;}
signals:
    void clicked();
protected:
    bool    m_autoHide;                 //!< 是否自动隐藏
    int     m_iFadeSpeed;               //!< 淡入淡出速度，默认60ms
    float   m_iconOpacity;              //!< 图标透明度(0~1)
    bool    m_bEableButtonStyle;        //!< 是否使用按钮风格
    QPixmap m_pixmap;                   //!< 显示的图标
    QSize   m_size;                     //!< Item的大小
    QParallelAnimationGroup *m_animGroup;
    QPropertyAnimation *m_animFade;     //!< 淡入淡出动画
};
}

#endif // GRAPHICSAUTOSHOWHIDEITEM_H
