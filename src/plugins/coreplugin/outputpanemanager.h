#ifndef OUTPUTPANEMANAGER_H
#define OUTPUTPANEMANAGER_H

#include "id.h"

#include <QMap>
#include <QToolButton>
#include <QVector>
#include <QList>

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QStackedWidget;
class QSplitter;
class QLabel;
class QTimeLine;
class QToolButton;
QT_END_NAMESPACE

namespace Core {

class IOutputPane;

namespace Internal {
class OutputPaneManager;
class OutputPaneManageButton;
class MainWindow;
}

namespace Internal {

class OutputPaneToggleButton;

class OutputPaneManager : public QWidget
{
    Q_OBJECT

public:
    void init();
    static OutputPaneManager *instance();
    QWidget *buttonsWidget();
    void updateStatusButtons(bool visible);

    bool isMaximized()const;

public slots:
    void slotHide();
    void shortcutTriggered();
    void slotMinMax();

protected:
    void focusInEvent(QFocusEvent *e);

private slots:

    void showPage(bool focus, bool ensureSizeHint);
    void togglePage(bool focus);
    void clearPage();
    void buttonTriggered();

    void popupMenu();

private:
    // the only class that is allowed to create and destroy
    friend class MainWindow;

    static void create();
    static void destroy();

    explicit OutputPaneManager(QWidget *parent = 0);
    ~OutputPaneManager();

    void showPage(int idx, bool focus);
    void ensurePageVisible(int idx);
    void setCurrentIndex(int idx);

    void readSettings();
    void saveSettings();

    int currentIndex() const;
    int findIndexForPage(IOutputPane *out);

    void buttonTriggered(int idx);


    QLabel *m_titleLabel;

    QAction *m_clearAction;
    QToolButton *m_clearButton;
    QToolButton *m_closeButton;

    QAction *m_minMaxAction;
    QToolButton *m_minMaxButton;

    QToolButton *m_prevToolButton;
    QToolButton *m_nextToolButton;
    OutputPaneManageButton *m_manageButton;

    QList<IOutputPane *> m_panes;

    int m_lastIndex;

    QStackedWidget *m_outputWidgetPane;
    QStackedWidget *m_opToolBarWidgets;
    QWidget *m_toolBar;
    QWidget *m_buttonsWidget;

    QVector<OutputPaneToggleButton *> m_buttons;
    QVector<QAction *> m_actions;
    QVector<Id> m_ids;


    QPixmap m_minimizeIcon;
    QPixmap m_maximizeIcon;
    bool m_maximised;
};

class OutputPaneToggleButton : public QToolButton
{
    Q_OBJECT
public:
    OutputPaneToggleButton(int number, const QString &text, QAction *action,
                           QWidget *parent = 0);
    QSize sizeHint() const;
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void flash(int count = 3);
    void setIconBadgeNumber(int number);

private slots:
    void updateToolTip();

private:
    void checkStateSet();

    QString m_number;
    QString m_text;
    QAction *m_action;
    QTimeLine *m_flashTimer;
    QLabel *m_label;
};

class OutputPaneManageButton : public QToolButton
{
    Q_OBJECT
public:
    OutputPaneManageButton();
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *event);
};

} // namespace Internal
} // namespace Core

#endif // OUTPUTPANEMANAGER_H
