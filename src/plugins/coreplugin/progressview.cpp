#include "progressview.h"
#include "utils/totemassert.h"
#include <QHBoxLayout>

namespace Core{

ProgressView::ProgressView(QWidget *parent)
    :QWidget(parent)
{
    m_layout = new QHBoxLayout;
    setLayout(m_layout);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    setWindowTitle(tr("Processes"));
}


FutureProgress *ProgressView::addTask(const QFuture<void> &future, const QString &title,
                                      const QString &type,
                                      ProgressManager::ProgressFlags flags)
{
    removeOldTasks(type);
    if (m_taskList.size() == 3)
        removeOneOldTask();
    FutureProgress *progress = new FutureProgress(this);
    progress->setTitle(title);
    progress->setFuture(future);

    m_layout->insertWidget(0, progress);
    m_taskList.append(progress);
    progress->setType(type);
    if (flags.testFlag(ProgressManager::KeepOnFinish))
    {
        progress->setKeepOnFinish(FutureProgress::KeepOnFinishTillUserInteraction);
    }
    else
    {
        progress->setKeepOnFinish(FutureProgress::HideOnFinish);
    }
    connect(progress, SIGNAL(removeMe()), this, SLOT(slotRemoveTask()));
    return progress;
}

void ProgressView::slotRemoveTask()
{
    FutureProgress *progress = qobject_cast<FutureProgress *>(sender());
    TOTEM_ASSERT(progress, return);
    QString type = progress->type();
    removeTask(progress);
    removeOldTasks(type, true);
}

void ProgressView::removeOldTasks(const QString &type, bool keepOne)
{
    //当希望保留一个时，firstFound为false就可以了。
    bool firstFound = !keepOne;
    QList<FutureProgress *>::iterator i = m_taskList.end();
    while (i != m_taskList.begin())
    {
        --i;
        if ((*i)->type() == type)
        {
            if (firstFound && ((*i)->future().isFinished() || (*i)->future().isCanceled()))
            {
                deleteTask(*i);
                i = m_taskList.erase(i);
            }
            firstFound = true;
        }
    }
}

void ProgressView::removeOneOldTask()
{
    for (QList<FutureProgress *>::iterator i = m_taskList.begin();
         i != m_taskList.end(); ++i)
    {
        if ((*i)->future().isFinished())
        {
            deleteTask(*i);//从view中删除就可以了
            i = m_taskList.erase(i);
            return;
        }
    }
    for (QList<FutureProgress *>::iterator i = m_taskList.begin();
         i != m_taskList.end(); ++i)
    {
        QString type = (*i)->type();

        int taskCount = 0;
        foreach (FutureProgress *p, m_taskList)
            if (p->type() == type)
                ++taskCount;

        if (taskCount > 1 && (*i)->future().isFinished())
        {
            deleteTask(*i);
            i = m_taskList.erase(i);
            return;
        }
    }

    FutureProgress *task = m_taskList.takeFirst();
    deleteTask(task);
}

void ProgressView::removeTask(FutureProgress *task)
{
    m_taskList.removeAll(task);
    deleteTask(task);
}

void ProgressView::deleteTask(FutureProgress *task)
{
    layout()->removeWidget(task);
    task->hide();
    task->deleteLater();
}
}
