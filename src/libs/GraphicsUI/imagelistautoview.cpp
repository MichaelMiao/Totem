#include "totem_gui_pch.h"
#include "imagelistautoview.h"

#include <QGraphicsScene>
#include <QApplication>
#include <QDebug>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

namespace GraphicsUI{


const int iViewWidth = 400;
const int iViewHeight = 400;
const int iDefualtFadeSpeed = 500;

ImageListAutoView::ImageListAutoView(QWidget *parent) :
    QGraphicsView(parent), m_interval(3000), m_iCurrentIndex(-1)
{
    m_timer = new QTimer(this);
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    m_timer->setInterval(m_interval);
    m_scene->setSceneRect(0, 0, iViewWidth, iViewHeight);
    this->resize(iViewWidth, iViewHeight);
    setCacheMode(QGraphicsView::CacheBackground);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ImageListAutoView::pushImages(const QStringList &strList)
{
    foreach(QString str, strList)
    {
        pushImage(str);
    }
}

void ImageListAutoView::pushImage(const QString &strFile, const QString &brief)
{
    QImage img(strFile);
    if(!img.isNull())
    {
        ImageItem *item = new ImageItem(QPixmap::fromImage(img), strFile, brief);
        item->setOffset(-0.5*item->boundingRect().width(),
                        -0.5*item->boundingRect().height());
        if (m_imageList.count() == 0)
        {
            item->setOpacity(1);
            m_iCurrentIndex = 0;
        }
        else
            item->setOpacity(0);
        m_imageList.push_back(item);

        m_scene->addItem(item);
    }
}

void ImageListAutoView::showImage(const int &iIndex)
{
    if(iIndex > 0 && m_imageList.size() > iIndex)
    {
        ImageItem *item = m_imageList.at(iIndex);
        if(m_iCurrentIndex == -1)
        {
            m_iCurrentIndex = iIndex;
            fitView();
            return;
        }
        bringToFront(item);
        m_iCurrentIndex = iIndex;
       
    }
	 fitView();
}

void ImageListAutoView::reset()
{
    foreach(ImageItem *item, m_imageList)
    {
        delete item;
    }
    m_imageList.clear();
    m_iCurrentIndex = 0;
}

void ImageListAutoView::fitView()
{
    QRectF rect;
    if(m_iCurrentIndex >= 0)
    {
        rect = (m_imageList[m_iCurrentIndex]->boundingRect());
    }
    fitInView(rect, Qt::KeepAspectRatio);
    setSceneRect(rect);
}

void ImageListAutoView::startToShow()
{
    if(m_timer->isActive())
        m_timer->stop();
    m_timer->start();
}

void ImageListAutoView::resizeEvent(QResizeEvent *event)
{
    fitView();
}

void ImageListAutoView::onTimeOut()
{
    if (m_imageList.count() <= 1)
            return;

    const int next = (m_iCurrentIndex+1) % m_imageList.count();
    m_imageList[m_iCurrentIndex]->fadeOut();
	m_iCurrentIndex = next;
	fitView();
    m_imageList[next]->fadeIn();

}

void ImageListAutoView::bringToFront(QGraphicsItem *pItem)
{
    if(pItem)
    {
        QList<QGraphicsItem *> list = pItem->collidingItems();
        qreal zValue = 0;
        foreach (QGraphicsItem *item, list)
        {
            if (item->zValue() >= zValue)
                zValue = item->zValue() + 0.1;
        }
		
        pItem->setZValue(zValue);
        fitInView(pItem);
    }
}

ImageItem::ImageItem(const QPixmap &pixmap,
                     const QString &strLabel,
                     QString brief):
    QGraphicsPixmapItem(pixmap),
    m_strLabel(strLabel),
    m_strBrief(brief)
{
    setToolTip(brief.isEmpty() ? strLabel : brief);
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
    m_animGroup = new QParallelAnimationGroup(this);
    m_animFade  = new QPropertyAnimation(this, "opacity");
    m_animGroup->addAnimation(m_animFade);
    m_animFade->setDuration(iDefualtFadeSpeed);
}

void ImageItem::fadeIn()
{
    m_animFade->setStartValue(opacity());
    m_animFade->setEndValue(1);
    m_animGroup->start();
}

void ImageItem::fadeOut()
{
    m_animFade->setStartValue(opacity());
    m_animFade->setEndValue(0);
    m_animGroup->start();
}

}