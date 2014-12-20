#ifndef PLUGINLOADERVIEW_H
#define PLUGINLOADERVIEW_H

#include "extensionsystem/futureprogress.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QProgressBar;
class QLabel;
QT_END_NAMESPACE

namespace GraphicsUI{
class ImageListAutoView;
}
/*!
 * \brief 插件加载进度Widget
 *
 *  使用系统线程池来报告进度
 */
class PluginLoaderView : public ExtensionSystem::FutureProgress
{
    Q_OBJECT
public:
    explicit PluginLoaderView(QWidget *parent = 0);
    void init();

protected slots:
    void setStarted();
    void setFinished();

private:
    GraphicsUI::ImageListAutoView *m_view;//!< QGraphicsView类型，有淡入淡出效果
};

#endif // PLUGINLOADERVIEW_H
