#include "progressmanagerprivate.h"
using namespace ExtensionSystem;
ProgressManagerPrivate *ProgressManagerPrivate::m_instance = 0;
ProgressManagerPrivate::ProgressManagerPrivate(QObject *parent) :
    ProgressManager(parent)
{
}

ProgressManagerPrivate *ProgressManagerPrivate::instance()
{
    if(!m_instance)
        m_instance = new ProgressManagerPrivate();
    return m_instance;
}

void ProgressManagerPrivate::destory()
{
    if(m_instance)
    {
        delete m_instance;
        m_instance = 0;
    }
}

void ProgressManagerPrivate::addTask(QFuture<void> future,
                                     const QString &type,
                                     const QString &title,
                                     FutureProgress *widget)
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    m_runningTasks.insert(watcher, type);
    connect(watcher, SIGNAL(finished()), this, SLOT(taskFinished()));
    watcher->setFuture(future);
    widget->setTitle(title);
    widget->setType(type);
    widget->setFuture(future);
}

void ProgressManagerPrivate::cancelTasks(const QString &type)
{
    bool found = false;
    QMap<QFutureWatcher<void> *, QString>::iterator task = m_runningTasks.begin();
    while (task != m_runningTasks.end())
    {
        if (task.value() != type)
        {
            ++task;
            continue;
        }
        found = true;
        disconnect(task.key(), SIGNAL(finished()), this, SLOT(taskFinished()));

        task.key()->cancel();
        delete task.key();
        task = m_runningTasks.erase(task);
    }
    if (found)
    {
        emit allTasksFinished(type);
    }
}

void ProgressManagerPrivate::taskFinished()
{
    QObject *taskObject = sender();
    QFutureWatcher<void> *task = static_cast<QFutureWatcher<void> *>(taskObject);

    QString type = m_runningTasks.value(task);
    m_runningTasks.remove(task);
    delete task;

    if (!m_runningTasks.key(type, 0))
    {
        emit allTasksFinished(type);
    }
}

void ProgressManagerPrivate::cancelAllRunningTasks()
{
    typedef QMap<QFutureWatcher<void> *, QString>::const_iterator FWIter;
    FWIter task = m_runningTasks.constBegin();
    while (task != m_runningTasks.constEnd())
    {
        disconnect(task.key(), SIGNAL(finished()), this, SLOT(taskFinished()));
        task.key()->cancel();
        delete task.key();
        ++task;
    }
    m_runningTasks.clear();
}