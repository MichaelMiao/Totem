#ifndef PROPERTYLISTWIDGET_H
#define PROPERTYLISTWIDGET_H

#include "CustomUI/collapsiblebutton.h"
#include "designnet_core_global.h"

#include <QScrollArea>
#include <QList>

namespace DesignNet{
class Processor;
class PropertyListWidgetPrivate;
class DESIGNNET_CORE_EXPORT PropertyListWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit PropertyListWidget(QWidget *parent = 0);

    virtual QSize sizeHint() const;
signals:

public slots:
    void processorAdded(Processor* processor);
    void processorSelected(QList<Processor*> selectedProcessors);
protected:
    PropertyListWidgetPrivate* d;
};
}

#endif // PROPERTYLISTWIDGET_H
