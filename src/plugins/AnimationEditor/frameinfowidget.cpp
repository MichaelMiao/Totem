#include "stdafx.h"
#include "frameinfowidget.h"
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QSignalMapper>
#include "AnimantionMgr.h"
#include "AnimationControlPane.h"
#include "animationdelegate.h"
#include "AniPreviewWnd.h"
#include "FrameStorage.h"


FrameInfoWidget::FrameInfoWidget(FrameStorageStruct* pFS, QWidget *parent)
	: FSUtil(pFS), QWidget(parent), m_posModelX(this), m_posModelY(this), m_alphaModel(this), m_scaleModel(this), m_dataMapper(this)
{
	FS()->m_pFrameInfo = this;
	ui.setupUi(this);
	connect(ui.btnImage, SIGNAL(clicked()), this, SLOT(onLoadImage()));
	ui.btnImage->setPixmap(QPixmap(":/AnimationEditor/Resources/obj.png"));

	m_dataMapper.setModel(AnimantionMgr::instance());
	setDisabled(true);
	struct
	{
		AnimationEditWidget eType;
		QWidget* pWidget;
	} widget[] = {
		{ AniEditWidget_Name, ui.name },
		{ AniEditWidget_TimeFrom, ui.spinBoxTimeFrom },
		{ AniEditWidget_Duration, ui.spinBoxTimeDuration },
		{ AniEditWidget_PosFromX, ui.editFromX },
		{ AniEditWidget_PosToX, ui.editToX },
		{ AniEditWidget_CurvePosX, ui.comboBoxPosX },
		{ AniEditWidget_PosFromY, ui.editFromY },
		{ AniEditWidget_PosToY, ui.editToY },
		{ AniEditWidget_CurvePosY, ui.comboBoxPosY },

		{ AniEditWidget_AlphaFrom, ui.editAlphaFrom },
		{ AniEditWidget_AlphaTo, ui.editAlphaTo },
		{ AniEditWidget_CurveAlpha, ui.comboBoxAlpha },
		{ AniEditWidget_ScaleFrom, ui.spinBoxScaleFrom },
		{ AniEditWidget_ScaleTo, ui.spinBoxScaleTo },
		{ AniEditWidget_CurveScale, ui.comboBoxScale },
	};
	for (int i = 0; i < _countof(widget); i++)
	{
		m_dataMapper.addMapping(widget[i].pWidget, (int)widget[i].eType);
	}

	m_dataMapper.setItemDelegate(new AnimationDelegate(&m_dataMapper));
	m_dataMapper.setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	ui.comboBoxPosX->setModel(&m_posModelX);
	ui.comboBoxPosY->setModel(&m_posModelY);
	ui.comboBoxAlpha->setModel(&m_alphaModel);
	ui.comboBoxScale->setModel(&m_scaleModel);
	ui.comboBoxPosX->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
	ui.comboBoxPosY->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
	ui.comboBoxAlpha->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
	ui.comboBoxScale->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
	QSignalMapper* pMapper = new QSignalMapper(this);
	connect(ui.btnPreview, SIGNAL(clicked()), this, SLOT(onPreview()));
	connect(ui.btnGen, SIGNAL(clicked()), this, SLOT(onGenCode()));

	connect(ui.comboBoxAlpha, SIGNAL(currentIndexChanged(int)), this, SLOT(onEasingCurveChanged(int)));
	connect(ui.comboBoxScale, SIGNAL(currentIndexChanged(int)), this, SLOT(onEasingCurveChanged(int)));
	connect(ui.comboBoxPosX, SIGNAL(currentIndexChanged(int)), this, SLOT(onEasingCurveChanged(int)));
	connect(ui.comboBoxPosY, SIGNAL(currentIndexChanged(int)), this, SLOT(onEasingCurveChanged(int)));
}

FrameInfoWidget::~FrameInfoWidget()
{

}

void FrameInfoWidget::onCurrentAnimationChanged(int iIndex)
{
	if (iIndex == -1)
	{
		setDisabled(true);
		return;
	}
	else
	{
		m_dataMapper.setCurrentIndex(iIndex);
		AniPreviewWnd* pWnd = FS()->m_pPreviewWnd;
		connect(this, SIGNAL(previewAnimation(AnimationInfo*)), pWnd, SLOT(onActionChanged(AnimationInfo*)));
		setDisabled(false);
	}
}

void FrameInfoWidget::onLoadImage()
{
	QString strFilePath = QFileDialog::getOpenFileName(0, "Load Image", "", "Images (*.png *.xpm *.jpg)");
	if (strFilePath.length() > 0)
		ui.btnImage->setPixmap(QPixmap(strFilePath));
}

void FrameInfoWidget::onKillFocus()
{

}

void FrameInfoWidget::onPreview()
{
	m_dataMapper.submit();
	int iIndex = m_dataMapper.currentIndex();
	if (iIndex >= 0)
		emit previewAnimation(AnimantionMgr::instance()->getAnimationByIndex(iIndex));
}

void FrameInfoWidget::onEasingCurveChanged(int iIndex)
{
	if (iIndex == QEasingCurve::Custom)
	{
		QComboBox* pCombo = qobject_cast<QComboBox*>(sender());
		if (pCombo)
		{
			EasingCurveModel* pModel = (EasingCurveModel*)pCombo->model();
			SplineEditor* pEditor = pModel->GetCustomEditor();
			if (pEditor && !pEditor->isVisible())
				pEditor->show();
		}
	}
}

QString GenEasingCurveCode(QEasingCurve easingCurve, QString strLabel)
{
	QString str = strLabel + "\r\n";
	for (int i = 0; i <= 1000; ++i)
	{
		str += QString("%1f, ").arg(easingCurve.valueForProgress((i * 1.0 / 1000)));
		if (i % 10 == 0 && i != 0)
		{
			str += "\r\n";
		}
	}
	str += "\r\n";
	return str;
}

void FrameInfoWidget::onGenCode()
{
	m_dataMapper.submit();
	AnimationInfo* pInfo = AnimantionMgr::instance()->getAnimationByIndex(m_dataMapper.currentIndex());
	if (pInfo)
	{
		m_textEdit.clear();
		m_textEdit.append(GenEasingCurveCode(pInfo->m_curvePosX, "[pos_x:]"));
		m_textEdit.append(GenEasingCurveCode(pInfo->m_curvePosY, "[pos_y:]"));
		m_textEdit.append(GenEasingCurveCode(pInfo->m_curveAlpha, "[alpha:]"));
		m_textEdit.append(GenEasingCurveCode(pInfo->m_curveScale, "[scale:]"));
		m_textEdit.show();
		m_textEdit.raise();
	}
}
