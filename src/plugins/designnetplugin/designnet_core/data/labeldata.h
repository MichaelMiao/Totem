#ifndef LABELDATA_H
#define LABELDATA_H

#include "idata.h"
#include "opencv2/core/core.hpp"
#include <QHash>
namespace DesignNet {
/*!
 *	\brief 载入分类标签
 *
 * 
 */
class DESIGNNET_CORE_EXPORT LabelData : public IData
{
	Q_OBJECT

public:
	LabelData(QObject *parent);
	~LabelData();
	virtual Core::Id id();
	QHash<int, int> &labels();//!< 返回标签对
private:
	QHash<int, int> m_labels;//!< 使用Hash来存放标签， <对象xi--类别y>
};
}//!< namespace DesignNet

#endif // LABELDATA_H
