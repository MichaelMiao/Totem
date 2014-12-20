#ifndef ANIMATIONDELEGATE_H
#define ANIMATIONDELEGATE_H

#include <QItemDelegate>

enum AnimationEditWidget
{
	AniEditWidget_Name,
	AniEditWidget_TimeFrom,
	AniEditWidget_Duration,
	AniEditWidget_PosFromX,
	AniEditWidget_PosToX,
	AniEditWidget_CurvePosX,
	AniEditWidget_PosFromY,
	AniEditWidget_PosToY,
	AniEditWidget_CurvePosY,
	AniEditWidget_AlphaFrom,
	AniEditWidget_AlphaTo,
	AniEditWidget_CurveAlpha,
	AniEditWidget_ScaleFrom,
	AniEditWidget_ScaleTo,
	AniEditWidget_CurveScale,
	AniEditWidget_End,
};

class AnimationDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	AnimationDelegate(QObject *parent);
	~AnimationDelegate();

	// override
	void	setEditorData(QWidget * editor, const QModelIndex & index) const;
	void	setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;

private:
	

};

#endif // ANIMATIONDELEGATE_H
