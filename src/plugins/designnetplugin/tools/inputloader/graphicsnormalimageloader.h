#ifndef GRAPHICSNORMALIMAGELOADER_H
#define GRAPHICSNORMALIMAGELOADER_H

#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"

namespace DesignNet{
class Processor;
class PathDialogProperty;
class Property;
}
namespace InputLoader {
/*!
 * \brief The GraphicsNormalImageLoader class
 *
 * 一般图片载入，支持类型为OpenCV所支持的所有类型
 */
class GraphicsNormalImageLoader : public DesignNet::ProcessorGraphicsBlock
{
    Q_OBJECT
public:
    GraphicsNormalImageLoader(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
    virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
    virtual QString title() const;
    virtual QString category() const;//!< 种类
    virtual bool process();     //!< 处理函数

protected:
    virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	virtual void propertyChanged(DesignNet::Property *prop);
    DesignNet::Port m_outPort;
    DesignNet::ImageData* m_imageData;     //!< 图片数据
    DesignNet::PathDialogProperty *m_imageFile;
};
}

#endif // GRAPHICSNORMALIMAGELOADER_H
