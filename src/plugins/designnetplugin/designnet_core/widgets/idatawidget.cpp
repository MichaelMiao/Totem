#include "idatawidget.h"
namespace DesignNet {

IDataWidget::IDataWidget(IData *data, QGraphicsItem *parent, Qt::WindowFlags wFlags) :
    QGraphicsWidget(parent, wFlags),m_data(data)
{
    connect(data, SIGNAL(dataChanged()), this ,SLOT(updateData()));
}

QRectF IDataWidget::boundingRect() const
{
    return QRectF(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

void IDataWidget::updateData()
{
    onUpdate();
}

QSizeF IDataWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(which)
    Q_UNUSED(constraint)
    return QSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

}
