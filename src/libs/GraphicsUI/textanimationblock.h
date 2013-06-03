#ifndef TEXTANIMATIONBLOCK_H
#define TEXTANIMATIONBLOCK_H

#include <QGraphicsObject>
#include "graphicsui_global.h"
namespace GraphicsUI{
class TextAnimationBlockPrivate;

/**
 * \class	TextAnimationBlock
 *
 * \brief	动画显示文字，继承自QGraphicsItem
 *
 * \author	Michael_BJFU
 * \date	2013/5/27
 */

class TOTEM_GRAPHICSUI_EXPORT TextAnimationBlock : public QGraphicsObject
{
	Q_OBJECT
	Q_PROPERTY(float m_yDelta READ getYDelta WRITE setyDelta)
public:
	TextAnimationBlock(QGraphicsItem *parent = 0);
	~TextAnimationBlock();
	
	void animateShow(const bool &bShow = true); //!< 
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget);
	
	void setFont(const QFont &font);
	void setSizeF(const QSizeF &size);
	virtual QRectF	boundingRect () const;

	float getYDelta() const;
	void setyDelta(const float& yDelta);

	void setContentsMargins ( qreal left, qreal top, qreal right, qreal bottom );

	void setTextColor(const QColor &color);
public slots:
    void animationFadeFinished();
	void showText(const QString &text);			//!< 显示文字，调用animateShow
private:
	TextAnimationBlockPrivate *d;
	float m_yDelta;				//!< y方向上的Delta。 
};

}

#endif // TEXTANIMATIONBLOCK_H
