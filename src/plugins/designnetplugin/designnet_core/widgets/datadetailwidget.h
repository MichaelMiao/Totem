#ifndef DATADETAILWIDGET_H
#define DATADETAILWIDGET_H

#include <QDialog>
#include "ui_datadetailwidget.h"

class DataDetailWidget : public QWidget
{
	Q_OBJECT

public:
	DataDetailWidget(QWidget *parent = 0, Qt::WindowFlags f = Qt::Tool);
	~DataDetailWidget();

private:
	Ui::DataDetailWidget ui;
};

#endif // DATADETAILWIDGET_H
