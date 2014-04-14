#include "ClassifierStatistic.h"

enum PortIndex
{
	PortIndex_In_PredictedLabelMat,
	PortIndex_In_LabelMat,
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_MATRIX,		"Predicted Labels" },
	{ Port::IN_PORT,	DATATYPE_MATRIX,		"Labels" },
};

ClassiferStatistic::ClassiferStatistic(DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/)
{
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
	setName(tr("Classifer Statistic"));
}

ClassiferStatistic::~ClassiferStatistic()
{

}

QString ClassiferStatistic::title() const
{
	return tr("Classifer Statistic");
}

QString ClassiferStatistic::category() const
{
	return tr("Statistic");
}

bool ClassiferStatistic::prepareProcess()
{
	QList<Port*> ls = getPorts(Port::IN_PORT);
	for (int i = 0; i < ls.size(); i++)
	{
		if (!ls.at(i)->isConnected())
		{
			emit logout(tr("Port: %1 is not connected!").arg(ls.at(i)->name()));
			return false;
		}
	}
	return true;
}

bool ClassiferStatistic::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	cv::Mat matPredict = getPortData<cv::Mat>(s_ports[PortIndex_In_PredictedLabelMat]);
	cv::Mat matLabel = getPortData<cv::Mat>(s_ports[PortIndex_In_LabelMat]);
	if (matPredict.rows != matLabel.rows)
	{
		emit logout(tr("The predicted label has different row count with the label"));
		return false;
	}
	int iSum = 0;
	int iSum1 = 0;
	for (int r = 0; r < matPredict.rows; r++)
	{
		emit logout(tr("%1 ---- %2").arg(matLabel.at<int>(r, 0)).arg(matPredict.at<int>(r, 0)));
		for (int c = 0; c < matPredict.cols; c++)
		{
			if (matLabel.at<int>(r, 0) == matPredict.at<int>(r, c))
			{
				iSum++;
				if (c == 0)
					iSum1++;
				break;
			}
		}
	}
	if (matLabel.rows)
	{
		emit logout(tr("Accuracy : Top%1--%2%, Top1--%3%").arg(matPredict.cols).arg(iSum * 100.0 / matLabel.rows).arg(iSum1 * 100.0 / matLabel.rows));
		return true;
	}
	return true;
}
