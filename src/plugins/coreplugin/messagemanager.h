#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "core_global.h"
#include <QObject>

namespace Core {

namespace Internal {
class MessageOutputWindow;
}

class CORE_EXPORT MessageManager : public QObject
{
    Q_OBJECT

public:
    MessageManager();
    ~MessageManager();

    void init();

    static MessageManager *instance() { return m_instance; }

    void showOutputPane();

public slots:
    void printToOutputPane(const QString &text, bool bringToForeground);
    void printToOutputPanePopup(const QString &text); // pops up
    void printToOutputPane(const QString &text);

private:
    Internal::MessageOutputWindow *m_messageOutputWindow;

    static MessageManager *m_instance;
};

} // namespace Core


#endif // MESSAGEMANAGER_H
