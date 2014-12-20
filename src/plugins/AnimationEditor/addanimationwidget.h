#pragma once
#ifndef ADDANIMATION_H
#define ADDANIMATION_H

#include <QDialog>
#include "ui_addanimation.h"

class AddAnimationWidget : public QDialog
{
	Q_OBJECT

public:
	AddAnimationWidget(QWidget *parent = 0);
	~AddAnimationWidget();
	QString getName();
	int		getDuration();
	int		getTimeStart();
	void	setStartTime(const int iTimeStart);

public slots:
	
	void	onOK();
	void	onCancel();

private:
	Ui::AddAnimation ui;
};

#endif // ADDANIMATION_H
