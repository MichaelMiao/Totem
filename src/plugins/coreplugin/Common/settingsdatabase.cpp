#include "stdafx.h"
#include "settingsdatabase.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


using namespace Core;
using namespace Core::Internal;

enum { debug_settings = 0 };

namespace Core {
namespace Internal {


typedef QMap<QString, QVariant> SettingsMap;
class SettingsDatabasePrivate
{
public:

	SettingsDatabasePrivate()
	{
		m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("settings"));
	}
	~SettingsDatabasePrivate()
	{
		QSqlDatabase::removeDatabase(QLatin1String("settings"));
	}

    QString effectiveGroup() const
    {
        return m_groups.join(QString(QLatin1Char('/')));
    }

    QString effectiveKey(const QString &key) const
    {
        QString g = effectiveGroup();
        if (!g.isEmpty() && !key.isEmpty())
            g += QLatin1Char('/');
        g += key;
        return g;
    }

    SettingsMap m_settings;

    QStringList m_groups;
    QStringList m_dirtyKeys;

    QSqlDatabase m_db;
};

} // namespace Internal
} // namespace Core

SettingsDatabase::SettingsDatabase(const QString &path, const QString &application, QObject *parent)
    : QObject(parent),
      d(new SettingsDatabasePrivate)
{
    const QLatin1Char slash('/');
    QDir pathDir(path);
    if (!pathDir.exists())
        pathDir.mkpath(pathDir.absolutePath());

    QString fileName = path;
    if (!fileName.endsWith(slash))
        fileName += slash;
    fileName += application;
    fileName += QLatin1String(".db");

    d->m_db.setDatabaseName(fileName);
    if (!d->m_db.open())
    {
        qWarning().nospace() << "Warning: Failed to open settings database at " << fileName << " ("
                             << d->m_db.lastError().driverText() << ")";
    }
    else
    {
        // 如果不存在表，就先创建
        QSqlQuery query(d->m_db);
        query.prepare(QLatin1String("CREATE TABLE IF NOT EXISTS settings ("
                                    "key PRIMARY KEY ON CONFLICT REPLACE, "
                                    "value)"));
        if (!query.exec())
            qWarning().nospace() << "Warning: Failed to prepare settings database! ("
                                 << query.lastError().driverText() << ")";

        if (query.exec(QLatin1String("SELECT key FROM settings")))
        {
            while (query.next())
                d->m_settings.insert(query.value(0).toString(), QVariant());
        }
    }
	d->m_db.close();
}

SettingsDatabase::~SettingsDatabase()
{
	sync();
	delete d;
}

void SettingsDatabase::setValue(const QString &key,
                                const QVariant &value)
{
    const QString effectiveKey = d->effectiveKey(key);


    d->m_settings.insert(effectiveKey, value);

    if (!d->m_db.isOpen())
        return;

    QSqlQuery query(d->m_db);
    query.prepare(QLatin1String("INSERT INTO settings VALUES (?, ?)"));
    query.addBindValue(effectiveKey);
    query.addBindValue(value);
    query.exec();

    if (debug_settings)
        qDebug() << "Stored:" << effectiveKey << "=" << value;
}

QVariant SettingsDatabase::value(const QString &key,
                                 const QVariant &defaultValue) const
{
    const QString effectiveKey = d->effectiveKey(key);
    QVariant value = defaultValue;

    SettingsMap::const_iterator i = d->m_settings.constFind(effectiveKey);
    if (i != d->m_settings.constEnd() && i.value().isValid())
    {
        value = i.value();
    }
    else if (d->m_db.isOpen())
    {
        QSqlQuery query(d->m_db);
        query.prepare(QLatin1String("SELECT value FROM settings WHERE key = ?"));
        query.addBindValue(effectiveKey);
        query.exec();
        if (query.next())
        {
            value = query.value(0);

            if (debug_settings)
                qDebug() << "Retrieved:" << effectiveKey << "=" << value;
        }


        d->m_settings.insert(effectiveKey, value);
    }

    return value;
}

bool SettingsDatabase::contains(const QString &key) const
{
    return d->m_settings.contains(d->effectiveKey(key));
}

void SettingsDatabase::remove(const QString &key)
{
    const QString effectiveKey = d->effectiveKey(key);

    // 从cache里移出key
    foreach (const QString &k, d->m_settings.keys()) {
        // 要么完全匹配，要么匹配到'/'
        if (k.startsWith(effectiveKey)
            && (k.length() == effectiveKey.length()
                || k.at(effectiveKey.length()) == QLatin1Char('/')))
        {
            d->m_settings.remove(k);
        }
    }

    if (!d->m_db.isOpen())
        return;


    QSqlQuery query(d->m_db);
    query.prepare(QLatin1String("DELETE FROM settings WHERE key = ? OR key LIKE ?"));
    query.addBindValue(effectiveKey);
    query.addBindValue(QString(effectiveKey + QLatin1String("/%")));
    query.exec();
}

void SettingsDatabase::beginGroup(const QString &prefix)
{
    d->m_groups.append(prefix);
}

void SettingsDatabase::endGroup()
{
    d->m_groups.removeLast();
}

QString SettingsDatabase::group() const
{
    return d->effectiveGroup();
}

QStringList SettingsDatabase::childKeys() const
{
    QStringList children;

    const QString g = group();
    QMapIterator<QString, QVariant> i(d->m_settings);
    while (i.hasNext())
    {
        const QString &key = i.next().key();
        if (key.startsWith(g) && key.indexOf(QLatin1Char('/'), g.length() + 1) == -1)
        {
            children.append(key.mid(g.length() + 1));
        }
    }

    return children;
}

void SettingsDatabase::sync()
{
    // TODO: Delay writing of dirty keys and save them here
}
