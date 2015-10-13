#include "stdafx.h"
#include "newdialogproxymodel.h"
#include <QRegExpValidator>
#include "../constants.h"
#include "../iwizard.h"
#include "Utils/totemassert.h"


namespace Core{
CategoryProxyModel::CategoryProxyModel(QObject *parent)
	: QAbstractProxyModel(parent)
{

}

CategoryProxyModel::~CategoryProxyModel()
{

}

QModelIndex CategoryProxyModel::mapFromSource( const QModelIndex & sourceIndex ) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();
	return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());

}

QModelIndex CategoryProxyModel::mapToSource( const QModelIndex & proxyIndex ) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();
	return static_cast<CategoryProxyModel*>(sourceModel())->createIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
}

int CategoryProxyModel::rowCount( const QModelIndex &index ) const
{
	QModelIndex source_index = mapToSource(index);
	int count = sourceModel()->rowCount(source_index);
	int rows = 0;
	for (int i = 0; i < count; ++i)
	{
		if (sourceModel()->rowCount(sourceModel()->index(i, 0, source_index)) != 0)
		{
			++rows;
		}
	}
	return rows;
}

int CategoryProxyModel::columnCount( const QModelIndex &index ) const
{
	return sourceModel()->columnCount(mapToSource(index));
}

QModelIndex CategoryProxyModel::index( int row, int column, const QModelIndex &parent ) const
{
	QModelIndex ourModelIndex = sourceModel()->index(row, column, mapToSource(parent));
	if (sourceModel()->rowCount(ourModelIndex) == 0)
	{
		return QModelIndex();
	}
	return createIndex(row, column, ourModelIndex.internalPointer());
}

QModelIndex CategoryProxyModel::parent( const QModelIndex &index ) const
{
	return mapFromSource(mapToSource(index).parent());
}

QModelIndex CategoryProxyModel::installedTemplateItem() const
{
	WizardModel *wizardModel = static_cast<WizardModel*>(sourceModel());
	if(wizardModel)
	{
		return mapFromSource(wizardModel->installedTemplateItem());
	}
	return QModelIndex();
}


ItemProxyModel::ItemProxyModel(WizardModel *sourceModel, QObject *parent )
	: QAbstractListModel(parent),
	m_model(sourceModel)
{

}

ItemProxyModel::~ItemProxyModel()
{

}

int ItemProxyModel::rowCount( const QModelIndex &parent ) const
{
	if (parent.isValid())
	{
		return 0;
	}
	return m_wizards.size();
}

int ItemProxyModel::columnCount( const QModelIndex &index ) const
{
	return 1;
}

QVariant ItemProxyModel::data( const QModelIndex & index, int role) const
{
	if (index.isValid())
	{
		if(role == Qt::DisplayRole)
		{
			QString strTemp = m_wizards.at(index.row())->category();
			return strTemp.split('/').back();
		}
		else if(role == Qt::DecorationRole)
		{
			return m_wizards.at(index.row())->icon();
		}
		else if (role == Qt::UserRole + 1)
		{
			return qVariantFromValue((void*)m_wizards.at(index.row()));
		}
	}
	return QVariant();
}

void ItemProxyModel::setRootIndex( const QModelIndex &root )
{
	m_wizards = m_model->wizards(root);
	beginResetModel();
	endResetModel();
}

IWizard * ItemProxyModel::wizard(const QModelIndex &index) const
{
	if (index.isValid())
	{
		return (IWizard*)index.data(Qt::UserRole + 1).value<void*>();
	}
	return 0;
}

WizardModel::WizardModel( QObject *parent /*= 0*/ )
	: QStandardItemModel(parent)
{
	m_installedTemplateIndex = QModelIndex();
}

WizardModel::~WizardModel()
{

}

void WizardModel::setWizards( QList<IWizard*> wizards )
{
	clear();
	m_installedTemplateIndex = QModelIndex();
	m_wizards = wizards;
	QStandardItem *parentItem = invisibleRootItem();


	QStandardItem *installedTemplates = new QStandardItem(tr("Installed Templates"));
	installedTemplates->setData(IWizard::FileWizard, Qt::UserRole);

	parentItem->appendRow(installedTemplates);
	m_installedTemplateIndex = installedTemplates->index();
	if (m_dummyIcon.isNull())
		m_dummyIcon = QIcon(QLatin1String(Core::Constants::ICON_NEW_FILE));

	foreach (IWizard *wizard, wizards) 
	{
		QStandardItem *kindItem;
		switch (wizard->kind()) {
		case IWizard::FileWizard:
		default:
			kindItem = installedTemplates;
			break;
		}
		addItem(kindItem, wizard);
	}
}

