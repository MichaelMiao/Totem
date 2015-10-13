#pragma once
#include <QIcon>
#include "../core_global.h"


namespace Core{

class CORE_EXPORT IOptionsPage : public QObject
{
    Q_OBJECT

public:

    IOptionsPage(QObject *parent = 0) : QObject(parent) {}

    QString id() const { return m_id; }
    QString displayName() const { return m_displayName; }
    QString category() const { return m_category; }
    QString displayCategory() const { return m_displayCategory; }
    QIcon categoryIcon() const { return QIcon(m_categoryIcon); }

    virtual bool matches(const QString & /* searchKeyWord*/) const { return false; }
    virtual QWidget *createPage(QWidget *parent) = 0;
    virtual void apply() = 0;
    virtual void finish() = 0;

protected:

    void setId(const QString &id) { m_id = id; }
    void setDisplayName(const QString &displayName) { m_displayName = displayName; }
    void setCategory(const QString &category) { m_category = category; }
    void setDisplayCategory(const QString &displayCategory) { m_displayCategory = displayCategory; }
    void setCategoryIcon(const QString &categoryIcon) { m_categoryIcon = categoryIcon; }

    QString m_id;
    QString m_displayName;
    QString m_category;
    QString m_displayCategory;
    QString m_categoryIcon;
};

class CORE_EXPORT IOptionsPageProvider : public QObject
{
    Q_OBJECT

public:

    IOptionsPageProvider(QObject *parent = 0) : QObject(parent) {}

    QString category() const { return m_category; }
    QString displayCategory() const { return m_displayCategory; }
    QIcon categoryIcon() const { return QIcon(m_categoryIcon); }

    virtual QList<IOptionsPage *> pages() const = 0;

protected:

    void setCategory(const QString &category) { m_category = category; }
    void setDisplayCategory(const QString &displayCategory) { m_displayCategory = displayCategory; }
    void setCategoryIcon(const QString &categoryIcon) { m_categoryIcon = categoryIcon; }

    QString m_category;
    QString m_displayCategory;
    QString m_categoryIcon;
};

}
