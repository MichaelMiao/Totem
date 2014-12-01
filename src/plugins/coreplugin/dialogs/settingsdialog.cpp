﻿#include "settingsdialog.h"
#include "../icore.h"
#include "ioptionspage.h"
#include "extensionsystem/pluginmanager.h"
#include <QEvent>
#include <QEventLoop>
#include <QSettings>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QHBoxLayout>
#include <QApplication>
#include <QDialogButtonBox>
#include <QStackedLayout>
#include <QTabWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QScrollBar>

static const char categoryKeyC[] = "General/LastPreferenceCategory";
static const char pageKeyC[] = "General/LastPreferencePage";

const int categoryIconSize = 24;

namespace Core{
namespace Internal{

QPointer<SettingsDialog> SettingsDialog::m_instance = 0;

// ----------- Category model

class Category {
public:
    QString id;
    QString displayName;
    QIcon icon;
    QList<IOptionsPage *> pages;
    QList<IOptionsPageProvider *> providers;
    int index;
    QTabWidget *tabWidget;
};

class CategoryModel : public QAbstractListModel
{
public:
    CategoryModel(QObject *parent = 0);
    ~CategoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setPages(const QList<IOptionsPage*> &pages,
                  const QList<IOptionsPageProvider *> &providers);
    const QList<Category*> &categories() const { return m_categories; }

private:
    Category *findCategoryById(const QString &id);

    QList<Category*> m_categories;
    QIcon m_emptyIcon;
};

CategoryModel::CategoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QPixmap empty(categoryIconSize, categoryIconSize);
    empty.fill(Qt::transparent);
    m_emptyIcon = QIcon(empty);
}

CategoryModel::~CategoryModel()
{
    qDeleteAll(m_categories);
}

int CategoryModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_categories.size();
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        return m_categories.at(index.row())->displayName;
    case Qt::DecorationRole: {
            QIcon icon = m_categories.at(index.row())->icon;
            if (icon.isNull())
                icon = m_emptyIcon;
            return icon;
        }
    }

    return QVariant();
}

void CategoryModel::setPages(const QList<IOptionsPage*> &pages,
                             const QList<IOptionsPageProvider *> &providers)
{
	beginResetModel();
    // 清空之前的category
    qDeleteAll(m_categories);
    m_categories.clear();

    // 往category里添加page
    foreach (IOptionsPage *page, pages)
    {
        const QString &categoryId = page->category();
        Category *category = findCategoryById(categoryId);
        if (!category)
        {
            category = new Category;
            category->id = categoryId;
            category->tabWidget = 0;
            category->index = -1;
            m_categories.append(category);
        }
        //displayName, icon
        if (category->displayName.isEmpty())
            category->displayName = page->displayCategory();
        if (category->icon.isNull())
            category->icon = page->categoryIcon();
        category->pages.append(page);
    }

    foreach (IOptionsPageProvider *provider, providers)
    {
        const QString &categoryId = provider->category();
        Category *category = findCategoryById(categoryId);
        if (!category)
        {
            category = new Category;
            category->id = categoryId;
            category->tabWidget = 0;
            category->index = -1;
            m_categories.append(category);
        }
        if (category->displayName.isEmpty())
            category->displayName = provider->displayCategory();
        if (category->icon.isNull())
            category->icon = provider->categoryIcon();
        category->providers.append(provider);
    }

	endResetModel();
}

Category *CategoryModel::findCategoryById(const QString &id)
{
    for (int i = 0; i < m_categories.size(); ++i)
    {
        Category *category = m_categories.at(i);
        if (category->id == id)
            return category;
    }

    return 0;
}

// ----------- Category filter model

class CategoryFilterModel : public QSortFilterProxyModel
{
public:
    explicit CategoryFilterModel(QObject *parent = 0)
        : QSortFilterProxyModel(parent)
    {}

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

bool CategoryFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // Regular contents check, then check page-filter.
    if (QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent))
        return true;

    const CategoryModel *cm = static_cast<CategoryModel*>(sourceModel());
    foreach (const IOptionsPage *page, cm->categories().at(sourceRow)->pages) {
        const QString pattern = filterRegExp().pattern();
        if (page->displayCategory().contains(pattern, Qt::CaseInsensitive)
            || page->displayName().contains(pattern, Qt::CaseInsensitive)
            || page->matches(pattern))
            return true;
    }

    return false;
}

class CategoryListViewDelegate : public QStyledItemDelegate
{
public:
    CategoryListViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(qMax(size.height(), 32));
        return size;
    }
};

class CategoryListView : public QListView
{
public:
    CategoryListView(QWidget *parent = 0) : QListView(parent)
    {
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        setItemDelegate(new CategoryListViewDelegate(this));
    }

    virtual QSize sizeHint() const
    {
        int width = sizeHintForColumn(0) + frameWidth() * 2 + 5;
        if (verticalScrollBar()->isVisible())
            width += verticalScrollBar()->width();
        return QSize(width, 100);
    }
};


//----------------------------------------------------------
//----------------------------------------------------------

