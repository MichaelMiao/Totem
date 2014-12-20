#include "stdafx.h"
#include "AnimationControlPane.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include "timelineeditor.h"


AnimationControlPane::AnimationControlPane(FrameStorageStruct* pFS, QWidget* parent, FrameInfoWidget* pInfoWidget)
	: FSUtil(pFS), QWidget(parent), m_widgetFrameEdit(this, pInfoWidget)
{
	FS()->m_pCtrlPane = this;
	QHBoxLayout* pLayout = new QHBoxLayout;
	m_widgetFrameEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	pLayout->addWidget(&m_box);
	pLayout->addWidget(&m_widgetFrameEdit);
	setLayout(pLayout);

	connect(AnimantionMgr::instance(), SIGNAL(onAnimationChanged()), m_widgetFrameEdit.getTimeLineEditor(), SLOT(update()));
}

AnimationControlPane::~AnimationControlPane()
{

}

void AnimationControlPane::onUpdate()
{
	m_widgetFrameEdit.getTimeLineEditor()->update();
}
