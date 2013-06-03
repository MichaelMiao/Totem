#ifndef ICORE_H
#define ICORE_H

#include "core_global.h"

#include <QMainWindow>
#include <QSettings>
#include <QObject>

#define LOGOUT(x) \
    {   \
        Core::ICore::messageManager()->printToOutputPane(x); \
        qDebug() << x; \
    }

#define LOGOUT_POPUP(x) \
    {   \
        Core::ICore::messageManager()->printToOutputPanePopup(x); \
        qDebug() << x; \
    }

QT_BEGIN_NAMESPACE
class QPrinter;
class QStatusBar;
template <class T> class QList;
QT_END_NAMESPACE

namespace Utils{
class XmlSerializableFactory;
}
namespace Core{
class ActionManager;
class Context;
class IContext;
class SettingsDatabase;
class EditorManager;
class MessageManager;
namespace Internal { class MainWindow; }

class CORE_EXPORT ICore : public QObject
{
    Q_OBJECT

    friend class Internal::MainWindow;
    explicit ICore(Internal::MainWindow *mw);
    ~ICore();
public:
    static ICore *instance();

    static QSettings *settings(QSettings::Scope scope = QSettings::UserScope);
    static SettingsDatabase *settingsDatabase();

    static QString resourcePath();
    static QString userResourcePath();


    static QMainWindow *mainWindow();
    static QStatusBar  *statusBar();

    static IContext *currentContextObject();
    static void updateAdditionalContexts(const Context &remove, const Context &add);
    static bool hasContext(int context);
    static void addContextObject(IContext *context);
    static void removeContextObject(IContext *context);

    static void addToolBar(QToolBar* toolBar);
    static void removeToolBar(QToolBar* toolBar);

    static ActionManager *actionManager();
    static EditorManager *editorManager();
    static MessageManager*messageManager();
	static Utils::XmlSerializableFactory *serializableFactory();
signals:
    void coreAboutToOpen();
    void coreOpened();
    void optionsDialogRequested();
    void coreAboutToClose();
    void contextAboutToChange(Core::IContext *context);
    void contextChanged(Core::IContext *context, const Core::Context &additionalContexts);
    void saveSettingsRequested();
public slots:
private:
    static ICore *m_instance;
    static Internal::MainWindow *m_mainwindow;
};

}

#endif // ICORE_H
