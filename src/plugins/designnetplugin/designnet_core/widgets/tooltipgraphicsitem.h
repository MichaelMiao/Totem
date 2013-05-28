#ifndef TOOLTIPGRAPHICSITEM_H
#define TOOLTIPGRAPHICSITEM_H

#include <QGraphicsObject>
#include <QGraphicsTextItem>
#include "designnet_core_global.h"

namespace DesignNet{
class ToolTipGraphicsItemPrivate;
class IData;
class PortGraphicsItem;
/*!
 * \brief The ToolTipGraphicsItem class DesignView中显示的提示
 *
 * 该提示具有能够显示指定数据类型的数据的能力
 */
class DESIGNNET_CORE_EXPORT ToolTipGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    ToolTipGraphicsItem(PortGraphicsItem *parent);
	virtual ~ToolTipGraphicsItem();
    void setText(const QString& text);
    QRectF boundingRect () const;
    virtual void	paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    QVariant itemChange ( GraphicsItemChange change, const QVariant & value ) ;
    bool topmost() const;
    void setData(IData* data);
	virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );
public slots:
    void onClosed();
    void onTopMost(bool topmost);
	void relayout(); //!< 重新布局
	void onVisibleChanged();
protected:
    ToolTipGraphicsItemPrivate *d;
};
}

#endif // TOOLTIPGRAPHICSITEM_H
