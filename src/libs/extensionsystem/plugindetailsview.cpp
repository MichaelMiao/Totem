#include "plugindetailsview.h"
#include "ui_plugindetailsview.h"
#include "pluginspec.h"

using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

PluginDetailsView::PluginDetailsView(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PluginDetailsView)
{
    m_ui->setupUi(this);
}

PluginDetailsView::~PluginDetailsView()
{
    delete m_ui;
}

void PluginDetailsView::update(PluginSpec *spec)
{
    if(spec)
    {
        m_ui->name->setText(spec->name());
        m_ui->version->setText(spec->version());
        m_ui->compatVersion->setText(spec->compatVersion());
        m_ui->creator->setText(spec->creator());
        m_ui->component->setText(spec->category());
        m_ui->location->setText(spec->location());
        m_ui->description->setText(spec->description());
        m_ui->copyright->setText(spec->copyright());
        m_ui->license->setText(spec->license());

        //dependencies
        QStringList depStrings;
        foreach (const PluginDependency &dep, spec->dependencies())
        {
            QString depString = dep.name;
            depString += QLatin1String(" (");
            depString += dep.version;
            if (dep.type == PluginDependency::Optional)
                depString += QLatin1String(", optional");
            depString += QLatin1Char(')');
            depStrings.append(depString);
        }
        m_ui->dependencies->addItems(depStrings);
    }
}