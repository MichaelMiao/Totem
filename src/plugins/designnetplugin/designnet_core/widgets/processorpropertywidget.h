#ifndef PROCESSORPROPERTYWIDGET_H
#define PROCESSORPROPERTYWIDGET_H

#include "CustomUI/collapsiblebutton.h"

namespace DesignNet{
class Processor;

class ProcessorPropertyWidget : public CustomUI::CollapsibleButton
{
    Q_OBJECT
public:
    explicit ProcessorPropertyWidget(Processor *processor, QWidget *parent = 0);
    virtual ~ProcessorPropertyWidget();
    void initWidgets();//!< 创建窗口
signals:

public slots:
protected:
    Processor* m_processor;
};
}

#endif // PROCESSORPROPERTYWIDGET_H
