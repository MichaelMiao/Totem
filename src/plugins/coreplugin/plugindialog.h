#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
QT_END_NAMESPACE

namespace ExtensionSystem{
class PluginSpec;
class PluginView;
}
namespace Core{
namespace Internal{

class PluginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PluginDialog(QWidget *parent = 0);

signals:

public slots:
    void updateButtons();
    void updateRestartRequired();
    void openDetails();
    void openDetails(ExtensionSystem::PluginSpec *spec);
    void openErrorDetails();
    void closeDialog();

private:
    ExtensionSystem::PluginView *m_view;
    QPushButton *m_detailsButton;
    QPushButton *m_closeButton;
    QPushButton *m_errorDetailsButton;
    QLabel *m_restartRequired;
    static bool m_isRestartRequired;
};

}
}


#endif // PLUGINDIALOG_H
