#ifndef NEWDIALOGPROXYMODEL_H
#define NEWDIALOGPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QAbstractListModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
namespace Core{
class IWizard;
class WizardModel: public QStandardItemModel
{
	Q_OBJECT
public:
	WizardModel(QObject *parent = 0);
	virtual ~WizardModel();
	void setWizards(QList<IWizard*> wizards);
	QModelIndex installedTemplateItem() const;

	QModelIndex categoryIndex(const QString &category);
	void addItem(QStandardItem *topLEvelCategoryItem, IWizard *wizard);
	QList<IWizard*> wizards(const QString &category);
	QList<IWizard*> wizards(const QModelIndex &parent);
protected:
	void getWizards(const QModelIndex& parent, QList<IWizard*> &wizards);
	QList<IWizard*> m_wizards;
	QIcon	m_dummyIcon;
	QModelIndex m_installedTemplateIndex;
};
class CategoryProxyModel : public QAbstractProxyModel
{
	Q_OBJECT

public:
	CategoryProxyModel(QObject *parent);
	~CategoryProxyModel();
	QModelIndex installedTemplateItem() const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;

	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &index) const;

	int columnCount(const QModelIndex &index) const;

	virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;
private:
};

class ItemProxyModel : public QAbstractListModel
{
	Q_OBJECT

public:
	ItemProxyModel(WizardModel *sourceModel, QObject *parent);
	~ItemProxyModel();
	void setRootIndex(const QModelIndex &root);

	int rowCount(const QModelIndex &index) const;

	int columnCount(const QModelIndex &index) const;
	IWizard *wizard(const QModelIndex &index) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
private:
	WizardModel *m_model;
	QList<IWizard*> m_wizards;
};

}

#endif // NEWDIALOGPROXYMODEL_H
