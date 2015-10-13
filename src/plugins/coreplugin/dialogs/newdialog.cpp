#include "stdafx.h"
#include "newdialog.h"
#include <QAbstractProxyModel>
#include <QButtonGroup>
#include <QPushButton>
#include <QStandardItemModel>
#include "CustomUI/filterlineedit.h"
#include "iwizard.h"
#include "newdialogproxymodel.h"


namespace Core{
const int ICON_SIZE_SMALL	= 24;
const int ICON_SIZE_LIST	= 40;
NewDialog::NewDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_buttonGroup = new QButtonGroup(this);
	m_buttonGroup->addButton(ui.buttonSmallIcon);
	m_buttonGroup->addButton(ui.buttonListIcon);
	ui.buttonSmallIcon->setChecked(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	QPushButton *okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
	okButton->setDefault(true);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
	m_filterLineEdit = new CustomUI::FilterLineEdit(this);
	ui.horizontalLayout->addWidget(m_filterLineEdit);
	QStringList items;
	items << tr("Name Ascending") << tr("Name Descending");
	ui.comboBox->addItems(items);
	connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sortWizard(int)));
	ui.listViewWizard->setFocusPolicy(Qt::StrongFocus);
	ui.listViewWizard->setSelectionBehavior(QAbstractItemView::SelectRows);

	m_model = new WizardModel(this);
	m_categoryProxyModel = new CategoryProxyModel(this);
	m_categoryProxyModel->setSourceModel(m_model);
	ui.treeViewCategory->setModel(m_categoryProxyModel);
	ui.treeViewCategory->expand(m_categoryProxyModel->installedTemplateItem());
	connect(ui.treeViewCategory, SIGNAL(clicked(QModelIndex)), this, SLOT(currentCategoryChanged(QModelIndex)));

	m_itemProxyModel = new ItemProxyModel(m_model, this);
	m_itemFilterProxyModel = new QSortFilterProxyModel(this);
	m_itemFilterProxyModel->setSourceModel(m_itemProxyModel);
	connect(m_filterLineEdit, SIGNAL(filterChanged(QString)), 
		m_itemFilterProxyModel, SLOT(setFilterRegExp(QString)));
}

NewDialog::~NewDialog()
{

}

void NewDialog::setWizards( QList<IWizard*> wizards )
{
	m_model->setWizards(wizards);
	ui.treeViewCategory->expand(m_categoryProxyModel->installedTemplateItem());
}

void NewDialog::currentCategoryChanged( const QModelIndex &index)
{
	ui.listViewWizard->setModel(m_itemFilterProxyModel);
	QModelIndex sourceIndex = m_categoryProxyModel->mapToSource(index);
	m_itemProxyModel->setRootIndex(sourceIndex);
	// Focus the first item by default
	ui.listViewWizard->setCurrentIndex(ui.listViewWizard->rootIndex().child(0,0));

	connect(ui.listViewWizard->selectionModel(),
		SIGNAL(currentChanged(QModelIndex,QModelIndex)),
		this, SLOT(currentItemChanged(QModelIndex)));
}

void NewDialog::currentItemChanged( const QModelIndex &index )
{
	ui.listViewWizard->setCurrentIndex(index);
	QModelIndex sourceIndex = m_itemFilterProxyModel->mapToSource(index);
	const IWizard *wizard = m_itemProxyModel->wizard(sourceIndex);
	if (wizard) 
	{
		QString desciption = wizard->description();
		if (!Qt::mightBeRichText(desciption))
			desciption.replace(QLatin1Char('\n'), QLatin1String("<br>"));
		desciption += QLatin1String("<br><br><b>");

		ui.descriptionText->setHtml(desciption);

		if (!wizard->descriptionImage().isEmpty()) {
			ui.descriptionImageLabel->setVisible(true);
			ui.descriptionImageLabel->setPixmap(wizard->descriptionImage());
		} else {
			ui.descriptionImageLabel->setVisible(false);
		}

	} else {
		ui.descriptionText->setText(QString());
	}
	updateOkButton();
}

void NewDialog::changedWizardIcon()
{
	QAbstractButton *button = m_buttonGroup->checkedButton();
	if (button)
	{
		;
	}
}

void NewDialog::sortWizard( int index )
{
	if (index == 0)
	{
		m_itemFilterProxyModel->sort(0);
	}
	else
	{
		m_itemFilterProxyModel->sort(0, Qt::DescendingOrder);
	}
}

Core::IWizard * NewDialog::showDialog()
{
	static QString lastCategory;//!< 最后一次的种类
	QModelIndex idx;

	if (!lastCategory.isEmpty())
	{
		idx = m_model->categoryIndex(lastCategory);
	}
	if (!idx.isValid())
		idx = m_categoryProxyModel->index(0,0, m_categoryProxyModel->index(0,0, QModelIndex()));
	
	ui.treeViewCategory->setCurrentIndex(idx);

	// We need to set ensure that the category has default focus
	ui.treeViewCategory->setFocus(Qt::NoFocusReason);

	// Ensure that item description is visible on first show
	currentCategoryChanged(idx);
	currentItemChanged(ui.listViewWizard->rootIndex().child(0,0));

	updateOkButton();

	const int retVal = exec();
	lastCategory = ui.treeViewCategory->currentIndex().data(Qt::UserRole).toString();
	if (retVal != Accepted)
	{
		return 0;
	}

	return currentWizard();
}

void NewDialog::updateOkButton()
{
	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(currentWizard());
}

IWizard* NewDialog::currentWizard() const
{
	QModelIndex index = m_itemFilterProxyModel->mapToSource(ui.listViewWizard->currentIndex());
	if (index.isValid())
	{
		return (IWizard*)index.data(Qt::UserRole + 1).value<void *>();
	}
	return 0;
}

void NewDialog::accepted()
{
	done(QDialog::Accepted);
}

void NewDialog::rejected()
{
	done(QDialog::Rejected);
}

}
