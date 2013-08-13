#ifndef PROCESSORCONFIGMANAGER_H
#define PROCESSORCONFIGMANAGER_H

#include <QObject>
#include "../designnet_core_global.h"
QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace DesignNet{
class ProcessorConfigManagerPrivate;
class ProcessorConfigWidget;
class Processor;
class DESIGNNET_CORE_EXPORT ProcessorConfigManager : public QObject
{
	Q_OBJECT

public:
	ProcessorConfigManager(QObject *parent = 0);
	~ProcessorConfigManager();

	void registerConfigWidget(const QString &processorId, ProcessorConfigWidget *widget);
	ProcessorConfigWidget* createConfigWidget(Processor* processor, QWidget *parent);
	static ProcessorConfigManager* instance();
	static void Release();
private:
	static ProcessorConfigManager *m_instance;
	ProcessorConfigManagerPrivate *d;
};

}

#endif // PROCESSORCONFIGMANAGER_H