void WizardModel::addItem( QStandardItem *topLevelCategoryItem, IWizard *wizard )
{
	const QString categoryName = wizard->category();
	TOTEM_ASSERT(!categoryName.isEmpty(), return);

	QStringList categroyHierarchy = categoryName.split('/');
	QStandardItem *parentItem = topLevelCategoryItem;
	foreach(QString strTitle, categroyHierarchy)
	{
		QStandardItem *categoryItem = 0;
		for (int i = 0; i < topLevelCategoryItem->rowCount(); i++)
		{
			if (topLevelCategoryItem->child(i, 0)->data(Qt::UserRole).toString() == strTitle)
			{
				categoryItem = topLevelCategoryItem->child(i, 0);
				break;
			}
		}
		if (!categoryItem)
		{
			categoryItem = new QStandardItem();
			parentItem->appendRow(categoryItem);
			categoryItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			categoryItem->setText(QLatin1String("  ") + wizard->displayCategory());
			categoryItem->setData(wizard->category(), Qt::UserRole);
		}
		parentItem = categoryItem;
	}
	QStandardItem *wizardItem = new QStandardItem(wizard->displayName());
	QIcon wizardIcon;

	// spacing hack. Add proper icons instead
	if (wizard->icon().isNull())
		wizardIcon = m_dummyIcon;
	else
		wizardIcon = wizard->icon();
	wizardItem->setIcon(wizardIcon);
	wizardItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	wizardItem->setData(QVariant::fromValue((void*)wizard), Qt::UserRole + 1);
	parentItem->appendRow(wizardItem);
}
static inline QString fixCategory(const QString &category)
{
	QString ret = category;
	if(category.length() == 0)
		return QString();
	if (ret.at(ret.length() - 1) != '/')
		ret += '/';
	return ret;
}
QList<IWizard*> WizardModel::wizards( const QString &category )
{
	QList<IWizard*> wizards;
	QString fixedCategory = fixCategory(category);
	QStringList categroyHierarchy = fixedCategory.split('/');
	foreach (IWizard *wizard, m_wizards) 
	{
		QString wizardCategory = fixCategory(wizard->category());
		if (wizardCategory.left(category.length()) == fixedCategory)
		{
			wizards.append(wizard);
		}
	}
	return wizards;
}

QList<IWizard*> WizardModel::wizards( const QModelIndex &parent )
{
	QList<IWizard*> wizards;
	getWizards(parent, wizards);
	return wizards;
}

void WizardModel::getWizards( const QModelIndex& parent, QList<IWizard*> &wizards )
{
	int rows = 0;
	if (parent.isValid())
	{
		rows = rowCount(parent);
	}
	
	for(int i = 0; i < rows; ++i)
	{
		QModelIndex index = parent.child(i, 0);
		if(rowCount(index) == 0)
		{
			IWizard *wizard = (IWizard*)(index.data(Qt::UserRole + 1).value<void*>());
			if (wizard)
			{
				wizards.append(wizard);
			}
		}
		else
		{
			getWizards(index, wizards);
		}
	}
}

QModelIndex WizardModel::installedTemplateItem() const
{
	return m_installedTemplateIndex;
}

QModelIndex WizardModel::categoryIndex( const QString &category )
{
	QString fixedCategory = fixCategory(category);
	QStringList categroyHierarchy = fixedCategory.split('/');
	QModelIndex parentIndex = m_installedTemplateIndex;
	foreach(QString subTitle, categroyHierarchy)
	{
		int iCount = rowCount(parentIndex);
		bool bExist = false;
		for (int i = 0; i < iCount; ++i)
		{
			if (parentIndex.child(i, 0).data().toString() == subTitle)
			{
				parentIndex = parentIndex.child(i, 0);
				bExist = true;
				break;
			}
		}
		if (!bExist)
		{
			return QModelIndex();
		}
	}
	return parentIndex;
	
}
}
