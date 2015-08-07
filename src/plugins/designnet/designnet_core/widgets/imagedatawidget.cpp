#include "imagedatawidget.h"
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include "../data/imagedata.h"
#include "utils/opencvhelper.h"


#define IMAGE_DEFAULT_WIDTH		100

namespace DesignNet{

ImageDataWidget::ImageDataWidget(IData *data, QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : IDataWidget(data, false, parent, wFlags)
{
}

QRectF ImageDataWidget::boundingRect() const
{
	return QRectF(0, 0, IMAGE_DEFAULT_WIDTH, IMAGE_DEFAULT_WIDTH);
}

void ImageDataWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_image.isNull())
        painter->drawImage(0, 0, m_image);
	IDataWidget::paint(painter, option, widget);
}

void ImageDataWidget::onShowDetail()
{

}

void ImageDataWidget::onDataChanged()
{
	ImageData *data = (ImageData *)m_data;
	if(data)
	{
		m_image = Utils::OpenCVHelper::Mat2QImage(data->imageData());
		m_image = m_image.scaled(IMAGE_DEFAULT_WIDTH, IMAGE_DEFAULT_WIDTH, Qt::KeepAspectRatio);
	}
	else
	{
		QImage img;
		m_image = img;
	}
	update();
}

}
