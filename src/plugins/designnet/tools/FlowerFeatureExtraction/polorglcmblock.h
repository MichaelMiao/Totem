#ifndef POLORGLCMBLOCK_H
#define POLORGLCMBLOCK_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
class MatrixData;
class DesignNetSpace;
class DoubleRangeProperty;
}

namespace FlowerFeatureExtraction{

/*!
 *	\brief Gray-level co-occurrence matrix 灰度共生矩阵
 *  
 *  参考裴勇2011[基于数字图像的花卉种类识别技术研究]
 *	生成使用极坐标的灰度共生矩阵
 */
class PolorGLCMBlock : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(PolorGLCMBlock)

	PolorGLCMBlock(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~PolorGLCMBlock();
	virtual QString title() const;
	virtual QString category() const;//!< 种类

	/*!
	 *	\brief 生成GLCM的6个特征，参考裴勇2011 
	 *	
	 *  feature[0]: 边缘轮廓点的个数/（边缘轮廓点到中心的距离之和）,对应裴勇2011中的F[6]
	 *	feature[1]: glcm 能量
	 *  feature[2]: glcm 对比度
	 *	feature[3]: glcm 同质性
	 *	feature[4]: 经过sobel算子之后的 glcm 能量 
	 *  feature[5]: 经过sobel算子之后的 glcm 对比度
	 *  feature[6]: 经过sobel算子之后的 glcm 同质性
	 */
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);

protected:
	
	virtual void propertyChanged(DesignNet::Property *prop);

	void centroid();	//!<	计算中心坐标
	void normalize(cv::Mat &glcm);	//!<	归一化glcm
	float energe(const cv::Mat &glcm);//!< 计算灰度共生矩阵的能量
	float contrast(const cv::Mat &glcm);//!<  计算对比度
	float homogeneity(const cv::Mat &glcm);//!< 计算同质性，GLCM中元素对角线分布的控件紧密度的度量

private:

	DesignNet::DoubleRangeProperty* m_doubleRangeProperty;
	DesignNet::DoubleRangeProperty* m_binCountProperty;//!< 灰度级，默认为8
	cv::Mat m_glcm;					//!< 灰度共生矩阵
	cv::Mat m_glcm_sobel;			//!< sobel滤波后的灰度共生矩阵
	cv::Point2d m_centroid;			//!< 图像中心坐标
};
}
#endif // POLORGLCMBLOCK_H
