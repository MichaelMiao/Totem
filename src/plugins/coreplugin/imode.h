#ifndef IMODE_H
#define IMODE_H
#include "icontext.h"
#include "id.h"

#include <QIcon>

namespace Core{

class CORE_EXPORT IMode : public IContext
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

public:
    IMode(QObject *parent = 0);
    virtual ~IMode(){}
    QString displayName() const { return m_displayName; }
    QIcon icon() const { return m_icon; }
    int priority() const { return m_priority; }
    Id id() const { return m_id; }
    Id type() const { return m_type; }
    bool isEnabled() const;

    void setEnabled(bool enabled);
    void setDisplayName(const QString &displayName) { m_displayName = displayName; }
    void setIcon(const QIcon &icon) { m_icon = icon; }
    void setPriority(int priority) { m_priority = priority; }
    void setId(Id id) { m_id = id; }
    void setType(Id type) { m_type = type; }

signals:
    void enabledStateChanged(bool enabled);

private:

    QString m_displayName;
    QIcon m_icon;
    int m_priority;
    Id m_id;
    Id m_type;
    bool m_isEnabled;
};

}

#endif // IMODE_H
