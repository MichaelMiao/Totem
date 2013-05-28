#ifndef INTDATA_H
#define INTDATA_H

#include <QObject>
#include "idata.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT IntData : public IData
{
	Q_OBJECT

public:
	IntData(const int &defaultValue = 0, QObject *parent = 0);
	~IntData();
	void setValue(const int &value);
	int value() const;

	virtual Core::Id id() ;
	virtual IData* clone(QObject *parent = 0);
	virtual bool copy(IData* data);
	virtual bool isValid() const;
	virtual QImage image();
private:
	int m_value;
};

}

#endif // INTDATA_H
