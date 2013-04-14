#include "test.h"

test::test(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);
}

test::~test()
{

}
