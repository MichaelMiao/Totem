#include "plugindialog.h"
#include "extensionsystem/pluginmanager.h"
#include "extensionsystem/pluginview.h"
#include "extensionsystem/pluginspec.h"
#include "extensionsystem/plugindetailsview.h"
#include "extensionsystem/pluginerrorview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

using namespace Core;
using namespace Core::Internal;


bool PluginDialog::m_isRestartRequired = false;

PluginDialog::PluginDialog(QWidget *parent) :
    QDialog(parent),
    m_view(new ExtensionSystem::PluginView(ExtensionSystem::PluginManager::instance(), this))
{
    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(m_view);
    resize(650, 400);
    setWindowTitle(tr("Installed Plugins"));
    m_detailsButton = new QPushButton(tr("Details"), this);
    m_closeButton   = new QPushButton(tr("Close"), this);
    m_errorDetailsButton   = new QPushButton(tr("Error Details"), this);
    m_restartRequired = new QLabel(tr("Restart required."), this);
    if (!m_isRestartRequired)
        m_restartRequired->setVisible(false);
    m_detailsButton->setEnabled(false);
    m_errorDetailsButton->setEnabled(false);
    m_closeButton->setEnabled(true);
    m_closeButton->setDefault(true);

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(m_detailsButton);
    hl->addWidget(m_errorDetailsButton);
    hl->addSpacing(10);
    hl->addWidget(m_restartRequired);
    hl->addStretch(5);
    hl->addWidget(m_closeButton);

    vl->addLayout(hl);

    connect(m_view, SIGNAL(currentPluginChanged(ExtensionSystem::PluginSpec*)),
            this, SLOT(updateButtons()));
    connect(m_view, SIGNAL(pluginActivated(ExtensionSystem::PluginSpec*)),
            this, SLOT(openDetails(ExtensionSystem::PluginSpec*)));
    connect(m_view, SIGNAL(pluginSettingsChanged(ExtensionSystem::PluginSpec*)),
            this, SLOT(updateRestartRequired()));
    connect(m_detailsButton, SIGNAL(clicked()), this, SLOT(openDetails()));
    connect(m_errorDetailsButton, SIGNAL(clicked()), this, SLOT(openErrorDetails()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
    updateButtons();

}

void PluginDialog::updateButtons()
{
    ExtensionSystem::PluginSpec *spec = m_view->currentPlugin();
    if(spec)
    {
        m_detailsButton->setEnabled(true);
        m_errorDetailsButton->setEnabled(spec->hasError());
    }
    else
    {
        m_detailsButton->setEnabled(false);
        m_errorDetailsButton->setEnabled(false);
    }

}

void PluginDialog::updateRestartRequired()
{
    m_isRestartRequired = true;
    m_restartRequired->setVisible(true);
}

void PluginDialog::openDetails()
{
    openDetails(m_view->currentPlugin());
}

void PluginDialog::openDetails(ExtensionSystem::PluginSpec *spec)
{
    if (!spec)
        return;
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Plugin Details of %1").arg(spec->name()));
    QVBoxLayout *layout = new QVBoxLayout;
    dialog.setLayout(layout);
    ExtensionSystem::PluginDetailsView *details = new ExtensionSystem::PluginDetailsView(&dialog);
    layout->addWidget(details);
    details->update(spec);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, &dialog);
    layout->addWidget(buttons);
    connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));
    dialog.resize(400, 500);
    dialog.exec();
}

void PluginDialog::openErrorDetails()
{
    ExtensionSystem::PluginSpec *spec = m_view->currentPlugin();
    if (!spec)
        return;
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Plugin Errors of %1").arg(spec->name()));
    QVBoxLayout *layout = new QVBoxLayout;
    dialog.setLayout(layout);
    ExtensionSystem::PluginErrorView *errors = new ExtensionSystem::PluginErrorView(&dialog);
    layout->addWidget(errors);
    errors->update(spec);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, &dialog);
    layout->addWidget(buttons);
    connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));
    dialog.resize(500, 300);
    dialog.exec();
}

void PluginDialog::closeDialog()
{
    ExtensionSystem::PluginManager::instance()->writeSettings();
    accept();
}