bool optionsPageLessThan(const IOptionsPage *p1, const IOptionsPage *p2)
{
    if (const int cc = p1->category().compare(p2->category()))
        return cc < 0;
    return p1->id().compare(p2->id()) < 0;
}

static inline QList<Core::IOptionsPage*> sortedOptionsPages()
{
    QList<Core::IOptionsPage*> rc = ExtensionSystem::PluginManager::instance()->getObjects<Core::IOptionsPage>();
    qStableSort(rc.begin(), rc.end(), optionsPageLessThan);
    return rc;
}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_pages(sortedOptionsPages()),
    m_proxyModel(new CategoryFilterModel(this)),
    m_model(new CategoryModel(this)),
    m_stackedLayout(new QStackedLayout),
    m_filterLineEdit(new CustomUI::FilterLineEdit),
    m_categoryList(new CategoryListView),
    m_headerLabel(new QLabel),
    m_running(false),
    m_applied(false),
    m_finished(false)
{
    createGui();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
#ifdef Q_OS_MAC
    setWindowTitle(tr("Preferences"));
#else
    setWindowTitle(tr("Options"));
#endif

    m_model->setPages(m_pages,
        ExtensionSystem::PluginManager::instance()->getObjects<IOptionsPageProvider>());

    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_categoryList->setIconSize(QSize(categoryIconSize, categoryIconSize));
    m_categoryList->setModel(m_proxyModel);
    m_categoryList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_categoryList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(m_categoryList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged(QModelIndex)));

    // The order of the slot connection matters here, the filter slot
    // opens the matching page after the model has filtered.
    connect(m_filterLineEdit, SIGNAL(filterChanged(QString)),
            this, SLOT(ensureAllCategoryWidgets()));
    connect(m_filterLineEdit, SIGNAL(filterChanged(QString)),
                m_proxyModel, SLOT(setFilterFixedString(QString)));
    connect(m_filterLineEdit, SIGNAL(filterChanged(QString)), this, SLOT(filter(QString)));
    m_categoryList->setFocus();
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::createGui()
{
    // 头标签，加粗加大字体
    QFont headerLabelFont = m_headerLabel->font();
    headerLabelFont.setBold(true);

    const int pointSize = headerLabelFont.pointSize();
    if (pointSize > 0)
        headerLabelFont.setPointSize(pointSize + 2);
    m_headerLabel->setFont(headerLabelFont);

    QHBoxLayout *headerHLayout = new QHBoxLayout;
    const int leftMargin = qApp->style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
    headerHLayout->addSpacerItem(new QSpacerItem(leftMargin, 0, QSizePolicy::Fixed, QSizePolicy::Ignored));
    headerHLayout->addWidget(m_headerLabel);

    m_stackedLayout->setMargin(0);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Apply |
                                                       QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainGridLayout = new QGridLayout;
    mainGridLayout->addWidget(m_filterLineEdit, 0, 0, 1, 1);
    mainGridLayout->addLayout(headerHLayout,    0, 1, 1, 1);
    mainGridLayout->addWidget(m_categoryList,   1, 0, 1, 1);
    mainGridLayout->addLayout(m_stackedLayout,  1, 1, 1, 1);
    mainGridLayout->addWidget(buttonBox,        2, 0, 1, 2);
    mainGridLayout->setColumnStretch(1, 4);
    setLayout(mainGridLayout);
    setMinimumSize(1000, 550);
}

void SettingsDialog::showCategory(int index)
{
    Category *category = m_model->categories().at(index);
    ensureCategoryWidget(category);
    // 更新当前category和page
    m_currentCategory = category->id;
    const int currentTabIndex = category->tabWidget->currentIndex();
    if (currentTabIndex != -1)
    {
        IOptionsPage *page = category->pages.at(currentTabIndex);
        m_currentPage = page->id();
        m_visitedPages.insert(page);
    }

    m_stackedLayout->setCurrentIndex(category->index);
    m_headerLabel->setText(category->displayName);

    updateEnabledTabs(category, m_filterLineEdit->text());
}

void SettingsDialog::showPage(const QString &categoryId, const QString &pageId)
{
    QString initialCategory = categoryId;
    QString initialPage = pageId;
    if (initialCategory.isEmpty() && initialPage.isEmpty())
    {
        QSettings *settings = ICore::settings();
        initialCategory = settings->value(QLatin1String(categoryKeyC), QVariant(QString())).toString();
        initialPage = settings->value(QLatin1String(pageKeyC), QVariant(QString())).toString();
    }

    int initialCategoryIndex = -1;
    int initialPageIndex = -1;
    const QList<Category*> &categories = m_model->categories();
    for (int i = 0; i < categories.size(); ++i)
    {
        Category *category = categories.at(i);
        if (category->id == initialCategory)
        {
            initialCategoryIndex = i;
            for (int j = 0; j < category->pages.size(); ++j)
            {
                IOptionsPage *page = category->pages.at(j);
                if (page->id() == initialPage)
                    initialPageIndex = j;
            }
        }
    }
    if (initialCategoryIndex != -1)
    {
        const QModelIndex modelIndex = m_proxyModel->mapFromSource(m_model->index(initialCategoryIndex));
        m_categoryList->setCurrentIndex(modelIndex);
        if (initialPageIndex != -1)
            categories.at(initialCategoryIndex)->tabWidget->setCurrentIndex(initialPageIndex);
    }
}

