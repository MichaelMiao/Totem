#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include "extensionsystem_global.h"

#include <QObject>
#include <QFuture>
#include "futureprogress.h"
namespace ExtensionSystem {

class EXTENSIONSYSTEM_EXPORT ProgressManager : public QObject
{
    Q_OBJECT
public:
    enum ProgressFlag {
        KeepOnFinish = 0x01,
        ShowInApplicationIcon = 0x02
    };
    Q_DECLARE_FLAGS(ProgressFlags, ProgressFlag)

    explicit ProgressManager(QObject *parent = 0): QObject(parent){}
    virtual ~ProgressManager() {}
    //添加任务
    virtual void addTask(QFuture<void> future,
                             const QString &type,
                             const QString &title,
                             FutureProgress* widget) = 0;

signals:
    void taskStarted(const QString &type);
    void allTasksFinished(const QString &type);
public slots:
    virtual void cancelTasks(const QString &type) = 0;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(ExtensionSystem::ProgressManager::ProgressFlags)


#endif // PROGRESSMANAGER_H