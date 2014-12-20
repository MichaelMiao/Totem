#include "totem_gui_pch.h"
#include "pluginloaderview.h"
#include "GraphicsUI/imagelistautoview.h"

#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

const char STR_IMAGE1[] = ":/app/images/Hydrangeas.jpg";
const char STR_IMAGE2[] = ":/app/images/Jellyfish.jpg";
const char STR_IMAGE3[] = ":/app/images/Koala.jpg";
const char STR_IMAGE4[] = ":/app/images/Lighthouse.jpg";
const char STR_IMAGE5[] = ":/app/images/Penguins.jpg";

/*!
 * \brief PluginLoaderView构造函数，完成数据创建与布局
 *
 * 添加了五张图片，要想修改仅仅需要通过m_view的pushImages来完成即可。
 * 该窗口为FrameLess类型的窗口。默认大小为256*256.
 * \param[in] parent 父窗口
 */
PluginLoaderView::PluginLoaderView(QWidget *parent) :
    FutureProgress(parent)
{
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);
	setKeepOnFinish(HideOnFinish);
    m_view = new GraphicsUI::ImageListAutoView(this);
    QVBoxLayout *vLayout = new QVBoxLayout;
    
    QStringList imageList;
    imageList << QLatin1String(STR_IMAGE1)
              << QLatin1String(STR_IMAGE2)
              << QLatin1String(STR_IMAGE3)
              << QLatin1String(STR_IMAGE4)
              << QLatin1String(STR_IMAGE5);
    m_view->pushImages(imageList);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	vLayout->addWidget(m_view);
	QWidget *widget = new QWidget(this);
    widget->setLayout(vLayout);
	setWidget(widget);
    resize(500, 500);
}
/*!
 * 该窗口的初始化，没有做任何操作
 */
void PluginLoaderView::init()
{

}
/*!
 * \brief 槽函数，用来报告已经开始加载插件
 *
 * 显示图片，并设置文本为Loading+进度
 */
void PluginLoaderView::setStarted()
{
    m_view->startToShow();
    setProgressText(tr("Loading...%1%").arg(0));
}
/*!
 * 槽函数，报告插件加载完毕
 */
void PluginLoaderView::setFinished()
{
    setProgressText(tr("Load plugin Finished"));
}
