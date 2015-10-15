#include "stdafx.h"
#include "designnetmainwindow.h"
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include "../../coreplugin/actionmanager/actioncontainer.h"
#include "../../coreplugin/actionmanager/actionmanager.h"
#include "../../coreplugin/constants.h"
#include "../../coreplugin/document/editormanager.h"
#include "../../coreplugin/icore.h"
#include "../../coreplugin/mainwindow/rebarmanager.h"
#include "designnet_core_def.h"
#include "DesignNetEditor.h"


DesignNetMainWindow::DesignNetMainWindow(QWidget *parent)
	: CustomUI::BaseMainWindow(parent), m_context(DesignNet::Constants::C_DESIGNNET)
{
	m_pStack = new QStackedWidget(this);
	QAction* pAction = new QAction(tr("Design"), this);
	Core::Command* pCmd = Core::ActionManager::registerAction(pAction, DesignNet::Constants::ID_EDITMODE, m_context);
	if (pCmd)
		RebarManager::instance()->addRebarCommand(DesignNet::Constants::C_DESIGNNET, pCmd);
	m_pTabWidget = new QTabWidget(m_pStack);
	ICore::instance()->insertCenterWidget(this);
}

void DesignNetMainWindow::initialize()
{
	m_pStack->addWidget(Core::EditorManager::instance());
	m_pStack->addWidget(m_pTabWidget);
	setCentralWidget(m_pStack);
}

void DesignNetMainWindow::addEditor(DesignNetEditor* pEditor)
{
	if (pEditor)
	{
		connect(pEditor, SIGNAL(changed()), this, SLOT(titleChanged()));
		connect(pEditor->document(), SIGNAL(titleChanged(QString)), this, SLOT(titleChanged()));
		m_pTabWidget->addTab(pEditor->widget(), pEditor->displayName());
	}
}
