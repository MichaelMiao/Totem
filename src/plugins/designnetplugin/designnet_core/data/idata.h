#ifndef IDATATYPE_H
#define IDATATYPE_H

#include "coreplugin/id.h"
#include "../designnet_core_global.h"
#include <QObject>
namespace DesignNet{
class Processor;
class DESIGNNET_CORE_EXPORT IData : public QObject
{
    Q_OBJECT
    friend class Processor;
public:
    explicit IData(QObject *parent = 0);
    virtual ~IData(){}
    virtual Core::Id id() = 0;
signals:
    void dataChanged();
};
}

#endif // IDATATYPE_H
