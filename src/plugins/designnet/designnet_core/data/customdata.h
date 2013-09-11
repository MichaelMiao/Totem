#ifndef CUSTOMDATA_H
#define CUSTOMDATA_H

#include <QObject>
#include "idata.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT CustomData : public IData
{
	Q_OBJECT

public:
	CustomData(void* defaultValue = 0, const QString &strLabel = tr("CustomData"), QObject *parent = 0);
	~CustomData();
	void setData(void *v, bool bAutoDelete = true) { m_value = v; }
	void* data(const QString &strLabel) const;

	virtual Core::Id id() ;
	virtual IData* clone(QObject *parent = 0);
	virtual bool copy(IData* data);
	virtual bool isValid() const;
	virtual QImage image();

private:

	void*	m_value;
	QString m_label;

	QImage m_image;
};
}

#endif // CUSTOMDATA_H
