#ifndef BLOCKTEXTITEM_H
#define BLOCKTEXTITEM_H

#include <QGraphicsTextItem>
namespace DesignNet{
class BlockTextItem : public QGraphicsTextItem
{
	Q_OBJECT

public:
	BlockTextItem(QGraphicsItem *parent);
	~BlockTextItem();
	void setMaxWidth(const float &fWidth);
	void setBlockName(const QString &blockName);
	virtual QRectF	boundingRect () const;
	void	setSize(const QSizeF &sz) ;
	QSizeF  getSize() const;
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
private:
	QString m_blockName;
	QSizeF	m_size;
	float	m_maxWidth;
};
}

#endif // BLOCKTEXTITEM_H
