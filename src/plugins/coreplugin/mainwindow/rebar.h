#pragma once
#include <QTabWidget>
#include "../actionmanager/command.h"
#include "../Common/id.h"
#include "CustomUI/flowlayout.h"


class RebarCtrl : public QWidget
{
	Q_OBJECT
public:

	RebarCtrl(QWidget* pParent);
	~RebarCtrl();

	QSize	sizeHint() const override;

	void	paintEvent(QPaintEvent *e) override;

public slots:

	void onRebarObjectAdded(const Core::Id & id, Core::Command* pCmd);

private:

	QTabWidget* m_pTab;
	CustomUI::FlowLayout* m_pToolbarLayout;
};
