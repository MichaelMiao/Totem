#ifndef GRAPHICSBLOCK_H
#define GRAPHICSBLOCK_H

#include <QGraphicsObject>
#include "portgraphicsitem.h"
#include "../designnet_core_global.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"
QT_BEGIN_NAMESPACE
class QGraphicsTextItem;
QT_END_NAMESPACE

namespace DesignNet{
class Block;
class DESIGNNET_CORE_EXPORT GraphicsBlock : public QGraphicsObject
{
    Q_OBJECT
public:
    enum{DEFAULT_WIDTH = 110,
         DEFAULT_HEIGHT = 150,
        TITLE_HEIGHT = 30,
		ITEMSPACING = 5};
    explicit GraphicsBlock(int iWidth = DEFAULT_WIDTH,
                           int iHeight = DEFAULT_HEIGHT,
                           QGraphicsItem *parent = 0);
    virtual ~GraphicsBlock();
    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget* widget = 0 );

    virtual void layoutItems() = 0; //!< 布局所有的子Item，如Port

    void setTitle(const QString &title);
    virtual QString title() const = 0;    //!< 返回所有title

    QRectF boundingRect() const;
    virtual QSizeF minimumSizeHint() const;
    void setSize(QSizeF size){m_size = size;}
    QSizeF size() const {return m_size;}

    QRectF mainRect() const;        //!< 返回除了Title部分的Main区域大小
signals:
    void closed();
public slots:
    void relayout();
protected:
    virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    Block* m_block;
    QSizeF  m_size;
    GraphicsUI::GraphicsAutoShowHideItem*   m_closeItem;
    QGraphicsTextItem*                      m_titleItem;
};
}//namespace DesignNet

#endif // GRAPHICSBLOCK_H
