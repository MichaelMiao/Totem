#include "availabledatawidget.h"
#include "designnetbase/processor.h"


using namespace DesignNet;

enum AvailableDataWidgetColumn{
	DataWidgetColumn_Label,	//!< ��ǩ
	DataWidgetColumn_Processor,//!< ��������˭�ṩ
	DataWidgetColumn_End,
};
AvailableDataWidget::AvailableDataWidget(QWidget *parent)
	: QTableWidget(0, DataWidgetColumn_End, parent)
{
	QStringList ls;
	ls << tr("Label") << tr("PID");
	setHorizontalHeaderLabels(ls);
}

AvailableDataWidget::~AvailableDataWidget()
{

}

void DesignNet::AvailableDataWidget::setAvailiableData( Processor* processor )
{
	while (rowCount() > 0)
		removeRow(0);
	const QList<ProcessData> &datas = processor->getOutputData();
	foreach (ProcessData p, datas)
	{
		insertRow(0);
		setItem(0, 0, new QTableWidgetItem(p.strLabel));
		setItem(0, 1, new QTableWidgetItem(QString("%1").arg(p.processorID)));
	}
	resizeRowsToContents();
}
