#ifndef TOOLLOADINGPROCESS_H
#define TOOLLOADINGPROCESS_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include "../designnet_core_global.h"
namespace DesignNet{
class DesignNetToolPlugin;
class DESIGNNET_CORE_EXPORT ToolLoadingProcess : public QThread
{
    Q_OBJECT
public:
    ToolLoadingProcess(QObject *parent);
    void setToolPluginsPath(QStringList &paths);//!< 设置插件路径

signals:
    void showMessage(const QString& strMsg);    //!< 显示消息
    void toolPluginLoaded(DesignNetToolPlugin *tool);//!< 当一个工具插件加载完毕后，发出此信号
public:
    virtual void run();
private:
    QStringList m_toolPaths;
};
}

#endif // TOOLLOADINGPROCESS_H
