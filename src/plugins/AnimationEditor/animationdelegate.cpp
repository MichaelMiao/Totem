#include "stdafx.h"
#include "animationdelegate.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QEasingCurve>

AnimationDelegate::AnimationDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

AnimationDelegate::~AnimationDelegate()
{

}

void AnimationDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	switch (index.column())
	{
	case AniEditWidget_Name:
	case AniEditWidget_PosFromX:
	case AniEditWidget_PosFromY:
	case AniEditWidget_PosToX:
	case AniEditWidget_PosToY:
	case AniEditWidget_AlphaFrom:
	case AniEditWidget_AlphaTo:
		((QLineEdit*)editor)->setText(index.data().toString());
		return;
	case AniEditWidget_TimeFrom:
	case AniEditWidget_Duration:
	case AniEditWidget_ScaleFrom:
	case AniEditWidget_ScaleTo:
		((QSpinBox*)editor)->setValue(index.data().toInt());
		return;
	case AniEditWidget_CurvePosX:
	case AniEditWidget_CurvePosY:
	case AniEditWidget_CurveScale:
	case AniEditWidget_CurveAlpha:
		((QComboBox*)editor)->setCurrentIndex(index.data().toInt());
		return;
	default:
		break;
	}
}

void AnimationDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	switch (index.column())
	{
	case AniEditWidget_Name:
	case AniEditWidget_PosFromX:
	case AniEditWidget_PosFromY:
	case AniEditWidget_PosToX:
	case AniEditWidget_PosToY:
	case AniEditWidget_AlphaFrom:
	case AniEditWidget_AlphaTo:
		model->setData(index, ((QLineEdit*)editor)->text());
		return;
	case AniEditWidget_TimeFrom:
	case AniEditWidget_Duration:
	case AniEditWidget_ScaleFrom:
	case AniEditWidget_ScaleTo:
		model->setData(index, ((QSpinBox*)editor)->value());
		return;
	case AniEditWidget_CurvePosX:
	case AniEditWidget_CurvePosY:
	case AniEditWidget_CurveScale:
	case AniEditWidget_CurveAlpha:
	{
		QAbstractItemModel* pModel =((QComboBox*)editor)->model();
		QModelIndex modelIndex = pModel->index(((QComboBox*)editor)->currentIndex(), 0);
		QEasingCurve ea = ((QComboBox*)editor)->model()->data(modelIndex, Qt::UserRole).toEasingCurve();
		QEasingCurve::Type t = ea.type();
		model->setData(index, ea);
	}
		return;
	default:
		break;
	}
}
