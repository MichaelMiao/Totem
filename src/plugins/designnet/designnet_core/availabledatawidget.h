#ifndef AVAILABLEDATAWIDGET_H
#define AVAILABLEDATAWIDGET_H

#include <QTableWidget>
#include "designnetbase/processor.h"
namespace DesignNet
{
class AvailableDataWidget : public QTableWidget
{
	Q_OBJECT

public:
	AvailableDataWidget(QWidget *parent);
	~AvailableDataWidget();

public slots:
	void setAvailiableData(Processor* processor);

private:
	
};

}

#endif // AVAILABLEDATAWIDGET_H
