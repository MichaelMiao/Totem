#ifndef PROCESSORGRAPHICSBLOCK_H
#define PROCESSORGRAPHICSBLOCK_H


#include "../designnet_core_global.h"
#include "../designnetbase/processor.h"
#include "../designnetconstants.h"
#include "../designnetbase/port.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"

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
QT_END_NAMESPACE
namespace GraphicsUI{
class GraphicsToolButton;
class TextAnimationBlock;
}

namespace DesignNet{


class DESIGNNET_CORE_EXPORT Position : public Utils::XmlSerializable
{
public:
	DECLARE_SERIALIZABLE(Position, Position)
	Position(){m_x = 0; m_y = 0;}
	Position(const float &x, const float &y){m_x = x;m_y = y;}
	Position(const QPointF &point){m_x = point.x(); m_y = point.y();}
	
	virtual void serialize(Utils::XmlSerializer& s)const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
	
	float m_x;
	float m_y;
};
class Block;
class PortGraphicsItem;
class DesignView;
class PortArrowLinkItem;
class DESIGNNET_CORE_EXPORT ProcessorGraphicsBlock
        : public Processor, public QGraphicsItem
{
    Q_OBJECT
public:

	/**
	 * \enum	
	 *
	 * \brief	显示宽度、高度 .
	 */

	enum{
		DEFAULT_WIDTH = 110, //!< 默认宽度
		DEFAULT_HEIGHT = 150,//!< 默认高度
		TITLE_HEIGHT = 30,	 //!< title高度
		ITEMSPACING = 5,	 //!< spacing
		STATUSBAR_HEIGHT = 30   //!< 状态栏高度
	};
    enum {Type = UserType + ProcessorGraphicsBlockType};
    int type() const;
    ProcessorGraphicsBlock( DesignNetSpace *space, QGraphicsItem *parent = 0);
    virtual ~ProcessorGraphicsBlock();
	virtual void init();
    QRectF  boundingRect() const;
    QSizeF  minimumSizeHint() const;

	virtual float statusBarHeight();

	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem *option,
		QWidget* widget = 0 );


    virtual void layoutItems(); //!< 布局所有的子Item，如Port
    
	virtual QString title() const;    //!< 返回title
	void setTitle(const QString &title);

    virtual QIcon getIcon() const;      //!< 该Processor的图标
    void setIcon(const QIcon &icon);    //!< 设置图标

    Processor* processor();     //!< 返回Processor

    
// 	void setScenePosition(Position &position);
// 	Position scenePosition();
    bool connect(PortGraphicsItem *inputPort, PortGraphicsItem *outputPort);
    PortGraphicsItem *getPortGraphicsItem(Port* port);
    
	virtual void propertyRemoving(Property* prop);
    virtual void propertyRemoved(Property* prop);
	virtual void propertyAdded(Property* prop);

    void detach();              //!< 移除所有连接

    virtual Core::Id typeID() const;
    virtual QString category() const;
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

	Position originalPosition() const;
	virtual void createContextMenu(QMenu *parentMenu);


	void setPortCountResizable(const bool &bResizable = true);
	void removePort(PortGraphicsItem* port);
signals:
    void arrowStarted(PortArrowLinkItem* item);
    void selectionChanged(bool bSelected);
    
signals:
	void closed();
	
public slots:
	void onPropertyChanged_internal();
	void relayout();
	void onRemovePort();
	void onAddPort();
protected:
	virtual QRectF mainRect() const;        //!< 返回除了Title部分的Main区域大小
	virtual QSizeF mainSize() const;		//!< 返回main区域的size
	virtual bool process();     //!< 处理函数
	virtual void propertyChanged(Property *prop);
    void stateChanged(Port *port);
    virtual void dataArrived(Port* port);  //!< 数据到达

    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void createPortItems();     //!< 创建端口

	virtual bool beforeProcess();		//!< 处理之前的准备,这里会确保数据已经准备好了
	virtual void afterProcess(bool status = true);		//!< 完成处理

	virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	void setLayoutDirty(const bool &dirty = true);			//!< 设置是否需要重新计算布局
	void showStatus(const QString &msg);
	Block* m_block;
	GraphicsUI::GraphicsAutoShowHideItem*   m_closeItem;
	GraphicsUI::GraphicsToolButton*			m_addInputPortItem;
	QGraphicsTextItem*                      m_titleItem;
	GraphicsUI::TextAnimationBlock*			m_statusBlock;
    QList<PortGraphicsItem*>    m_inputPortItems;
    QList<PortGraphicsItem*>    m_outputPortItems;
	
    QIcon                       m_icon;
	Position					m_pos;
	mutable bool						m_layoutDirty;	//!< 是否需要重新计算布局
	mutable QSizeF						m_mainSize;		//!< main区域的size
	bool						m_portCountResizable;

};
}

#endif // PROCESSORGRAPHICSBLOCK_H
