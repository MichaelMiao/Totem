#include "timelineeditor.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include "addanimationwidget.h"
#include "AnimantionMgr.h"
#include "frameeditwidget.h"

#define EDITOR_LEFT_PADDING	10
#define FRAME_LINE_WIDTH	2
#define FRAME_LINE_MARGIN	20
#define FRAME_LINE_COUNT	500
#define ANIMATION_LINE_SPACE	20
#define ANIMATION_LINE_HIEGHT	8
#define TIME_SCALE			10

TimeLineEditor::TimeLineEditor(FrameEditWidget *parent)
	: QWidget(parent), m_ptPressed(-1, -1), m_pFrame(parent), m_iCurTimeLineIndex(-1), m_iCurAnimation(-1)
{
	connect(AnimantionMgr::instance(), SIGNAL(animationRemoved(AnimationInfo*)), this, SLOT(update()));
	connect(AnimantionMgr::instance(), SIGNAL(animationAdded(AnimationInfo*)), this, SLOT(update()));
}

TimeLineEditor::~TimeLineEditor()
{

}

void TimeLineEditor::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	QRect rc(rect());
	p.fillRect(rc, Qt::darkGray);
	p.setRenderHint(QPainter::Antialiasing);
	int iLeft = EDITOR_LEFT_PADDING;
	for (int i = 0; i < FRAME_LINE_COUNT; ++i)
	{
		QPen pen;
		pen.setWidth(FRAME_LINE_WIDTH);
		if (m_iCurTimeLineIndex == i)
			pen.setColor(Qt::red);
		else
		{
			if (i % 5 != 0)
				pen.setColor(Qt::gray);
			else
				pen.setColor(Qt::black);
		}
		if (i % 5 != 0)
			pen.setStyle(Qt::DashDotLine);
		p.setPen(pen);
		p.drawLine(iLeft, rc.top(), iLeft, rc.bottom());
		iLeft += FRAME_LINE_WIDTH + FRAME_LINE_MARGIN;
	}
	
	const int iEllipse = ANIMATION_LINE_HIEGHT + 6;
	const int iLineHeight = ANIMATION_LINE_HIEGHT;
	// »æÖÆAnimation
	QVector<AnimationInfo*> vecAni = AnimantionMgr::instance()->getAllAnimation();
	int iTop = ANIMATION_LINE_SPACE;
	for (int i = 0; i < vecAni.count(); ++i)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		const int iFrom = vecAni[i]->iTimeFrom;
		const int iEnd = iFrom + vecAni[i]->iDuration;
		int iXFrom	= GetXPoxFromTime(iFrom);
		int iXEnd = GetXPoxFromTime(iEnd);
		const int iYCenter = iTop - iLineHeight / 2;
		path.addEllipse(iXFrom - iEllipse / 2, iTop - iEllipse / 2, iEllipse, iEllipse);
		path.addRect(iXFrom, iTop - iLineHeight / 2, iXEnd - iXFrom, iLineHeight);
		path.addEllipse(iXEnd - iEllipse / 2, iTop - iEllipse / 2, iEllipse, iEllipse);
		QColor clr = Qt::blue;
		if (m_iCurAnimation == i)
			clr = Qt::red;

		p.fillPath(path, clr);
		p.drawRect(path.boundingRect());
		iTop += ANIMATION_LINE_SPACE + ANIMATION_LINE_HIEGHT;
	}
}

QSize TimeLineEditor::sizeHint() const
{
	return QSize(FRAME_LINE_COUNT * FRAME_LINE_MARGIN, 100);
}

void TimeLineEditor::mouseMoveEvent(QMouseEvent * event)
{

}

void TimeLineEditor::mousePressEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		m_ptPressed = event->pos();
		m_iCurAnimation = HitAnimationTest(event->pos());
		if (m_iCurAnimation >= 0)
			event->accept();
		emit currentAnimationChanged(m_iCurAnimation);
	}
	m_iCurTimeLineIndex = HitTimeLineTest(event->pos().x());
	update();
}

