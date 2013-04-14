#ifndef IDATAWIDGETFACTORY_H
#define IDATAWIDGETFACTORY_H

#include <QObject>
#include "designnet_core_global.h"
#include "coreplugin/id.h"

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

namespace DesignNet {
class IDataWidget;
class IData;
class DESIGNNET_CORE_EXPORT IDataWidgetFactory : public QObject
{
    Q_OBJECT
public:
    explicit IDataWidgetFactory(QObject *parent = 0);
    virtual ~IDataWidgetFactory(){}
    virtual  IDataWidget *createWidget(IData *property, QGraphicsItem *parent) = 0;
    virtual  Core::Id id() const = 0;
    virtual  QList<Core::Id> supportedTypes() = 0;

};
}

#endif // IDATAWIDGETFACTORY_H
