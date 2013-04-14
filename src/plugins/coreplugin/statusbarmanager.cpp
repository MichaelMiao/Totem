#include "statusbarmanager.h"

#include "coreconstants.h"
#include "mainwindow.h"
#include "statusbarwidget.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <aggregation/aggregate.h>
#include <extensionsystem/pluginmanager.h>

#include <QSettings>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>

using namespace Core;
using namespace Core::Internal;

StatusBarManager::StatusBarManager(MainWindow *mainWnd)
  : QObject(mainWnd),
    m_mainWnd(mainWnd)
{
    for (int i = 0; i <= StatusBarWidget::Last; ++i) {
        QWidget *w = new QWidget();
        m_mainWnd->statusBar()->insertPermanentWidget(i, w);
        w->setLayout(new QHBoxLayout);
        w->setVisible(true);
        w->layout()->setMargin(0);
        m_statusBarWidgets.append(w);
    }
    m_mainWnd->statusBar()->insertPermanentWidget(StatusBarWidget::Last+1,
                                                  new QLabel(), 1);
}

StatusBarManager::~StatusBarManager()
{
}

void StatusBarManager::init()
{
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(aboutToRemoveObject(QObject*)),
            this, SLOT(aboutToRemoveObject(QObject*)));
}

void StatusBarManager::objectAdded(QObject *obj)
{
    StatusBarWidget *view = Aggregation::query<StatusBarWidget>(obj);
    if (!view)
        return;

    QWidget *viewWidget = 0;
    viewWidget = view->widget();
    m_statusBarWidgets.at(view->position())->layout()->addWidget(viewWidget);

    m_mainWnd->addContextObject(view);
}

void StatusBarManager::aboutToRemoveObject(QObject *obj)
{
    StatusBarWidget *view = Aggregation::query<StatusBarWidget>(obj);
    if (!view)
        return;
    m_mainWnd->removeContextObject(view);
}

void StatusBarManager::extensionsInitalized()
{
}
