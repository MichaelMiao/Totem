#ifndef GRAPHICSNORMALIMAGELOADER_H
#define GRAPHICSNORMALIMAGELOADER_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"

namespace DesignNet{
class Processor;
class Property;
}
namespace InputLoader {
/*!
 * \brief The GraphicsNormalImageLoader class
 *
 * 一般图片载入，支持类型为OpenCV所支持的所有类型
 */
class GraphicsNormalImageLoader : public DesignNet::Processor
{
    Q_OBJECT

public:

    GraphicsNormalImageLoader(DesignNet::DesignNetSpace *space, QObject* parent = 0);
    Processor* create(DesignNet::DesignNetSpace *space = 0) const;
    QString title() const;
    QString category() const;//!< 种类
    bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
	
	void setPath(const QString &p);
	QString path() const;

protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	QString					m_filePath;
	mutable QReadWriteLock	m_lock;
};
}

#endif // GRAPHICSNORMALIMAGELOADER_H
