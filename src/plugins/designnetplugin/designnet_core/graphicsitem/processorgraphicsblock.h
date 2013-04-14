#ifndef PROCESSORGRAPHICSBLOCK_H
#define PROCESSORGRAPHICSBLOCK_H

#include "graphicsblock.h"
#include "../designnet_core_global.h"
#include "../designnetbase/processor.h"
#include "../designnetconstants.h"
#include <QList>

namespace DesignNet{
class PortGraphicsItem;
class DesignView;
class DESIGNNET_CORE_EXPORT ProcessorGraphicsBlock
        : public GraphicsBlock, public Processor
{
    Q_OBJECT
public:
    enum {Type = UserType + ProcessorGraphicsBlockType};
    int type() const;
    ProcessorGraphicsBlock( DesignNetSpace *space, QGraphicsItem *parent = 0);
    virtual ~ProcessorGraphicsBlock();
    QRectF  boundingRect() const;
    QSizeF  minimumSizeHint() const;

    virtual void layoutItems(); //!< 布局所有的子Item，如Port
    virtual QString title() const;    //!< 返回title

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
    void logout(QString log);
public slots:
	void onPropertyChanged_internal();
protected:
	virtual bool process();     //!< 处理函数
	virtual void propertyChanged(Property *prop);
    void stateChanged(Port *port);
    virtual void dataArrived(Port* port);  //!< 数据到达
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void createPortItems();     //!< 创建端口
    QList<PortGraphicsItem*>    m_inputPortItems;
    QList<PortGraphicsItem*>    m_outputPortItems;
    QIcon                       m_icon;
};
}

#endif // PROCESSORGRAPHICSBLOCK_H
