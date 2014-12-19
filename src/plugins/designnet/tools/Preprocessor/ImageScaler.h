#pragma once

#pragma once
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "opencv2/core/core.hpp"


class ImageScaler : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(ImageScaler)
	ImageScaler(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	virtual ~ImageScaler();
	virtual QString title() const;
	virtual QString category() const;	//!< ����
	virtual bool	process(QFutureInterface<DesignNet::ProcessResult> &future);				//!< ������
	virtual void	serialize(Utils::XmlSerializer& s) const;
	virtual void	deserialize(Utils::XmlDeserializer& s) ;

private:

};