#ifndef PROCESSORCONFIGPAGE_H
#define PROCESSORCONFIGPAGE_H

#include <QWidget>
#include "customui/uicollapsiblewidget.h"
#include "../designnet_core_global.h"
QT_BEGIN_NAMESPACE
class QDialogButtonBox;
QT_END_NAMESPACE

namespace DesignNet{
class DESIGNNET_CORE_EXPORT ProcessorConfigPage : public CustomUI::uiCollapsibleWidget
{
	Q_OBJECT

public:
	enum PageType{
		PageType_Permanent,	//!< 永久性参数
		PageType_RealTime	//!< 实时性参数
	};
	ProcessorConfigPage(PageType pt = PageType_RealTime, QWidget *parent = 0);
	virtual ~ProcessorConfigPage();
	PageType pageType();
signals:
	void configFinished();//!< 配置完成
public slots:
	void setApplyButtonEnabled(bool bEnabled);
private:
	PageType m_pageType;//!< 页类型
	QDialogButtonBox *m_buttonBox;
};

}

#endif // PROCESSORCONFIGPAGE_H
