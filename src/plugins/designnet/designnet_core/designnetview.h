#ifndef DESIGNNETVIEW_H
#define DESIGNNETVIEW_H

#include "Utils/XML/xmlserializable.h"
#include <QGraphicsView>
#include <QList>
QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

namespace Utils{
class XmlDeserializer;
class XmlSerializer;
}

namespace DesignNet{
class Port;
class DesignNetViewPrivate;
class DesignNetSpace;
class PortArrowLinkItem;
class PortGraphicsItem;
class Processor;
class ProcessorGraphicsBlock;
/*!
 * \brief The DesignView class
 *
 * DesignNet主窗口，使用QGraphicsView Framework, 当鼠标从\e ToolDockWidgetView 拖拽工具到
 * 该区域后松开鼠标，就会创建一个Processor, \e Processor 类完成处理器非UI相关操作，
 * \e ProcessorGraphicsBlock 继承自Processor，完成可视的Processor操作。
 */

class DesignNetView : public QGraphicsView
{
	Q_OBJECT

public:
	DesignNetView(DesignNetSpace *space, QWidget *parent = 0);
	virtual ~DesignNetView();
	void setDesignNetSpace(DesignNetSpace *space);
	DesignNetSpace* getSpace() const;//!< 返回相应的Space

	virtual void dragEnterEvent ( QDragEnterEvent * event );
	virtual void dropEvent ( QDropEvent * event );
	virtual void dragLeaveEvent ( QDragLeaveEvent * event );
	virtual void dragMoveEvent ( QDragMoveEvent * event );
	virtual void keyReleaseEvent(QKeyEvent *event);

	void deserialize(Utils::XmlDeserializer &x);
	void serialize(Utils::XmlSerializer &s);

public slots:

	void onProcessorAdded(Processor* processor);
	void onProcessorRemoved(Processor* processor);
	void processorClosed();
	void reloadSpace();
	void OnShowMessage(const QString &strMessage);
protected:

	ProcessorGraphicsBlock *getGraphicsProcessor(const int &id);
	virtual void mouseMoveEvent ( QMouseEvent * event );
	virtual void mousePressEvent ( QMouseEvent * event );
	virtual void mouseReleaseEvent ( QMouseEvent * event );

	void removeItems(QList<QGraphicsItem*> items);//!< 删除Item
	void addProcessor(ProcessorGraphicsBlock *processor);
	void removeProcessor(ProcessorGraphicsBlock *processor);
private:
	DesignNetViewPrivate* d;
	bool		m_bLinking;//!<		连接
};

}

#endif // DESIGNNETVIEW_H
