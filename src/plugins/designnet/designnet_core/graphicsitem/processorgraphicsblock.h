#ifndef PROCESSORGRAPHICSBLOCK_H
#define PROCESSORGRAPHICSBLOCK_H


#include "../designnet_core_global.h"
#include "../designnetbase/processor.h"
#include "../designnetconstants.h"
#include "Utils/XML/xmlserializable.h"

#include <QList>
#include <QIcon>
#include <QGraphicsObject>

#define DECLARE_PROCESSOR_SERIALIZABLE(TNAME) \
	DECLARE_SERIALIZABLE(TNAME, ProcessorGraphicsBlock)

namespace Utils{
class XmlSerializer;
}
QT_BEGIN_NAMESPACE
class QMenu;
class QGraphicsDropShadowEffect;
class  QGraphicsSceneMouseEvent;
QT_END_NAMESPACE
namespace GraphicsUI{
class GraphicsToolButton;
class TextAnimationBlock;
class GraphicsAutoShowHideItem;
}

namespace DesignNet{


class DESIGNNET_CORE_EXPORT Position : public Utils::XmlSerializable
{
public:
	DECLARE_SERIALIZABLE(Position, Position)
	Position(){m_x = 0; m_y = 0;m_id = -1;}
	Position(const float &x, const float &y){m_x = x;m_y = y;}
	Position(const QPointF &point){m_x = point.x(); m_y = point.y();}
	
	virtual void serialize(Utils::XmlSerializer& s)const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
	
	float	m_x;
	float	m_y;
	int		m_id;
};
class BlockTextItem;
class DesignView;
class ProcessorConfigWidget;

class DESIGNNET_CORE_EXPORT ProcessorGraphicsBlock
        : public QGraphicsObject
{
    Q_OBJECT
public:

	/**
	 * \enum	
	 *
	 * \brief	显示宽度、高度 .
	 */

	enum{
		DEFAULT_WIDTH	= 110, //!< 默认宽度
		DEFAULT_HEIGHT	= 80,//!< 默认高度
		TITLE_HEIGHT	= 30,	 //!< title高度
		ITEMSPACING		= 5,	 //!< spacing
		STATUSBAR_HEIGHT = 15   //!< 状态栏高度
	};
    enum {Type = UserType + ProcessorGraphicsBlockType};
	enum State{
		STATE_WORKING	= 0x000001,
		STATE_MOUSEOVER = 0x000001 << 1,
		STATE_SELECTED  = 0x000001 << 2,
		STATE_EMPHASIZE = 0x000001 << 3,
	};

    int type() const;
    ProcessorGraphicsBlock(Processor *processor = 0, QGraphicsScene *scene = 0, 
		const QPointF &pos = QPointF(0, 0), QGraphicsItem *parent = 0);
    virtual ~ProcessorGraphicsBlock();
	virtual void init();
    QRectF  boundingRect() const;
    QSizeF  minimumSizeHint() const;

	virtual float statusBarHeight();

	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem *option,
		QWidget* widget = 0 );

    Processor* processor();     //!< 返回Processor

	virtual void propertyRemoving(Property* prop);
    virtual void propertyRemoved(Property* prop);
	virtual void propertyAdded(Property* prop);

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

	Position originalPosition() const;
	virtual void createContextMenu(QMenu *parentMenu);

	
	void setHover(bool bHovered = true);
	bool isHover() { return testState(STATE_MOUSEOVER); }

	void setEmphasized(bool bEmphasized = true);
	bool isEmphasized() { return testState(STATE_EMPHASIZE); }

	QPointF getCrossPoint(const QLineF &line);
signals:
    void selectionChanged(bool bSelected);
    void positionChanged();

	void closed();
	void processorLog(const QString &log);
public slots:
	void onPropertyChanged_internal();
	void onShowLog(const QString &log);
	void configWidgetClosed();
	void startDeserialize(Utils::XmlDeserializer& s);
protected:
	void setState(const State &s, const bool &bAdd = false); //!< 设置状态，\e bAdd \e 表示是否以添加的形式添加属性。
	bool testState(const State &s) { return m_state & s; }
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void propertyChanged(Property *prop);

    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	void mouseDoubleClickEvent (  QGraphicsSceneMouseEvent * event );
	virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	void setLayoutDirty(const bool &dirty = true);			//!< 设置是否需要重新计算布局
	void showStatus(const QString &msg);

	QGraphicsDropShadowEffect*				m_dropdownShadowEffect;
	GraphicsUI::GraphicsAutoShowHideItem*   m_closeItem;
	BlockTextItem*							m_titleItem;
	Position					m_pos;
	mutable bool				m_layoutDirty;	//!< 是否需要重新计算布局
	mutable QSizeF				m_mainSize;		//!< main区域的size
	int							m_state;		//!< block状态
	Processor*					m_processor;
	ProcessorConfigWidget*		m_configWidget;
};
}

#endif // PROCESSORGRAPHICSBLOCK_H
