#ifndef PROGRESSMANAGERPRIVATE_H
#define PROGRESSMANAGERPRIVATE_H

#include "progressmanager.h"

#include <QFuture>
#include <QFutureWatcher>
#include "futureprogress.h"
namespace ExtensionSystem{
class EXTENSIONSYSTEM_EXPORT ProgressManagerPrivate : public ProgressManager
{
    Q_OBJECT
public:

    static ProgressManagerPrivate* instance();
    void destory();
    virtual void addTask(QFuture<void> future,
                             const QString &type,
                             const QString &title,
                             FutureProgress* widget);
signals:

public slots:
    virtual void cancelTasks(const QString &type);

private slots:
    void taskFinished();
    void cancelAllRunningTasks();
private:
    explicit ProgressManagerPrivate(QObject *parent = 0);

    QMap<QFutureWatcher<void> *, QString> m_runningTasks;
    static ProgressManagerPrivate* m_instance;
};
}

#endif // PROGRESSMANAGERPRIVATE_H