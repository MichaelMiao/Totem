#ifndef RIGHTPANE_H
#define RIGHTPANE_H

#include "core_global.h"

#include <QWidget>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Core {

class IMode;
class RightPaneWidget;

class CORE_EXPORT RightPanePlaceHolder : public QWidget
{
    friend class Core::RightPaneWidget;
    Q_OBJECT

public:
    explicit RightPanePlaceHolder(Core::IMode *mode, QWidget *parent = 0);
    ~RightPanePlaceHolder();
    static RightPanePlaceHolder *current();

private slots:
    void currentModeChanged(Core::IMode * mode);

private:
    void applyStoredSize(int width);
    Core::IMode *m_mode;
    static RightPanePlaceHolder* m_current;
};

class CORE_EXPORT RightPaneWidget : public QWidget
{
    Q_OBJECT

public:
    RightPaneWidget();
    ~RightPaneWidget();

    void saveSettings(QSettings *settings);
    void readSettings(QSettings *settings);

    bool isShown();
    void setShown(bool b);

    static RightPaneWidget *instance();

    void setWidget(QWidget *widget);

    int storedWidth();

protected:
    void resizeEvent(QResizeEvent * re);

private:
    void clearWidget();
    bool m_shown;
    int m_width;
    QPointer<QWidget> m_widget;
    static RightPaneWidget *m_instance;
};

} // namespace Core

#endif // RIGHTPANE_H
