#ifndef IMAGELISTAUOTVIEW_H
#define IMAGELISTAUOTVIEW_H

#include "graphicsui_global.h"

#include <QGraphicsView>
#include <QStringList>
#include <QTimer>
#include <QList>
#include <QGraphicsPixmapItem>

QT_BEGIN_NAMESPACE
class QImage;
class QGraphicsScene;
class QParallelAnimationGroup;
class QPropertyAnimation;
QT_END_NAMESPACE


namespace GraphicsUI{

class TOTEM_GRAPHICSUI_EXPORT ImageItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    ImageItem(const QPixmap &pixmap, const QString &strLabel = tr(""),
              QString brief = QObject::tr(""));

    void fadeIn();

    void fadeOut();

private:
    int m_speed;            //!< 速度
    QString m_strBrief;     //!< 简要说明
    QString m_strLabel;
    QParallelAnimationGroup *m_animGroup;
    QPropertyAnimation *m_animFade;//!< 淡入淡出动画
};

class TOTEM_GRAPHICSUI_EXPORT ImageListAutoView : public QGraphicsView
{
    Q_OBJECT
public:
    enum TYPE{FADE, SCROLL};
    explicit ImageListAutoView(QWidget *parent = 0);

    void pushImages(const QStringList &strList);
    void pushImage(const QString &str, const QString &brief = tr(""));

    void showImage(const int &iIndex);
    void reset();
    void fitView();

    void startToShow();//开始显示列表
protected:
    virtual void resizeEvent(QResizeEvent * event);
    void imageAdded();
    void bringToFront(QGraphicsItem* pItem);
signals:

public slots:
    void onTimeOut();
private:
    QList<ImageItem*> m_imageList;
    QTimer *m_timer;
    int m_interval;//切换图片的间隔
    QGraphicsScene *m_scene;
    int m_iCurrentIndex;
};
}


#endif // IMAGELISTAUOTVIEW_H
