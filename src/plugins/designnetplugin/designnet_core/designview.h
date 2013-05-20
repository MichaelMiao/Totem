#ifndef DESIGNVIEW_H
#define DESIGNVIEW_H

#include <QGraphicsView>
#include <QMultiHash>

namespace DesignNet{

class GraphicsBlockScene;
class DesignNetSpace;
class Processor;
class ProcessorGraphicsBlock;
class DesignNetSpace;
class PortArrowLinkItem;
class PortGraphicsItem;
class Port;
/*!
 * \brief The DesignView class
 *
 * DesignNet主窗口，使用QGraphicsView Framework, 当鼠标从\e ToolDockWidgetView 拖拽工具到
 * 该区域后松开鼠标，就会创建一个Processor, \e Processor 类完成处理器非UI相关操作，
 * \e ProcessorGraphicsBlock 继承自Processor，完成可视的Processor操作。
 */
class DesignView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DesignView(DesignNetSpace *space = 0, QWidget *parent = 0);
	
	virtual ~DesignView();
	void setDesignNetSpace(DesignNetSpace *space);
    DesignNetSpace* getSpace() const;//!< 返回相应的Space
    void removeItems(QList<QGraphicsItem*> items);//!< 删除Item
    virtual void contextMenuEvent ( QContextMenuEvent * event );
protected:
    virtual void dragEnterEvent ( QDragEnterEvent * event );
    virtual void dragLeaveEvent ( QDragLeaveEvent * event );
    virtual void dragMoveEvent ( QDragMoveEvent * event );
    virtual void dropEvent ( QDropEvent * event );

    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );

    void wheelEvent(QWheelEvent *event);

    virtual void keyPressEvent ( QKeyEvent * keyEvent );

    PortGraphicsItem* getPortGraphicsItem(Port* port);

    void removeArrow(PortArrowLinkItem* arrow);
    void updatedSelectedItems();
    void removeProcessor(ProcessorGraphicsBlock *processor);
signals:
    void processorSelected(QList<Processor*> processors);
	void processorAdded(Processor* processor);
public slots:
    void processorClosed();
	void onProcessorAdded(Processor *processor);
    void onConnectionAdded(Port* inputPort, Port* outputPort);
    void onConnectionRemoved(Port* inputPort, Port* outputPort);
	void reloadSpace();
private:
    QMultiHash<Port*, PortArrowLinkItem*> m_arrowLinkItems;
    DesignNetSpace* m_designnetSpace;
    ProcessorGraphicsBlock* m_pMousePressedItem;
	PortArrowLinkItem*			m_currentArrowLinkItem;
	PortGraphicsItem*			m_pressedPort;//!< 鼠标在该Port上按下
};
}

#endif // DESIGNVIEW_H
