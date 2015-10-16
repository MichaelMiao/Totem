#ifndef BOOLPROPERTYWIDGET_H
#define BOOLPROPERTYWIDGET_H

#include "ipropertywidget.h"



QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

namespace DesignNet{

class BoolProperty;
class BoolPropertyWidget : public IPropertyWidget
{
	Q_OBJECT

public:
	
	BoolPropertyWidget(BoolProperty* prop, QWidget *parent = 0);
	~BoolPropertyWidget();

public slots:
	
	void onValueChanged(int state);

private:
	
	QCheckBox* m_checkBox;
};

}

#endif // BOOLPROPERTYWIDGET_H
