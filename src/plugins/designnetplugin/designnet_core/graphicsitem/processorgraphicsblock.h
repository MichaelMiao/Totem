#ifndef PROCESSORGRAPHICSBLOCK_H
#define PROCESSORGRAPHICSBLOCK_H


#include "../designnet_core_global.h"
#include "../designnetbase/processor.h"
#include "../designnetconstants.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"

#include <QList>
#include <QIcon>
#include <QGraphicsItem>
namespace DesignNet{
class Block;
class PortGraphicsItem;
class DesignView;
class PortArrowLinkItem;
class DESIGNNET_CORE_EXPORT ProcessorGraphicsBlock
        : public Processor, public QGraphicsItem
{
    Q_OBJECT
public:
	enum{DEFAULT_WIDTH = 110,
		DEFAULT_HEIGHT = 150,
		TITLE_HEIGHT = 30,
		ITEMSPACING = 5};
    enum {Type = UserType + ProcessorGraphicsBlockType};
    int type() const;
    ProcessorGraphicsBlock( DesignNetSpace *space, QGraphicsItem *parent = 0);
    virtual ~ProcessorGraphicsBlock();
    QRectF  boundingRect() const;
    QSizeF  minimumSizeHint() const;


	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem *option,
		QWidget* widget = 0 );


    virtual void layoutItems(); //!< 布局所有的子Item，如Port
    
	virtual QString title() const;    //!< 返回title
	void setTitle(const QString &title);

    virtual QIcon getIcon() const;      //!< 该Processor的图标
    void setIcon(const QIcon &icon);    //!< 设置图标

    Processor* processor();     //!< 返回Processor

    
    virtual void initialize();  //!< 初始化

    bool connect(PortGraphicsItem *inputPort, PortGraphicsItem *outputPort);
    PortGraphicsItem *getPortGraphicsItem(Port* port);
    
	virtual void propertyRemoving(Property* prop);
    virtual void propertyRemoved(Property* prop);
	virtual void propertyAdded(Property* prop);

    void detach();              //!< 移除所有连接

    virtual Core::Id typeID() const;
    virtual QString category() const;
signals:
    void arrowStarted(PortArrowLinkItem* item);
    void selectionChanged(bool bSelected);
    
signals:
	void closed();
	
public slots:
	void onPropertyChanged_internal();
	void relayout();
protected:
	QRectF mainRect() const;        //!< 返回除了Title部分的Main区域大小

	virtual bool process();     //!< 处理函数
	virtual void propertyChanged(Property *prop);
    void stateChanged(Port *port);
    virtual void dataArrived(Port* port);  //!< 数据到达

    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void createPortItems();     //!< 创建端口


	virtual void	hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void	hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	Block* m_block;
	GraphicsUI::GraphicsAutoShowHideItem*   m_closeItem;
	QGraphicsTextItem*                      m_titleItem;
    QList<PortGraphicsItem*>    m_inputPortItems;
    QList<PortGraphicsItem*>    m_outputPortItems;
    QIcon                       m_icon;
};
}

#endif // PROCESSORGRAPHICSBLOCK_H
