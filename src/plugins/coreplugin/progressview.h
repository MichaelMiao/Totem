#ifndef PROGRESSVIEW_H
#define PROGRESSVIEW_H

#include "extensionsystem/futureprogress.h"
#include "extensionsystem/progressmanager.h"
#include <QWidget>


QT_BEGIN_NAMESPACE
class QHBoxLayout;
QT_END_NAMESPACE

using namespace ExtensionSystem;
namespace Core {
class ProgressView : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressView(QWidget *parent = 0);
    FutureProgress *addTask(const QFuture<void> &future,
                            const QString &title,
                            const QString &type,
                            ExtensionSystem::ProgressManager::ProgressFlags flags);
private slots:
    void slotRemoveTask();

private:
    void removeOldTasks(const QString &type, bool keepOne = false);
    void removeOneOldTask();
    void removeTask(FutureProgress *task);
    void deleteTask(FutureProgress *task);

private:
    QHBoxLayout *m_layout;
    QList<FutureProgress *> m_taskList;
};
}

#endif // PROGRESSVIEW_H
