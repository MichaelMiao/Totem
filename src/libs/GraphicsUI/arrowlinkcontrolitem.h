#ifndef ARROWLINKCONTROLITEM_H
#define ARROWLINKCONTROLITEM_H

#include <QGraphicsObject>
#include "graphicsui_global.h"

namespace GraphicsUI{

class TOTEM_GRAPHICSUI_EXPORT ArrowLinkControlItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ArrowLinkControlItem(QGraphicsItem *parent = 0);
    virtual QRectF	boundingRect () const ;
    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option,
                       QWidget * widget = 0 );
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value );
signals:
    void positionChanged();
};
}

#endif // ARROWLINKCONTROLITEM_H
