#ifndef CUSTOMDATA_H
#define CUSTOMDATA_H

#include <QObject>
#include "idata.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT CustomData : public IData
{
	Q_OBJECT

public:
	CustomData(void* defaultValue = 0, QObject *parent = 0);
	~CustomData();
	void setData(void *v, bool bAutoDelete = true) { m_value = v; }
	void* data() const;

	void setId(Core::Id id) { m_id = id; }
	
	virtual Core::Id id() { return m_id; }

	virtual IData* clone(QObject *parent = 0);
	virtual bool copy(IData* data);
	virtual bool isValid() const;
	virtual QImage image();

private:

	void*	m_value;

	QImage m_image;
	Core::Id m_id;
};
}

#endif // CUSTOMDATA_H
