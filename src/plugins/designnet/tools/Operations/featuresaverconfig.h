#ifndef FEATURESAVERCONFIG_H
#define FEATURESAVERCONFIG_H

#include <QWidget>
#include "ui_featuresaverconfig.h"
#include "widgets/processorconfigwidget.h"
#include "designnet/designnet_core/processorconfiglistener.h"

namespace CustomUI{
class PathChooser;
class FileNameValidatingLineEdit;
}
class FeatureSaverConfig;
class FeatureSaver;
class FeatureSaverConfigListener: public DesignNet::ProcessorConfigListener
{
public:
	FeatureSaverConfigListener(FeatureSaverConfig *widget, DesignNet::Processor *processor);
	virtual void onSettingsChanged();
	virtual void onPropertyChanged(DesignNet::Property *prop);
	FeatureSaver*					m_processor;
	FeatureSaverConfig*				m_widget;
};

class FeatureSaverConfig : public DesignNet::ProcessorConfigWidget
{
	Q_OBJECT

public:
	FeatureSaverConfig(DesignNet::Processor* processor, QWidget *parent);
	~FeatureSaverConfig();
	virtual DesignNet::ProcessorConfigWidget* create(DesignNet::Processor * processor, QWidget *parent = 0);
	void init() override;
	QString getPath();
private:
	CustomUI::PathChooser *					m_pathChooser;
	CustomUI::FileNameValidatingLineEdit*	m_nameValidatingLineEdit;
	FeatureSaverConfigListener*				m_listener;
	Ui::FeatureSaverConfig ui;
};

#endif // FEATURESAVERCONFIG_H
