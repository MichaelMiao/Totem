#ifndef DESIGNNETFORMMANAGER_H
#define DESIGNNETFORMMANAGER_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>
#include "designnet_core_global.h"
#include "coreplugin/icontext.h"
#include "coreplugin/id.h"
#include <QFuture>
QT_BEGIN_NAMESPACE
class QToolBar;
QT_END_NAMESPACE

namespace Core{
class EditorToolBar;
class IMode;
class IEditor;
class Command;
class ActionContainer;
}

namespace DesignNet{
class DesignNetEditor;
class DesignNetMainWindow;
class DesignNetMode;
class DesignNetContext;
class DesignNetSpace;
class DesignNetFormManager : public QObject
{
    Q_OBJECT
    friend class DesignNetMainWindow;
public:
    explicit DesignNetFormManager(QObject *parent = 0);
    ~DesignNetFormManager();
    static DesignNetFormManager *instance();
    bool startInit();//开始初始化

    DesignNetEditor *createEditor(QWidget *parent);
    void writeSettings();
    void readSettings();
    /*!
     * \brief addToolAction 添加工具条
     * \param a 相关联的Action
     * \param context Action所在的Context
     * \param id
     * \param c1
     * \param keySequence 快捷键
     * \return
     */
    Core::Command *addToolAction(QAction *a,
                                 const Core::Context &context, const Core::Id &id,
                                 Core::ActionContainer *ac, const QString &keySequence = QString());
    QToolBar *createEditorToolBar() const;
    void createControlToolBar();

    void runSpace(QFutureInterface<bool> &fi, DesignNetSpace *space);
signals:
	void logout(QString str);
public slots:
    void onModeChanged(Core::IMode *mode);
    void onCoreAboutToOpen();
    void currentEditorChanged(Core::IEditor *editor);
    void onProcessAction(bool checked);
    void updateAction();
private:
    static DesignNetFormManager *m_instance;
    DesignNetMainWindow *   m_mainWindow;
    QWidget*                m_editorToolBar;
    Core::EditorToolBar *   m_toolBar;          //!< 工具条
    QWidget             *   m_modeWidget;
    DesignNetContext    *   m_context;
    DesignNetMode       *   m_designMode;
    Core::Context           m_contexts;
    QToolBar*               m_controlToolBar;   //!< 处理工具栏
    QList<Core::Id>         m_toolActionIds;
    QFutureWatcher<bool>*   m_processWatcher;   //!< 处理进程监控
    QAction*                m_processAction;    //!< 处理Action
};
}

#endif // DESIGNNETFORMMANAGER_H