void TimeLineEditor::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton && m_ptPressed != QPoint(-1, -1))
	{
		update();
	}
}

int TimeLineEditor::HitTimeLineTest(const int x)
{
	const int iWidth = (FRAME_LINE_WIDTH + FRAME_LINE_MARGIN);
	int iIndex = (x - EDITOR_LEFT_PADDING) / iWidth;
	if ((x - EDITOR_LEFT_PADDING) % iWidth > iWidth / 2)
		iIndex++;
	
	return iIndex;
}

int TimeLineEditor::HitAnimationTest(QPoint pt)
{
	QVector<AnimationInfo*> vecAni = AnimantionMgr::instance()->getAllAnimation();
	int iTop = ANIMATION_LINE_SPACE;
	for (int i = 0; i < vecAni.count(); ++i)
	{
		const int iFrom = vecAni[i]->iTimeFrom;
		const int iEnd = iFrom + vecAni[i]->iDuration;
		int iXFrom = GetXPoxFromTime(iFrom);
		int iXEnd = GetXPoxFromTime(iEnd);
		QRect rc(iXFrom, iTop - ANIMATION_LINE_HIEGHT / 2, iXEnd - iXFrom, ANIMATION_LINE_HIEGHT);
		if (rc.contains(pt))
			return i;
		iTop += ANIMATION_LINE_SPACE + ANIMATION_LINE_HIEGHT;
	}
	return -1;
}

void TimeLineEditor::contextMenuEvent(QContextMenuEvent * event)
{
	QPoint pt = event->pos();
	const int iAnimation = HitAnimationTest(pt);
	QMenu* menu = new QMenu;
	if (iAnimation >= 0)
	{
		QAction* pAction = menu->addAction(QLatin1String("Remove Animation"));
		pAction->setData(iAnimation);
		connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onActionRemoveAnimation()));
	}
	else
	{
		const int iTime = HitTimeLineTest(pt.x()) * TIME_SCALE;
		QAction* pAction = menu->addAction(QLatin1String("Add Animation"));
		pAction->setData(iTime);
		connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onActionAddAnimation()), Qt::QueuedConnection);
	}
	menu->exec(event->globalPos());
	menu->deleteLater();
}

void TimeLineEditor::onActionAddAnimation()
{
	m_widgetAddAni.setStartTime(m_iCurTimeLineIndex * TIME_SCALE);

	if (QDialog::Accepted == m_widgetAddAni.exec())
	{
		AnimationInfo* pInfo = new AnimationInfo;
		pInfo->id = -1;
		pInfo->strName = m_widgetAddAni.getName();
		pInfo->iTimeFrom = m_widgetAddAni.getTimeStart();
		pInfo->iDuration = m_widgetAddAni.getDuration();
		AnimantionMgr::instance()->addAnimation(pInfo);
	}
}

int TimeLineEditor::GetXPoxFromTime(const int iTime)
{
	return (EDITOR_LEFT_PADDING + iTime / TIME_SCALE * (FRAME_LINE_MARGIN + FRAME_LINE_WIDTH));
}

AnimationInfo* TimeLineEditor::getCurrentAnimation()
{
	QVector<AnimationInfo*> vecAni = AnimantionMgr::instance()->getAllAnimation();
	if (m_iCurAnimation >= 0 && m_iCurAnimation < vecAni.count())
		return vecAni[m_iCurAnimation];

	return NULL;
}

void TimeLineEditor::onActionRemoveAnimation()
{
	QAction* pAction = qobject_cast<QAction*>(sender());
	if (pAction)
	{
		const int iIndex = pAction->data().toInt();
		if (iIndex == m_iCurAnimation)
			m_iCurAnimation = -1;
		AnimantionMgr::instance()->removeAnimation(iIndex);
	}
}
