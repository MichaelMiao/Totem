#pragma once
#ifndef TIMELINEEDIT_H
#define TIMELINEEDIT_H

#include <QGraphicsView>
#include "AnimantionMgr.h"
#include "addanimationwidget.h"
class FrameEditWidget;
class TimeLineEditor : public QWidget
{
	Q_OBJECT

public:
	TimeLineEditor(FrameEditWidget *parent);
	~TimeLineEditor();

	void	paintEvent(QPaintEvent *) override;
	QSize	sizeHint() const override;

	void	mouseMoveEvent(QMouseEvent * event) override;
	void	mousePressEvent(QMouseEvent * event) override;
	void	mouseReleaseEvent(QMouseEvent * event) override;
	void	contextMenuEvent(QContextMenuEvent * event) override;

	AnimationInfo* getCurrentAnimation();

signals:

	void	currentAnimationChanged(int);

public slots:

	void	onActionAddAnimation();
	void	onActionRemoveAnimation();

private:
	
	int		HitTimeLineTest(const int x);
	int		HitAnimationTest(QPoint pt);
	int		GetXPoxFromTime(const int iTime);

	int					m_iCurTimeLineIndex;
	int					m_iCurAnimation;
	QPoint				m_ptPressed;
	FrameEditWidget*	m_pFrame;
	AddAnimationWidget  m_widgetAddAni;
};

#endif // TIMELINEEDIT_H
