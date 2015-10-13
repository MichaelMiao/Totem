#ifndef IDATATYPE_H
#define IDATATYPE_H

#include "coreplugin/id.h"
#include "../designnet_core_global.h"
#include <QObject>
#include <QImage>
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
	virtual bool copy(IData* data);
	virtual bool isValid() const = 0;
	virtual QImage image() = 0;
	int index() const{return m_index;}
	void setIndex(const int& i){m_index = i;}

	void setPermanent(const bool &p);
	bool isPermanent() const;
signals:
    void dataChanged();
protected:
	QImage	m_image;//!< ����ͼƬ
	int		m_index;//!< ��������
	bool	m_bPermanent;//!< �Ƿ���permanent����
};
Q_DECLARE_METATYPE(IData*)
}

#endif // IDATATYPE_H
