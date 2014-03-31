#pragma once


#include "GraphicsUI/arrowlinkitem.h"
#include "../designnetconstants.h"
#include <QObject>
#include <QGraphicsItem>


namespace DesignNet
{
class PortItem;
class PortArrowLink : public GraphicsUI::ArrowLinkItem
{
	Q_OBJECT

public:

	TYPE(PortArrowLink, GraphicsItemType_PortConnect);

	PortArrowLink(QGraphicsItem* parent);
	~PortArrowLink();

	void connectPort(PortItem* pSrc, PortItem* pTarget);

	PortItem* getSrc() { return m_srcPortItem; }
	PortItem* getTarget() { return m_targetPortItem; }

	void onControlItemPostionChanged();
	void updatePosition();

protected slots:
	
	void onPortVisibleChanged();

protected:

	QVariant itemChange(GraphicsItemChange change, const QVariant & value) override;

private:

	PortItem* m_srcPortItem;
	PortItem* m_targetPortItem;
};
}