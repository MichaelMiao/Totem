#ifndef PROCESSORCONFIGWIDGET_H
#define PROCESSORCONFIGWIDGET_H

#include "../designnet_core_global.h"
#include <QWidget>

#define PROPERTY_WIDGET(X)	\
	virtual ProcessorConfigWidget* create(Processor* processor, QWidget *parent = 0, bool bAddProperty = false) \
	{ \
		return new X(processor, parent, bAddProperty); \
	}


QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

namespace DesignNet{
class ProcessorConfigPage;
class Processor;
class ProcessorConfigListener;
class DESIGNNET_CORE_EXPORT ProcessorConfigWidget : public QWidget
{
	Q_OBJECT

public:

	PROPERTY_WIDGET(ProcessorConfigWidget)
	ProcessorConfigWidget(Processor* processor, QWidget *parent, bool bPropertyPage = false);
	virtual ~ProcessorConfigWidget();

	virtual void init();
	void addPropertyPage();
	void addPage(ProcessorConfigPage* page);
	void addListener(ProcessorConfigListener *listener);

public slots:

	void onSettingsChanged();

protected:
	
	void showEvent(QShowEvent *);

	QTabWidget*	m_tabWidget;
	Processor*	m_processor;
	QList<ProcessorConfigListener*> m_listeners;
};

}

#endif // PROCESSORCONFIGWIDGET_H
