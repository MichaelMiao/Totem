#ifndef SETTINGSDATABASE_H
#define SETTINGSDATABASE_H

#include "core_global.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace Core {

namespace Internal {
class SettingsDatabasePrivate;
}

class CORE_EXPORT SettingsDatabase : public QObject
{
public:

	SettingsDatabase(const QString &path, const QString &application, QObject *parent = 0);
    ~SettingsDatabase();

    void	setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    bool	contains(const QString &key) const;
    void	remove(const QString &key);

    void	beginGroup(const QString &prefix);
    void	endGroup();
    QString group() const;
    QStringList childKeys() const;

    void sync();

private:
    Internal::SettingsDatabasePrivate *d;
};

} // namespace Core

#endif // SETTINGSDATABASE_H
