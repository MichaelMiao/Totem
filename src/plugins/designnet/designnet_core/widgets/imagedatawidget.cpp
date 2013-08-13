#include "imagedatawidget.h"
#include "data/imagedata.h"
#include "utils/opencvhelper.h"

#include <QPainter>
#include <QLabel>
namespace DesignNet{

ImageDataWidget::ImageDataWidget(IData *data, QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : IDataWidget(data, false, parent, wFlags)
{
    onUpdate();
}

QRectF ImageDataWidget::boundingRect() const
{
    ImageData *data = (ImageData *)m_data;
    if(data)
    {
        return QRectF(0, 0, data->m_imageMat.cols, data->m_imageMat.rows);
    }
    return IDataWidget::boundingRect();
}

QSizeF ImageDataWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    ImageData *data = (ImageData *)m_data;
    if(data)
    {
        return QSizeF(data->m_imageMat.cols, data->m_imageMat.rows);
    }
    return IDataWidget::sizeHint(which, constraint);
}

void ImageDataWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	onUpdate();
    if(!m_image.isNull())
        painter->drawImage(0, 0, m_image);
	IDataWidget::paint(painter, option, widget);
}

void ImageDataWidget::onUpdate()
{
	prepareGeometryChange();
    ImageData *data = (ImageData *)m_data;
    if(data)
    {
        m_image = Utils::OpenCVHelper::Mat2QImage(data->m_imageMat);
    }
    else
    {
        QImage img;
        m_image = img;
    }
    updateGeometry();
}

void ImageDataWidget::onShowDetail()
{

}

}
