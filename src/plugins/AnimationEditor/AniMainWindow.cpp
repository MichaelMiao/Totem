#include "stdafx.h"
#include "AniMainWindow.h"
#include <QShortcut>
#include "../coreplugin/actionmanager/actionmanager.h"
#include "../coreplugin/actionmanager/command.h"
#include "../coreplugin/coreconstants.h"
#include "../coreplugin/minisplitter.h"
#include "AnimationControlPane.h"
#include "AniPreviewWnd.h"
#include "FrameStorage.h"


using namespace Core;
AniMainWindow::AniMainWindow(QWidget *parent)
	: CustomUI::BaseMainWindow(parent), m_pPreviewWnd(0)
{
	setProperty(FRAME_STORAGE_STRUCT_PROPERTY, QVariant::fromValue<void*>((void*)&m_fs));
	m_frameInfoWidget = new FrameInfoWidget(&m_fs);
	m_dockWndFrameInfo = addDockForWidget(m_frameInfoWidget, Qt::RightDockWidgetArea);
	// Ô¤ÀÀ´°¿Ú
	m_pPreviewWnd = new AniPreviewWnd(&m_fs);
	MiniSplitter *rightPaneSplitter = new MiniSplitter;
	rightPaneSplitter->insertWidget(0, m_pPreviewWnd);
	rightPaneSplitter->setStretchFactor(0, 1);

	// ¿ØÖÆÃæ°å
	m_pControlPane = new AnimationControlPane(&m_fs, this, m_frameInfoWidget);
	MiniSplitter *bottomPaneSplitter = new MiniSplitter;
	bottomPaneSplitter->setOrientation(Qt::Horizontal);
	bottomPaneSplitter->insertWidget(0, m_pControlPane);
	
	MiniSplitter *m_splitter = new MiniSplitter;
	m_splitter->setOrientation(Qt::Vertical);
	m_splitter->insertWidget(0, rightPaneSplitter);
	m_splitter->insertWidget(1, bottomPaneSplitter);
	m_splitter->setStretchFactor(0, 3);
	m_splitter->setStretchFactor(1, 1);

	
	setCentralWidget(m_splitter);

	m_fs.m_pMainWnd = this;

	// ¿ì½Ý¼ü
	{
		QShortcut *shortcut = new QShortcut(this);
		shortcut->setWhatsThis("Preview");
		Core::Command *cmd = Core::ActionManager::registerShortcut(shortcut, "ShotcutPreview", Context(Core::Constants::C_GLOBAL));
		cmd->setDefaultKeySequence(QKeySequence(QString::fromLatin1("F5")));
		connect(shortcut, SIGNAL(activated()), m_frameInfoWidget, SLOT(onPreview()));
	}
	{
		QShortcut *shortcut = new QShortcut(this);
		shortcut->setWhatsThis("GenCode");
		Core::Command *cmd = Core::ActionManager::registerShortcut(shortcut, "GenCode", Context(Core::Constants::C_GLOBAL));
		cmd->setDefaultKeySequence(QKeySequence(QString::fromLatin1("Ctrl+F5")));
		connect(shortcut, SIGNAL(activated()), m_frameInfoWidget, SLOT(onGenCode()));
	}
}
