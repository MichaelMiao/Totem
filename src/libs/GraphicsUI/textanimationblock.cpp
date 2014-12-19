#include "totem_gui_pch.h"
#include "textanimationblock.h"
#include <QPropertyAnimation>
#include <QPainter>
#include <QFont>
#include <QTextOption>
#include <QParallelAnimationGroup>
#include <QDebug>
namespace GraphicsUI{

class TextAnimationBlockPrivate{
public:
	TextAnimationBlockPrivate(){m_animationFade = 0; m_animationFade = 0;}
	~TextAnimationBlockPrivate(){}
	QPropertyAnimation *m_animationFade;				//!< 淡入淡出动画
	QPropertyAnimation *m_animationScroll;				//!< 滚动动画
	QParallelAnimationGroup *m_parallelAnimation;	
	QString				m_textTemp;
	QString				m_text;
	QFont				m_font;
	QSizeF				m_sizeF;
	QColor				m_textColor;
	float				m_leftMargin;
	float				m_topMargin;
	float				m_rightMargin;
	float				m_bottomMargin;
};
TextAnimationBlock::TextAnimationBlock(QGraphicsItem *parent)
	: QGraphicsObject(parent),
	d(new TextAnimationBlockPrivate())
{
	d->m_font		=  QFont("Times", 10);
	setSizeF(QSizeF(100, 100));
	m_yDelta = 0;
	d->m_textColor = Qt::black;
	d->m_leftMargin = d->m_topMargin = d->m_rightMargin = d->m_bottomMargin = 3;
	
}

TextAnimationBlock::~TextAnimationBlock()
{
	delete d;
}

void TextAnimationBlock::animateShow( const bool &bShow )
{
	d->m_parallelAnimation= new QParallelAnimationGroup(this);
	d->m_animationFade	= new QPropertyAnimation(this, "opacity");
	d->m_animationScroll	= new QPropertyAnimation(this, "m_yDelta");



	d->m_animationFade->setDuration(500);
	d->m_animationFade->setStartValue(0.0);
	d->m_animationFade->setEndValue(1.0);
	d->m_animationScroll->setDuration(500);
	d->m_animationScroll->setStartValue(d->m_sizeF.height()/2);
	d->m_animationScroll->setEndValue(0);
	d->m_parallelAnimation->addAnimation(d->m_animationFade);
	d->m_parallelAnimation->addAnimation(d->m_animationScroll);
	
	if (!bShow)
	{
		d->m_animationFade->setDirection(QAbstractAnimation::Backward);
		d->m_animationScroll->setDirection(QAbstractAnimation::Backward);
		d->m_parallelAnimation->setDirection(QAbstractAnimation::Backward);
		connect(d->m_parallelAnimation, SIGNAL(finished()), this, SLOT(animationFadeFinished()));
	}
	else
	{
		d->m_animationScroll->setDirection(QAbstractAnimation::Forward);
		d->m_animationFade->setDirection(QAbstractAnimation::Forward);
		d->m_parallelAnimation->setDirection(QAbstractAnimation::Forward);
	}

	d->m_parallelAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TextAnimationBlock::showText( const QString &text )
{
	d->m_textTemp = text;
	update();
	animateShow(false);
}

void TextAnimationBlock::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	painter->setFont(d->m_font);
	QRectF rect = boundingRect();
	rect.setTop(rect.top() + m_yDelta);
	rect.adjust(d->m_leftMargin, d->m_topMargin, -d->m_rightMargin, -d->m_bottomMargin);
	QTextOption textOption(Qt::AlignLeft);
	textOption.setWrapMode(QTextOption::NoWrap);
	painter->setPen(d->m_textColor);
	painter->drawText(rect, d->m_text, textOption);
}

void TextAnimationBlock::setFont( const QFont &font )
{
	d->m_font = font;
}

void TextAnimationBlock::setSizeF( const QSizeF &size )
{
	prepareGeometryChange();
	d->m_sizeF = size;
	update();
}

QRectF TextAnimationBlock::boundingRect() const
{
	return QRectF(0, 0, d->m_sizeF.width(), d->m_sizeF.height());
}

float TextAnimationBlock::getYDelta() const
{
	return m_yDelta;
}

void TextAnimationBlock::setyDelta( const float& yDelta )
{
	m_yDelta = yDelta;
}

void TextAnimationBlock::animationFadeFinished()
{
	if (d->m_parallelAnimation->direction() == QAbstractAnimation::Backward)
	{
		d->m_text = d->m_textTemp;
		animateShow(true);
	}
}

void TextAnimationBlock::setContentsMargins( qreal left, qreal top, qreal right, qreal bottom )
{
	d->m_leftMargin		= left;
	d->m_topMargin		= top;
	d->m_rightMargin	= right;
	d->m_bottomMargin	= bottom;
	update();
}

void TextAnimationBlock::setTextColor( const QColor &color )
{
	d->m_textColor = color;
	update();
}

}
