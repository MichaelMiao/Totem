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
	const QList<Port*> ports = processor->getPorts(Port::IN_PORT);
	foreach (Port* p, ports)
	{
		insertRow(0);
		setItem(0, 0, new QTableWidgetItem(p->name()));
		setItem(0, 1, new QTableWidgetItem(QString("%1").arg(p->data()->processorID)));
	}
	resizeRowsToContents();
}
