#include "frameeditwidget.h"
#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QScrollArea>
#include "frameinfowidget.h"


#define TOPPNE_HEIGHT		40
FrameEditWidget::FrameEditWidget(QWidget *parent, FrameInfoWidget* pInfoWidget)
	: QWidget(parent), m_timeLineEditor(this)
{
	m_pScrollArea = new QScrollArea(this);
	m_pScrollArea->setWidget(&m_timeLineEditor);
	m_pScrollArea->installEventFilter(this);

	connect(&m_timeLineEditor, SIGNAL(currentAnimationChanged(int)), pInfoWidget, SLOT(onCurrentAnimationChanged(int)));
}

FrameEditWidget::~FrameEditWidget()
{

}

void FrameEditWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QRect rc(rect());
	rc.setBottom(rc.top() + TOPPNE_HEIGHT);
	p.fillRect(rc, Qt::black);
}

QSize FrameEditWidget::sizeHint() const
{
	QSize sz = m_timeLineEditor.sizeHint();
	sz.setHeight(sz.height() + TOPPNE_HEIGHT);
	return sz;
}

void FrameEditWidget::resizeEvent(QResizeEvent *)
{
	QRect rc(rect());
	QRect rcScrollArea(rc);
	rcScrollArea.setTop(rc.top() + TOPPNE_HEIGHT);
	m_pScrollArea->setGeometry(rcScrollArea);
}

bool FrameEditWidget::eventFilter(QObject * pObj, QEvent *pEvent)
{
	if (pObj == m_pScrollArea && pEvent->type() == QEvent::Resize)
	{
		QSize sz = m_timeLineEditor.sizeHint();
		m_pScrollArea->updateGeometry();
		QSize szMax = m_pScrollArea->viewport()->size();
		sz.setHeight(qMax(szMax.height() - 1, sz.height()));
		m_timeLineEditor.resize(sz);
	}
	return false;
}
