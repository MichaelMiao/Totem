#include "FeatureSaver.h"
#include "data/matrixdata.h"
#include "data/intdata.h"
#include "featuresaverconfig.h"
#include <QFile>
#include <QReadWriteLock>
#include <QWriteLocker>
using namespace DesignNet;

struct FeatureSaverPara
{
	FeatureSaverPara() { iFeatureVectorCount = -1; }
	int iFeatureVectorCount;
	QString fileName;
};

class FeatureSaverPrivate
{
public:
	FeatureSaverPrivate(FeatureSaver* featureSaver)
		: p(featureSaver)
	{
		m_inputPort = new Port(new MatrixData(featureSaver), Port::IN_PORT, "Input Vector");
	}

	Port*			m_inputPort;		//!< feature向量收入
	QFile			m_file;
	FeatureSaverPara m_para;
	QReadWriteLock	m_lock;
	FeatureSaver*	p;
};


FeatureSaver::FeatureSaver(DesignNet::DesignNetSpace *space, QObject* parent)
	: Processor(space, parent),
	d(new FeatureSaverPrivate(this))
{
	addPort(d->m_inputPort);
	setName(tr("FeatureSaver"));
}


FeatureSaver::~FeatureSaver(void)
{
	delete d;
}

DesignNet::Processor* FeatureSaver::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new FeatureSaver(space);
}

QString FeatureSaver::title() const
{
	return "FeatureSaver";
}

QString FeatureSaver::category() const
{
	return "Operations/FeatureSaver";
}

bool FeatureSaver::prepareProcess()
{
	if (d->m_para.fileName.isEmpty())
	{
		emit logout("The file to save feature vectors hasn't been specified.");
		return false;
	}
	else
	{
		d->m_file.setFileName(d->m_para.fileName);
		if (!d->m_file.open(QIODevice::WriteOnly))
		{
			emit logout(QString("Open file %1 faild").arg(d->m_para.fileName));
			return false;
		}
	}
	return true;
}

bool FeatureSaver::process(QFutureInterface<ProcessResult> &future)
{
	QVector<IData*> datas = d->m_inputPort->getInputData();
	for (int i = 0; i < datas.size(); i++)
	{
		MatrixData *pData = qobject_cast<MatrixData*>(datas.at(i));
		if (pData)
		{
			cv::Mat mat = pData->getMatrix();
			mat = mat.reshape(1,1);
			if(-1 == d->m_file.write((char*)mat.data, mat.cols * sizeof(uchar)))
			{
				emit logout(QString("write data to file %1 faild").arg(d->m_para.fileName));
				return false;
			}
		}
	}
	return true;
}

bool FeatureSaver::finishProcess()
{
	if (d->m_file.isOpen())
	{
		d->m_file.close();
		return true;
	}
	return false;
}

void FeatureSaver::showConfig()
{
}

void FeatureSaver::setFilePath( const QString &fileName )
{
	d->m_lock.lockForWrite();
	d->m_para.fileName = fileName;
	d->m_lock.unlock();
}

QString FeatureSaver::getFileName()
{
	QString fileName;
	d->m_lock.lockForRead();
	fileName = d->m_para.fileName;
	d->m_lock.unlock();
	return fileName;
}

bool FeatureSaver::isFileOpen()
{
	bool bOpend = false;
	d->m_lock.lockForRead();
	bOpend = d->m_file.isOpen();
	d->m_lock.unlock();
	return bOpend;
}