SettingsDialog *SettingsDialog::getSettingsDialog(QWidget *parent,
                           const QString &initialCategory,
                           const QString &initialPage)
{
    if (!m_instance)
    {
        m_instance = new SettingsDialog(parent);
    }
    m_instance->showPage(initialCategory, initialPage);
    return m_instance;
}

bool SettingsDialog::execDialog()
{
    if (!m_running)
    {
        m_running = true;
        m_finished = false;
        exec();
        m_running = false;
        m_instance = 0;
        deleteLater();
    }
    else
    {
        QEventLoop *loop = new QEventLoop(this);
        m_eventLoops.append(loop);
        loop->exec();
    }
    return m_applied;
}

QSize SettingsDialog::sizeHint() const
{
    return minimumSize();
}

void SettingsDialog::done(int val)
{
    QSettings *settings = ICore::settings();
    settings->setValue(QLatin1String(categoryKeyC), m_currentCategory);
    settings->setValue(QLatin1String(pageKeyC), m_currentPage);

    // 退出多余的事件循环
    QListIterator<QEventLoop *> it(m_eventLoops);
    it.toBack();
    while (it.hasPrevious())
    {
        QEventLoop *loop = it.previous();
        loop->exit();
    }

    QDialog::done(val);
}

void SettingsDialog::accept()
{
    if (m_finished)
        return;
    m_finished = true;
    disconnectTabWidgets();
    m_applied = true;
    foreach (IOptionsPage *page, m_visitedPages)
        page->apply();
    foreach (IOptionsPage *page, m_pages)
        page->finish();
    done(QDialog::Accepted);

}

void SettingsDialog::reject()
{
    if (m_finished)
        return;
    m_finished = true;
    disconnectTabWidgets();
    foreach (IOptionsPage *page, m_pages)
        page->finish();
    done(QDialog::Rejected);
}

void SettingsDialog::apply()
{
    foreach (IOptionsPage *page, m_visitedPages)
        page->apply();
    m_applied = true;
}

void SettingsDialog::currentChanged(const QModelIndex &current)
{
    if (current.isValid())
        showCategory(m_proxyModel->mapToSource(current).row());

}

void SettingsDialog::currentTabChanged(int index)
{
    if (index == -1)
        return;

    const QModelIndex modelIndex = m_proxyModel->mapToSource(m_categoryList->currentIndex());
    if (!modelIndex.isValid())
        return;

    // Remember the current tab and mark it as visited
    const Category *category = m_model->categories().at(modelIndex.row());
    IOptionsPage *page = category->pages.at(index);
    m_currentPage = page->id();
    m_visitedPages.insert(page);
}

void SettingsDialog::filter(const QString &text)
{
    ensureAllCategoryWidgets();
    // When there is no current index, select the first one when possible
    if (!m_categoryList->currentIndex().isValid() && m_model->rowCount() > 0)
        m_categoryList->setCurrentIndex(m_proxyModel->index(0, 0));

    const QModelIndex currentIndex = m_proxyModel->mapToSource(m_categoryList->currentIndex());
    if (!currentIndex.isValid())
        return;

    Category *category = m_model->categories().at(currentIndex.row());
    updateEnabledTabs(category, text);
}

void SettingsDialog::ensureAllCategoryWidgets()
{
    foreach (Category *category, m_model->categories())
        ensureCategoryWidget(category);
}

void SettingsDialog::ensureCategoryWidget(Category *category)
{
    if (category->tabWidget != 0)
        return;
    foreach (const IOptionsPageProvider *provider, category->providers)
    {
        category->pages += provider->pages();
    }
    qStableSort(category->pages.begin(), category->pages.end(), optionsPageLessThan);

    QTabWidget *tabWidget = new QTabWidget;
    for (int j = 0; j < category->pages.size(); ++j)
    {
        IOptionsPage *page = category->pages.at(j);
        QWidget *widget = page->createPage(0);
        tabWidget->addTab(widget, page->displayName());
    }

    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(currentTabChanged(int)));

    category->tabWidget = tabWidget;
    category->index = m_stackedLayout->addWidget(tabWidget);
}

void SettingsDialog::disconnectTabWidgets()
{
    foreach (Category *category, m_model->categories())
    {
        if (category->tabWidget)
            disconnect(category->tabWidget, SIGNAL(currentChanged(int)),
                    this, SLOT(currentTabChanged(int)));
    }
}

void SettingsDialog::updateEnabledTabs(Category *category, const QString &searchText)
{
    for (int i = 0; i < category->pages.size(); ++i)
    {
        const IOptionsPage *page = category->pages.at(i);
        const bool enabled = searchText.isEmpty()
                             || page->category().contains(searchText, Qt::CaseInsensitive)
                             || page->displayName().contains(searchText, Qt::CaseInsensitive)
                             || page->matches(searchText);
        category->tabWidget->setTabEnabled(i, enabled);
    }
}


}
}
