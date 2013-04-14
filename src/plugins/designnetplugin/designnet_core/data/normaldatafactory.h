#ifndef NORMALDATAFACTORY_H
#define NORMALDATAFACTORY_H

#include "idatawidgetfactory.h"
QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE
namespace DesignNet{
/*!
 * \brief NormalDataFactory 常见数据类型的工厂
 */
class NormalDataFactory : public IDataWidgetFactory
{
public:
    NormalDataFactory(QObject *parent = 0);
    virtual  IDataWidget *createWidget(IData *property, QGraphicsItem* parent);

    virtual  Core::Id id() const;
    virtual  QList<Core::Id> supportedTypes();//!< 目前支持IMAGE
};
}

#endif // NORMALDATAFACTORY_H
