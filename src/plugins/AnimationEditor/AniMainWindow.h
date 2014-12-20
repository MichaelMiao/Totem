#pragma once
#include "../../libs/CustomUI/basemainwindow.h"
#include "CustomUI/dockwindow.h"
#include "frameinfowidget.h"
#include "FrameStorage.h"


class AniPreviewWnd;
class AnimationControlPane;
namespace Core
{
	class MiniSplitter;
}
class AniMainWindow : public CustomUI::BaseMainWindow
{
public:

	AniMainWindow(QWidget *parent);
	~AniMainWindow(){}

	FrameStorageStruct* getFrameStorage() { return &m_fs; }

private:

	AniPreviewWnd*			m_pPreviewWnd;
	AnimationControlPane*	m_pControlPane;
	Core::MiniSplitter *	m_splitter;
	FrameInfoWidget*		m_frameInfoWidget;
	FrameStorageStruct		m_fs;
	CustomUI::DockWindow*	m_dockWndFrameInfo;
};
