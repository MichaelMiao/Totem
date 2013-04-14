#ifndef PROPERTYWIDGETFACTORY_H
#define PROPERTYWIDGETFACTORY_H

#include <QObject>
#include "designnet_core_global.h"
#include "ipropertywidget.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT IPropertyWidgetFactory : public QObject
{
    Q_OBJECT
public:
    explicit IPropertyWidgetFactory(QObject *parent = 0);
    virtual IPropertyWidget *createWidget(Property *property, QWidget* parent) = 0;
    virtual Core::Id id() const = 0;
    virtual QList<Core::Id> supportedTypes() = 0;
signals:

public slots:

};
}

#endif // PROPERTYWIDGETFACTORY_H
