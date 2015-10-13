#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "CustomUI/filterlineedit.h"

#include <QDialog>
#include <QPointer>
#include <QEventLoop>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QSortFilterProxyModel;
class QStackedLayout;
class QLabel;
class QListView;
QT_END_NAMESPACE

namespace Core {

class IOptionsPage;

namespace Internal {
class CategoryModel;
class Category;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:

    static SettingsDialog *getSettingsDialog(QWidget *parent,
                               const QString &initialCategory = QString(),
                               const QString &initialPage = QString());

    bool execDialog();

    virtual QSize sizeHint() const;
public slots:
    void done(int val);

private slots:
    void accept();
    void reject();
    void apply();
    void currentChanged(const QModelIndex &current);
    void currentTabChanged(int index);
    void filter(const QString &text);
    void ensureAllCategoryWidgets();

private:
    SettingsDialog(QWidget *parent);
    ~SettingsDialog();

    void createGui();
    void showCategory(int index);
    void showPage(const QString &categoryId, const QString &pageId);
    void updateEnabledTabs(Category *category, const QString &searchText);
    void ensureCategoryWidget(Category *category);
    void disconnectTabWidgets();

    const QList<Core::IOptionsPage*> m_pages;

    QSet<Core::IOptionsPage*> m_visitedPages;
    QSortFilterProxyModel *m_proxyModel;
    CategoryModel *m_model;
    QString m_currentCategory;
    QString m_currentPage;
    QStackedLayout *m_stackedLayout;
    CustomUI::FilterLineEdit *m_filterLineEdit;
    QListView *m_categoryList;
    QLabel *m_headerLabel;
    bool m_running;
    bool m_applied;
    bool m_finished;
    QList<QEventLoop *> m_eventLoops;
    static QPointer<SettingsDialog> m_instance;

};
}
}



#endif // SETTINGSDIALOG_H
