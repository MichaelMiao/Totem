#ifndef NORMALPROPERTYWIDGETFACTORY_H
#define NORMALPROPERTYWIDGETFACTORY_H

#include "ipropertywidgetfactory.h"

namespace DesignNet {
class IPropertyWidget;
class NormalPropertyWidgetFactory : public IPropertyWidgetFactory
{
    Q_OBJECT
public:
    explicit NormalPropertyWidgetFactory(QObject *parent = 0);
    IPropertyWidget *createWidget(Property *property, QWidget* parent);
    virtual Core::Id id() const;
    QList<Core::Id> supportedTypes();
signals:

public slots:

};
}

#endif // NORMALPROPERTYWIDGETFACTORY_H
