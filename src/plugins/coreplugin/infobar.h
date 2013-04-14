#ifndef INFOBAR_H
#define INFOBAR_H

#include "core_global.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

namespace Core {

class InfoBar;
class InfoBarDisplay;

class CORE_EXPORT InfoBarEntry
{
public:
    InfoBarEntry(const QString &id, const QString &infoText);
    InfoBarEntry(const InfoBarEntry &other) { *this = other; }

    void setCustomButtonInfo(const QString &_buttonText,
                             QObject *object,
                             const char *member);
    void setCancelButtonInfo(QObject *object, const char *member);
    void setCancelButtonInfo(const QString &cancelButtonText,
                             QObject *object, const char *member);

private:
    QString m_id;
    QString m_infoText;
    QString m_buttonText;
    QObject *m_object;
    const char *m_buttonPressMember;
    QString m_cancelButtonText;
    QObject *m_cancelObject;
    const char *m_cancelButtonPressMember;
    friend class InfoBar;
    friend class InfoBarDisplay;
};

class CORE_EXPORT InfoBar : public QObject
{
    Q_OBJECT

public:
    void addInfo(const InfoBarEntry &info);
    void removeInfo(const QString &id);
    void clear();

signals:
    void changed();

private:
    QList<InfoBarEntry> m_infoBarEntries;
    friend class InfoBarDisplay;
};

class CORE_EXPORT InfoBarDisplay : public QObject
{
    Q_OBJECT

public:
    InfoBarDisplay(QObject *parent = 0);
    void setTarget(QBoxLayout *layout, int index);
    void setInfoBar(InfoBar *infoBar);

private slots:
    void cancelButtonClicked();
    void update();
    void infoBarDestroyed();
    void widgetDestroyed();

private:
    QList<QWidget *> m_infoWidgets;
    InfoBar *m_infoBar;
    QBoxLayout *m_boxLayout;
    int m_boxIndex;
};

} // namespace Core

#endif // INFOBAR_H
