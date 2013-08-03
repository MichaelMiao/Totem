#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "icontext.h"
#include "icore.h"

#include <QMainWindow>
QT_BEGIN_NAMESPACE
class QSettings;
class QShortcut;
class QPrinter;
class QToolButton;
class QAction;
QT_END_NAMESPACE
namespace ExtensionSystem{
class ProgressManagerPrivate;
}
namespace Core{

class StatusBarWidget;
class MessageManager;
class ModeManager;
class RightPaneWidget;
class EditorManager;
class ActionManager;
class ModeTabWidget;
class ProgressView;

namespace Internal{

class OutputPaneManager;
class StatusBarManager;
class ShortcutSettings;

class ModeTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool initialize(QString *errorMessage);
    void extensionsInitialized();
    void aboutToShutdown();


    IContext *contextObject(QWidget *widget);
    void addContextObject(IContext *contex);
    void removeContextObject(IContext *contex);
    void resetContext();

    QSettings *settings(QSettings::Scope scope) const;
    inline SettingsDatabase *settingsDatabase() const { return m_settingsDatabase; }
    IContext * currentContextObject() const;

    void updateAdditionalContexts(const Context &remove, const Context &add);
    bool hasContext(int context) const;

    bool isPresentationModeEnabled();
    void setPresentationModeEnabled(bool);
    void setOverrideColor(const QColor &color);

    void addModeTabToolBar(QToolBar *toolbar);
    void removeModeTabToolBar(QToolBar *toolbar);

    Core::ActionManager *actionManager() const;
    Core::EditorManager *editorManager() const;
    Core::MessageManager*messageManager() const;
    Core::ModeTabWidget* modeTabWidget() const;
    QStatusBar *statusBar() const;
    ExtensionSystem::ProgressManagerPrivate *progressManager() const;
signals:
    void windowActivated();

private slots:
    void aboutTotem();
    void aboutPlugins();

    void updateFocusWidget(QWidget *old, QWidget *now);
    bool showOptionsDialog(const QString &category = QString(),
                           const QString &page = QString(),
                           QWidget *parent = 0);
    void newFile();
    void openFile();
protected:
    virtual void closeEvent(QCloseEvent *event);
    void openFiles(const QStringList &fileNames);
    virtual void changeEvent(QEvent *e);
private:
    void updateContextObject(IContext *context);
    void updateContext();

    void registerDefaultContainers();
    void registerDefaultActions();


    void readSettings();
    void writeSettings();

    ICore *m_coreImpl;
    Context m_additionalContexts;
    QSettings *m_settings;
    QSettings *m_globalSettings;
    SettingsDatabase *m_settingsDatabase;
    ActionManager *m_actionManager;
    EditorManager *m_editorManager;
    StatusBarManager *m_statusBarManager;
    MessageManager *m_messageManager;
    ExtensionSystem::ProgressManagerPrivate *m_progressManager;
    RightPaneWidget *m_rightPaneWidget;
    ModeTabWidget *m_modeStack;
    ModeManager *m_modeManager;
    Core::StatusBarWidget *m_outputView;
	ProgressView*		   m_progressView;


    IContext *m_activeContext;
    QToolButton *m_toggleSideBarButton;
    QColor m_overrideColor;
    QMap<QWidget *, IContext *> m_contextWidgets;
    ShortcutSettings *m_shortcutSettings;
    QAction *m_optionsAction;
};
}
}
#endif // MAINWINDOW_H
