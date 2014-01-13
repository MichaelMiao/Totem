#ifndef PROCESSORARROWLINK_H
#define PROCESSORARROWLINK_H

#include "GraphicsUI/arrowlinkitem.h"
#include <QObject>

namespace DesignNet{
class ProcessorGraphicsBlock;

class ProcessorArrowLink : public GraphicsUI::ArrowLinkItem
{
	Q_OBJECT

public:
	ProcessorArrowLink(QGraphicsItem *parent);
	~ProcessorArrowLink();

	void connectProcessor(ProcessorGraphicsBlock* pSrc, ProcessorGraphicsBlock* pTarget);
	ProcessorGraphicsBlock* getSrc() { return m_srcProcessor; }
	ProcessorGraphicsBlock* getTarget() { return m_targetProcessor; }

	void onControlItemPostionChanged();
	void updatePosition();

protected slots:
	void onProcessorPosChanged();
protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value );
private:
	ProcessorGraphicsBlock* m_srcProcessor;
	ProcessorGraphicsBlock* m_targetProcessor;
};

}

#endif // PROCESSORARROWLINK_H
