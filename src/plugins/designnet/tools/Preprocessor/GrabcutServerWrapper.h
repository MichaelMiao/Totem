#pragma once

#include <QTcpServer>
#include <QObject>
#include "GrabCutThread.h"

struct GrabCutThreadData 
{
	QTcpSocket* m_pSocket;
	QThread*	m_pThread;
	int			m_iId;
};

struct RecognizeTaskData
{
	int iId;
	QTcpSocket* m_pSocket;
};

namespace DesignNet
{
	class Processor;
}

class GrabcutServerWrapper : public QObject
{
	Q_OBJECT
public:

	GrabcutServerWrapper(DesignNet::Processor* processor);
	~GrabcutServerWrapper();

	bool startServer(QString strIP);

	void sendResult(cv::Mat res);

public slots:

	void onNewComing();

	void onReadMessage();

	void onProcessFinished();

	void onObjectAdded(QObject *obj);


protected:
	
	void doFragment(int iId, QRect rc, QString strSrcFileName, QString strMaskFileName, QTcpSocket* pSocket);
	void doRecognization(int iId, QTcpSocket* pSocket);

private:

	QTcpServer				m_server;
	QVector<QTcpSocket *>	m_vecSocket;
	QList<RecognizeTaskData>	m_vecTask;
	DesignNet::Processor*	m_processor;

	QString					m_strIP;
};