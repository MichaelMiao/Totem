#include "stdafx.h"
#include "addanimationwidget.h"

AddAnimationWidget::AddAnimationWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(onOK()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(onCancel()));
}

AddAnimationWidget::~AddAnimationWidget()
{

}

QString AddAnimationWidget::getName()
{
	return ui.editName->text();
}

int AddAnimationWidget::getDuration()
{
	return ui.spinBoxDuration->value();
}

int AddAnimationWidget::getTimeStart()
{
	return ui.spinBoxFrom->value();
}

void AddAnimationWidget::onOK()
{
	if (ui.editName->text().length() > 0)
	{
		done(QDialog::Accepted);
	}
	else
	{
		QMessageBox::warning(this, "Error", "You can't use an empty name", QMessageBox::Ok);
	}
}

void AddAnimationWidget::onCancel()
{
	done(QDialog::Rejected);
}

void AddAnimationWidget::setStartTime(const int iTimeStart)
{
	ui.spinBoxFrom->setValue(qMax(iTimeStart, 0));
}
