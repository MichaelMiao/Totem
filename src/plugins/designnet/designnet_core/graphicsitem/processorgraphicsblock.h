#ifndef PROCESSORGRAPHICSBLOCK_H
#define PROCESSORGRAPHICSBLOCK_H


#include "../designnet_core_global.h"
#include "../designnetbase/processor.h"
#include "../designnetconstants.h"
#include "../designnetbase/port.h"
#include "Utils/XML/xmlserializable.h"

#include <QList>
#include <QIcon>
#include <QGraphicsItem>

#define DECLARE_PROCESSOR_SERIALIZABLE(TNAME) \
	DECLARE_SERIALIZABLE(TNAME, ProcessorGraphicsBlock)

namespace Utils{
class XmlSerializer;
}
QT_BEGIN_NAMESPACE
class QMenu;
class QGraphicsBlurEffect;
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
class PortGraphicsItem;
class BlockTextItem;
class DesignView;
class PortArrowLinkItem;
class ProcessorConfigWidget;
class DESIGNNET_CORE_EXPORT ProcessorGraphicsBlock
        : public QObject, public QGraphicsItem
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
		DEFAULT_HEIGHT	= 150,//!< 默认高度
		TITLE_HEIGHT	= 30,	 //!< title高度
		ITEMSPACING		= 5,	 //!< spacing
		STATUSBAR_HEIGHT = 15   //!< 状态栏高度
	};
    enum {Type = UserType + ProcessorGraphicsBlockType};
	enum State{
		STATE_WORKING	= 0x000001,
		STATE_MOUSEOVER = 0x000001 << 1,
		STATE_SELECTED  = 0x000001 << 2
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


    virtual void layoutItems(); //!< 布局所有的子Item，如Port

    Processor* processor();     //!< 返回Processor

    bool connect(PortGraphicsItem *inputPort, PortGraphicsItem *outputPort);
    PortGraphicsItem *getPortGraphicsItem(Port* port);
    
	virtual void propertyRemoving(Property* prop);
    virtual void propertyRemoved(Property* prop);
	virtual void propertyAdded(Property* prop);

    void detach();              //!< 移除所有连接


	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

	Position originalPosition() const;
	virtual void createContextMenu(QMenu *parentMenu);
	void setPortCountResizable(const bool &bResizable = true);
	void removePort(PortGraphicsItem* port);

	void setState(const State &s, const bool &bAdd = false); //!< 设置状态，\e bAdd \e 表示是否以添加的形式添加属性。
signals:
    void arrowStarted(PortArrowLinkItem* item);
    void selectionChanged(bool bSelected);
    
signals:
	void closed();
	void processorLog(const QString &log);
public slots:
	void onPropertyChanged_internal();
	void relayout();
	void onRemovePort();
	void onAddPort();
	void onShowLog(const QString &log);
	void configWidgetClosed();
	void startDeserialize(Utils::XmlDeserializer& s);
protected:

	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event );
	virtual QRectF mainRect() const;        //!< 返回除了Title部分的Main区域大小
	virtual QSizeF mainSize() const;		//!< 返回main区域的size
	virtual void propertyChanged(Property *prop);
    void stateChanged(Port *port);

    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void createPortItems();     //!< 创建端口
	void mouseDoubleClickEvent (  QGraphicsSceneMouseEvent * event );
	virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	void setLayoutDirty(const bool &dirty = true);			//!< 设置是否需要重新计算布局
	void showStatus(const QString &msg);
	QGraphicsBlurEffect*					m_blurEffect;
	GraphicsUI::GraphicsAutoShowHideItem*   m_closeItem;
	GraphicsUI::GraphicsToolButton*			m_addInputPortItem;
	BlockTextItem*							m_titleItem;
	GraphicsUI::TextAnimationBlock*			m_statusBlock;
    QList<PortGraphicsItem*>    m_inputPortItems;
    QList<PortGraphicsItem*>    m_outputPortItems;
	Position					m_pos;
	mutable bool				m_layoutDirty;	//!< 是否需要重新计算布局
	mutable QSizeF				m_mainSize;		//!< main区域的size
	int							m_state;		//!< block状态
	Processor*					m_processor;
	ProcessorConfigWidget*		m_configWidget;
};
}

#endif // PROCESSORGRAPHICSBLOCK_H
