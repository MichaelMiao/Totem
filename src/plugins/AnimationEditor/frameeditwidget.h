#pragma once
#ifndef FRAMEEDITWIDGET_H
#define FRAMEEDITWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include "timelineeditor.h"


class FrameInfoWidget;
class FrameEditWidget : public QWidget
{
	Q_OBJECT

public:
	FrameEditWidget(QWidget *parent, FrameInfoWidget* pInfoWidget);
	~FrameEditWidget();

	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *) override;
	QSize sizeHint() const override;


	bool eventFilter(QObject * pObj, QEvent *pEvent);
	TimeLineEditor* getTimeLineEditor() { return &m_timeLineEditor; }

private:
	
	TimeLineEditor	m_timeLineEditor;
	QScrollArea*	m_pScrollArea;
};

#endif // FRAMEEDITWIDGET_H
