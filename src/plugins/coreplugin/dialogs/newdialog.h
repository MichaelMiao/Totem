#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include "ui_newdialog.h"
#include <QList>
#include <QIcon>
#include <QSortFilterProxyModel>
QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QAbstractProxyModel;
class QStandardItem;
class QButtonGroup;
QT_END_NAMESPACE

namespace CustomUI{
class FilterLineEdit;
}
namespace Core{
class IWizard;
class WizardModel;
class ItemProxyModel;
class CategoryProxyModel;
class NewDialog : public QDialog
{
	Q_OBJECT

public:
	NewDialog(QWidget *parent = 0);
	~NewDialog();
	void setWizards(QList<IWizard*> wizards);
	Core::IWizard * showDialog();
	void updateOkButton();
	IWizard* currentWizard() const;
public slots:
	void currentCategoryChanged(const QModelIndex &index);
	void currentItemChanged(const QModelIndex &index);

	void changedWizardIcon();
	void sortWizard(int index);
	void accepted();
	void rejected();
private:
	Ui::NewDialog ui;
	WizardModel *m_model;
	CategoryProxyModel *m_categoryProxyModel;
	ItemProxyModel *m_itemProxyModel;
	QSortFilterProxyModel* m_itemFilterProxyModel;
	CustomUI::FilterLineEdit*	m_filterLineEdit;
	QButtonGroup *m_buttonGroup;
};

}

#endif // NEWDIALOG_H
