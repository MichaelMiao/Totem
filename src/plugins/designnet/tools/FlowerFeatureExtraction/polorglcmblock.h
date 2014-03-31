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
 *	\brief Gray-level co-occurrence matrix �Ҷȹ�������
 *  
 *  �ο�����2011[��������ͼ��Ļ�������ʶ�����о�]
 *	����ʹ�ü�����ĻҶȹ�������
 */
class PolorGLCMBlock : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(PolorGLCMBlock)

	PolorGLCMBlock(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~PolorGLCMBlock();
	virtual QString title() const;
	virtual QString category() const;//!< ����

	/*!
	 *	\brief ����GLCM��6���������ο�����2011 
	 *	
	 *  feature[0]: ��Ե������ĸ���/����Ե�����㵽���ĵľ���֮�ͣ�,��Ӧ����2011�е�F[6]
	 *	feature[1]: glcm ����
	 *  feature[2]: glcm �Աȶ�
	 *	feature[3]: glcm ͬ����
	 *	feature[4]: ����sobel����֮��� glcm ���� 
	 *  feature[5]: ����sobel����֮��� glcm �Աȶ�
	 *  feature[6]: ����sobel����֮��� glcm ͬ����
	 */
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);

protected:
	
	virtual void propertyChanged(DesignNet::Property *prop);

	void centroid();	//!<	������������
	void normalize(cv::Mat &glcm);	//!<	��һ��glcm
	float energe(const cv::Mat &glcm);//!< ����Ҷȹ������������
	float contrast(const cv::Mat &glcm);//!<  ����Աȶ�
	float homogeneity(const cv::Mat &glcm);//!< ����ͬ���ԣ�GLCM��Ԫ�ضԽ��߷ֲ��Ŀؼ����ܶȵĶ���

private:

	DesignNet::DoubleRangeProperty* m_doubleRangeProperty;
	DesignNet::DoubleRangeProperty* m_binCountProperty;//!< �Ҷȼ���Ĭ��Ϊ8
	cv::Mat m_glcm;					//!< �Ҷȹ�������
	cv::Mat m_glcm_sobel;			//!< sobel�˲���ĻҶȹ�������
	cv::Point2d m_centroid;			//!< ͼ����������
};
}
#endif // POLORGLCMBLOCK_H
