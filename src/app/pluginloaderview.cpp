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
    m_view = new GraphicsUI::ImageListAutoView(this);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(m_view);
    QStringList imageList;
    imageList << QLatin1String(STR_IMAGE1)
              << QLatin1String(STR_IMAGE2)
              << QLatin1String(STR_IMAGE3)
              << QLatin1String(STR_IMAGE4)
              << QLatin1String(STR_IMAGE5);
    m_view->pushImages(imageList);

    m_pLabel = new QLabel(" ", this);
    m_pProgressBar = new QProgressBar(this);

    m_pLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    vLayout->addWidget(m_view);
    vLayout->addWidget(m_pLabel);
    vLayout->addWidget(m_pProgressBar);

    this->setWindowFlags(Qt::FramelessWindowHint);

    setLayout(vLayout);
    resize(256, 256);
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
    m_pLabel->setText(tr("Loading...%1%").arg(0));
}
/*!
 * 槽函数，报告插件加载完毕
 */
void PluginLoaderView::setFinished()
{
    m_pLabel->setText(tr("Load plugin Finished"));
}
/*!
 * 设置进度条显示范围
 * \param[in] min 进度条最小值
 * \param[in] max 进度条最大值
 */
void PluginLoaderView::setProgressRange(int min, int max)
{
    m_pProgressBar->setRange(min, max);
}

/*!
 * 设置当前进度，该值不能大于最大值，否则显示出错
 * \param[in] val
 */
void PluginLoaderView::setProgressValue(int val)
{
    float max = m_pProgressBar->maximum();
    if(!qFuzzyCompare(max, 0))
        m_pLabel->setText(tr("Loading...%1%").arg(int(val/max * 100)));
    m_pProgressBar->setValue(val);
}
/*!
 * 设置进度文字
 * \param[in] text 要显示的文字
 */
void PluginLoaderView::setProgressText(const QString &text)
{
    m_pLabel->setText(text);
}
