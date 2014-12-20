#pragma once
#ifndef FRAMEINFOWIDGET_H
#define FRAMEINFOWIDGET_H

#include <QWidget>
#include <QDataWidgetMapper>
#include <QTextEdit>
#include <QAbstractItemView>
#include "ui_frameinfowidget.h"
#include "easingcurvemodel.h"
#include "FrameStorage.h"
#include "splineeditor.h"


class AnimationInfo;
class FrameInfoWidget : public QWidget, public FSUtil
{
	Q_OBJECT

public:
	FrameInfoWidget(FrameStorageStruct* pFS, QWidget *parent = 0);
	~FrameInfoWidget();

signals:

	void onImageChanged();
	void previewAnimation(AnimationInfo* pInfo);

public slots:

	void onCurrentAnimationChanged(int iIndex);
	void onLoadImage();
	void onKillFocus();
	void onPreview();
	void onEasingCurveChanged(int iIndex);
	void onGenCode();

private:
	Ui::FrameInfoWidget ui;

	EasingCurveModel m_posModelX;
	EasingCurveModel m_posModelY;
	EasingCurveModel m_alphaModel;
	EasingCurveModel m_scaleModel;
	QDataWidgetMapper m_dataMapper;

	SplineEditor	m_splineEditor;
	QTextEdit		m_textEdit;
};

#endif // FRAMEINFOWIDGET_H
