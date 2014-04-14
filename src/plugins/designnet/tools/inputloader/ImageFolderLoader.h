#pragma once
#include "designnet\designnet_core\designnetbase\processor.h"
#include "designnet\designnet_core\data\imagedata.h"
#include <QReadWriteLock>


namespace DesignNet{
class PathDialogProperty;
}
namespace InputLoader{

class ImageFolderLoader : public DesignNet::Processor
{
	Q_OBJECT
public:

	DECLEAR_PROCESSOR(ImageFolderLoader)

	ImageFolderLoader(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~ImageFolderLoader(void);

	virtual QString title() const;
	virtual QString category() const;//!< ����
	
	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������
	virtual bool finishProcess();

protected:

	bool waitForAccept();	//!< �ȴ�������processor������
	void onChildProcessorFinish(Processor* p) override;
	QList<Processor*> m_listWaitProcessors;	//!< ��ǰ���ڵȴ��Ĵ������б�
	
	QStringList					m_filePaths;
	int							m_iCurIndex;
	mutable QReadWriteLock	m_lock;
	mutable QMutex	m_waitLock;
};

}

