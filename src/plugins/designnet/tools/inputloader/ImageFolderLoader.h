#pragma once
#include "designnet\designnet_core\designnetbase\processor.h"
#include "designnet\designnet_core\designnetbase\port.h"
#include <QReadWriteLock>
namespace DesignNet{
class PathDialogProperty;
}
namespace InputLoader{

class ImageFolderLoader :
	public DesignNet::Processor
{
	Q_OBJECT
public:
	ImageFolderLoader(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~ImageFolderLoader(void);
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	
	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
	virtual bool finishProcess();

	void setPath(const QString &p);
	QString path() const;
signals:
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	DesignNet::Port m_outPort;	//!< 输出Image数据
	DesignNet::Port m_outImageCountPort;  //!< 输出整个Image个数
	QString			m_folderPath;
	QStringList		m_filePaths;
	int				m_iCurIndex;
	mutable QReadWriteLock	m_lock;
};

}

