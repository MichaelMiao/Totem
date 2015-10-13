#ifndef TOOLTIPGRAPHICSITEM_H
#define TOOLTIPGRAPHICSITEM_H

#include <QGraphicsObject>
#include <QGraphicsTextItem>
#include "designnet_core_global.h"

namespace DesignNet{
class ToolTipGraphicsItemPrivate;
class IData;
class Port;
class ProcessorGraphicsBlock;
/*!
 * \brief The ToolTipGraphicsItem class DesignView中显示的提示
 *
 * 该提示具有能够显示指定数据类型的数据的能力
 */
class DESIGNNET_CORE_EXPORT ToolTipGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    ToolTipGraphicsItem(QGraphicsItem *parent);
	virtual ~ToolTipGraphicsItem();
    
	QRectF boundingRect () const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	void setText(const QString& text);
	void setPort(Port* pPort);
	void setData(QVariant v);
	bool topmost() const;

    QVariant itemChange(GraphicsItemChange change, const QVariant & value);

public slots:

	void onClosed();
    void onTopMost(bool topmost);
	void relayout(); //!< 重新布局
	void onVisibleChanged();

protected:

	void focusOutEvent(QFocusEvent *event);

    ToolTipGraphicsItemPrivate *d;
};
}

#endif // TOOLTIPGRAPHICSITEM_H
