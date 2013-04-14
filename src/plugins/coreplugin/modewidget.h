#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QList>

QT_BEGIN_NAMESPACE
class QPainter;
class QStackedLayout;
class QStatusBar;
class QHBoxLayout;
class QMenu;
class QToolBar;
QT_END_NAMESPACE

namespace Core{
namespace Internal{

class ModeTab : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float fader READ fader WRITE setFader)
public:
    ModeTab(QWidget *tabbar) : enabled(false), tabbar(tabbar), m_fader(0)
    {
        animator.setPropertyName("fader");
        animator.setTargetObject(this);
    }
    float fader() { return m_fader; }
    void setFader(float value);

    void fadeIn();
    void fadeOut();

    QIcon icon;
    QString text;
    QString toolTip;
    bool enabled;

private:
    QPropertyAnimation animator;
    QWidget *tabbar;
    float m_fader;
};

class ModeTabBar : public QWidget
{
    Q_OBJECT

public:
    ModeTabBar(QWidget *parent = 0);
    ~ModeTabBar();

    bool event(QEvent *event);

    void paintEvent(QPaintEvent *event);
    void paintTab(QPainter *painter, int tabIndex) const;
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent * e);
    void enterEvent(QEvent * e);
    void leaveEvent(QEvent *e);
    bool validIndex(int index) const { return index >= 0 && index < m_tabs.count(); }

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    void insertTab(int index, const QIcon &icon, const QString &label)
    {
        ModeTab *tab = new ModeTab(this);
        tab->icon = icon;
        tab->text = label;
        m_tabs.insert(index, tab);
    }

    void removeTab(int index)
    {
        ModeTab *tab = m_tabs.takeAt(index);
        delete tab;
    }
    void setCurrentIndex(int index);
    int currentIndex() const { return m_currentIndex; }

    void setTabToolTip(int index, QString toolTip) { m_tabs[index]->toolTip = toolTip; }
    QString tabToolTip(int index) const { return m_tabs.at(index)->toolTip; }

    QIcon tabIcon(int index) const { return m_tabs.at(index)->icon; }
    QString tabText(int index) const { return m_tabs.at(index)->text; }
    int count() const {return m_tabs.count(); }
    QRect tabRect(int index) const;

signals:
    void currentChanged(int);

public slots:
    void emitCurrentIndex();

private:
    static const int m_rounding;
    static const int m_textPadding;
    QRect m_hoverRect;
    int m_hoverIndex;
    int m_currentIndex;
    QList<ModeTab*> m_tabs;
    QTimer m_triggerTimer;
    QSize tabSizeHint(bool minimum = false) const;

};

class ModeTabWidget : public QWidget
{
    Q_OBJECT

public:
    ModeTabWidget(QWidget *parent = 0);

    void insertTab(int index, QWidget *tab, const QIcon &icon, const QString &label);
    void removeTab(int index);
    void setBackgroundBrush(const QBrush &brush);

    void setTabToolTip(int index, const QString &toolTip);

    void paintEvent(QPaintEvent *event);

    int currentIndex() const;
    QStatusBar *statusBar() const;

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;
    void addToolBar(QToolBar *toolBar);
    void removeToolBar(QToolBar *toolBar);
protected:
    virtual void contextMenuEvent (QContextMenuEvent * event);
    QMenu* createPopupMenu();
signals:
    void currentAboutToShow(int index);
    void currentChanged(int index);

public slots:
    void setCurrentIndex(int index);
    void setSelectionWidgetHidden(bool hidden);

private slots:
    void showWidget(int index);

private:
    ModeTabBar *        m_tabBar; //!< 模式管理工具栏，放在selectionWidget最右边
    QWidget *           m_modeToolBar; //!< 模式工具栏，放在selectionWidget最左边
    QHBoxLayout*        m_toolBarLayout;//!< 工具栏layout
    QStackedLayout *    m_modesStack;
    QWidget *           m_selectionWidget;
    QStatusBar *        m_statusBar;
    QList<QToolBar *>   m_toolBars;
};
}

}


#endif // MODEWIDGET_H
