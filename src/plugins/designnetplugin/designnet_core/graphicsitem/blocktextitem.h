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
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
private:
	
};
}

#endif // BLOCKTEXTITEM_H
