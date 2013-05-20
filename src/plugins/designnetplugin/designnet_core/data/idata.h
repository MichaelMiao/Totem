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
	virtual IData* clone(QObject *parent = 0) = 0;
	virtual bool copy(IData* data) = 0;
	virtual bool isValid() const = 0;
	virtual QImage image() = 0;
signals:
    void dataChanged();
protected:
	QImage m_image;//!< ÀàÐÍÍ¼Æ¬
};
}

#endif // IDATATYPE_H
