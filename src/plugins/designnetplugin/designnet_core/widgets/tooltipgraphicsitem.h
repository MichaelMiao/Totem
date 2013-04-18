#ifndef TOOLTIPGRAPHICSITEM_H
#define TOOLTIPGRAPHICSITEM_H

#include <QGraphicsWidget>
#include <QGraphicsTextItem>
#include "designnet_core_global.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "graphicsitem/portgraphicsitem.h"

namespace DesignNet{
class ToolTipGraphicsItemPrivate;
class IData;
/*!
 * \brief The ToolTipGraphicsItem class DesignView中显示的提示
 *
 * 该提示具有能够显示指定数据类型的数据的能力
 */
class DESIGNNET_CORE_EXPORT ToolTipGraphicsItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    ToolTipGraphicsItem(PortGraphicsItem *parent);
    void setText(const QString& text);
    QRectF boundingRect () const;
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem *item,
                            QWidget *w = 0);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * event);
    QVariant itemChange ( GraphicsItemChange change, const QVariant & value ) ;
    bool topmost() const;
    void setData(IData* data);
    
public slots:
    void onClosed();
    void onTopMost(bool topmost);
	void relayout(); //!< 重新布局
protected:
    ToolTipGraphicsItemPrivate *d;
};
}

#endif // TOOLTIPGRAPHICSITEM_H
