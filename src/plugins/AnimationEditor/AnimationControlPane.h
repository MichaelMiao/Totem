#pragma once

#include <QComboBox>
#include <QWidget>
#include "frameeditwidget.h"
#include "FrameStorage.h"


class FrameInfoWidget;
class AnimationControlPane : public QWidget, public FSUtil
{
public:

	AnimationControlPane(FrameStorageStruct* pFS, QWidget* parent, FrameInfoWidget* pInfoWidget);
	~AnimationControlPane();

public slots:
	
	void onUpdate();

private:

	QComboBox		m_box;
	FrameEditWidget m_widgetFrameEdit;
};
