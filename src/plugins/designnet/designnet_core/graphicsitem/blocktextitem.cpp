#include "blocktextitem.h"
#include <QPainter>
#include <QFontMetrics>

namespace DesignNet{
BlockTextItem::BlockTextItem(QGraphicsItem *parent)
	: QGraphicsTextItem(parent)
{
	m_blockName = "";
	m_maxWidth = 120;
}

BlockTextItem::~BlockTextItem()
{

}

void BlockTextItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	QRectF rectF = boundingRect();
	painter->save();
	painter->setBrush(Qt::black);
	painter->drawRoundedRect(rectF, 10, 15);

	painter->setPen(Qt::white);
	QFontMetrics fm(font());
	QRectF rect = fm.boundingRect(m_blockName);

	QString tempBlockName = fm.elidedText(m_blockName, Qt::ElideRight, qMin(rectF.width(), (qreal)m_maxWidth));
	painter->drawText(rectF,  Qt::AlignCenter, tempBlockName);
	painter->restore();	
}

QRectF BlockTextItem::boundingRect() const
{
	QFontMetrics fm(font());
	QRectF rect = fm.boundingRect(m_blockName);
	QRect rt(0, 0, 
		qMin((qreal)(rect.width() + fm.averageCharWidth() * 3), (qreal)(m_maxWidth)), 
		rect.height() + fm.xHeight() * 2);
	return rt;
}

void BlockTextItem::setBlockName( const QString &blockName )
{
	m_blockName = blockName;
	setToolTip(blockName);
}

void BlockTextItem::setSize( const QSizeF &sz )
{
	m_size = sz;
	update();
}

QSizeF BlockTextItem::getSize() const
{
	return m_size;
}

void BlockTextItem::setMaxWidth( const float &fWidth )
{
	if (fWidth <= 0)
	{
		return ;
	}
	m_maxWidth = fWidth;
}

}
