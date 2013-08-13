#ifndef PROCESSORCONFIGWIDGET_H
#define PROCESSORCONFIGWIDGET_H

#include <QWidget>
#include "../designnet_core_global.h"
#include <QList>
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
	ProcessorConfigWidget(Processor* processor, QWidget *parent);
	virtual ~ProcessorConfigWidget();
	virtual ProcessorConfigWidget* create(Processor* processor, QWidget *parent = 0);
	void addPropertyPage(Processor* processor);
	void addPage(ProcessorConfigPage* page);
	void addListener(ProcessorConfigListener *listener);
public slots:
	void onSettingsChanged();
private:
	QTabWidget*	m_tabWidget;
	Processor*	m_processor;
	QList<ProcessorConfigListener*> m_listeners;
};

}

#endif // PROCESSORCONFIGWIDGET_H
