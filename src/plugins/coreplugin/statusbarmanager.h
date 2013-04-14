#ifndef STATUSBARMANAGER_H
#define STATUSBARMANAGER_H

#include <QObject>
#include <QList>

QT_FORWARD_DECLARE_CLASS(QWidget)

namespace Core {
namespace Internal {

class MainWindow;

class StatusBarManager : public QObject
{
    Q_OBJECT

public:
    StatusBarManager(MainWindow *mainWnd);
    ~StatusBarManager();

    void init();
    void extensionsInitalized();

private slots:
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);

private:
    MainWindow *m_mainWnd;
    QList<QWidget *> m_statusBarWidgets;
};

} // namespace Internal
} // namespace Core
#endif // STATUSBARMANAGER_H
