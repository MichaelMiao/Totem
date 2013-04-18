#ifndef IMAGEDATAWIDGET_H
#define IMAGEDATAWIDGET_H
#include "idatawidget.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT ImageDataWidget : public IDataWidget
{
    Q_OBJECT
public:
    ImageDataWidget(IData *data, QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = Qt::FramelessWindowHint);
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint(Qt::SizeHint which,
                            const QSizeF & constraint = QSizeF()) const;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
protected:
    virtual void onUpdate();
    QImage m_image;
};
}

#endif // IMAGEDATAWIDGET_H
