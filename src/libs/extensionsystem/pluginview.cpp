#include "pluginview.h"
#include "pluginmanager.h"
#include "pluginspec.h"
#include "plugincollection.h"
#include "ui_pluginview.h"

#include <QDir>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QPalette>
#include <QtDebug>

using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

Q_DECLARE_METATYPE(ExtensionSystem::PluginSpec*)
Q_DECLARE_METATYPE(ExtensionSystem::PluginCollection*)

PluginView::PluginView(PluginManager *pm, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PluginView),
    m_pManager(pm),
    m_okIcon(QLatin1String(":/extensionsystem/images/ok.png")),
    m_errorIcon(QLatin1String(":/extensionsystem/images/error.png")),
    m_notLoadedIcon(QLatin1String(":/extensionsystem/images/notloaded.png")),
    m_allowCheckStateUpdate(true)
{
    m_ui->setupUi(this);
    QHeaderView *header = m_ui->categoryWidget->header();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	header->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    m_ui->categoryWidget->setColumnWidth(1, 40);

    m_whitelist << QString("Core");
    connect(m_pManager, SIGNAL(pluginsChanged()), this, SLOT(updateList()));
    //当前选择发生变化
    connect(m_ui->categoryWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(selectPlugin(QTreeWidgetItem*)));
    //itemActived在双击或者按回车的时候被发出
    connect(m_ui->categoryWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(activatePlugin(QTreeWidgetItem*)));
    updateList();
}

PluginView::~PluginView()
{
    delete m_ui;
}
/*!
    \fn currentPlugin()
    \brief QTreeWidgetItem中的UserRole里存放着相应的PluginSpec,
    在\fn parsePluginSpecs()中可以看到
*/
PluginSpec *PluginView::currentPlugin() const
{
    if(!m_ui->categoryWidget->currentItem())
    {
        return 0;
    }
    if(!m_ui->categoryWidget->currentItem()->data(0, Qt::UserRole).isNull())
        return m_ui->categoryWidget->currentItem()->data(0, Qt::UserRole).value<PluginSpec *>();
    return 0;
}

void PluginView::updateList()
{
    connect(m_ui->categoryWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updatePluginSettings(QTreeWidgetItem*,int)));
    PluginCollection *defaultCollection = 0;
    foreach(PluginCollection *collection, m_pManager->pluginCollections())
    {
        if (collection->name().isEmpty())
        {
            defaultCollection = collection;
            continue;
        }
        QTreeWidgetItem *collectionItem = new QTreeWidgetItem(
                    QStringList() << collection->name()
                    << QString()    //state
                    << QString()    //load
                    << QString()    //version
                    << QString());  //creator
        m_items.append(collectionItem);
        Qt::CheckState groupState = Qt::Unchecked;
        int state = parsePluginSpecs(collectionItem, groupState,
                                     collection->plugins());
        collectionItem->setIcon(0, iconForState(state));
        collectionItem->setData(1, Qt::CheckStateRole, QVariant(groupState));
        collectionItem->setToolTip(1, tr("Load on Startup"));
        collectionItem->setData(0, Qt::UserRole,
                                qVariantFromValue(collection));
    }
    updatePluginDependencies();
    m_ui->categoryWidget->clear();

    if (!m_items.isEmpty())
    {
        m_ui->categoryWidget->addTopLevelItems(m_items);
        m_ui->categoryWidget->expandAll();
    }
    m_ui->categoryWidget->sortItems(0, Qt::AscendingOrder);
    if (m_ui->categoryWidget->topLevelItemCount())
        m_ui->categoryWidget->setCurrentItem(m_ui->categoryWidget->topLevelItem(0));

}
//在外部由MainWindow来处理调用updateButtons()槽
void PluginView::selectPlugin(QTreeWidgetItem *current)
{
    if(!current)
    {
        emit currentPluginChanged(0);
    }
    else if (current->data(0, Qt::UserRole).canConvert<PluginSpec*>())
    {
        emit currentPluginChanged(current->data(0, Qt::UserRole).value<PluginSpec *>());
    }
    else
        emit currentPluginChanged(0);
}

void PluginView::activatePlugin(QTreeWidgetItem *item)
{
    if (item->data(0, Qt::UserRole).canConvert<PluginSpec*>())
    {
        emit pluginActivated(item->data(0, Qt::UserRole).value<PluginSpec *>());
    }
    else
        emit pluginActivated(0);
}

QIcon PluginView::iconForState(int state)
{
    if(state & ParsedWithErrors)
        return m_errorIcon;
    if(state & ParsedNone || state & ParsedPartial)
        return m_notLoadedIcon;
    return m_okIcon;
}

int PluginView::parsePluginSpecs(QTreeWidgetItem *parentItem,
                                 Qt::CheckState &groupState,
                                 QList<PluginSpec *> plugins)
{
    int ret = 0;
    int loadCount = 0;

    for (int i = 0; i < plugins.length(); ++i)//添加该组的插件到树形控件当中去
    {
        PluginSpec *spec = plugins[i];
        if (spec->hasError())//插件有错
            ret |= ParsedWithErrors;

        QTreeWidgetItem *pluginItem = new QTreeWidgetItem(QStringList()
            << spec->name()
            << QString()    //插件载入状态，是否载入
            << QString::fromLatin1("%1 (%2)").arg(spec->version(), spec->compatVersion())
            << spec->creator());

        pluginItem->setToolTip(0, QDir::toNativeSeparators(spec->filePath()));
        bool ok = !spec->hasError();
        QIcon icon = ok ? m_okIcon : m_errorIcon;
        if (ok && (spec->state() != PluginSpec::Running))
            icon = m_notLoadedIcon;

        pluginItem->setIcon(0, icon);
        pluginItem->setData(0, Qt::UserRole, qVariantFromValue(spec));

        Qt::CheckState state = Qt::Unchecked;
        if (spec->isEnabled())
        {
            state = Qt::Checked;
            ++loadCount;
        }

        if (!m_whitelist.contains(spec->name()))
        {
            pluginItem->setData(1, Qt::CheckStateRole, state);
        }
        else
        {
            pluginItem->setData(1, Qt::CheckStateRole, Qt::Checked);
            pluginItem->setFlags(Qt::ItemIsSelectable);//如果是白名单里面的，那么用户只能选择，不能编辑
        }

        pluginItem->setToolTip(1, tr("Load on Startup"));//设置提示启动时载入

        m_specToItem.insert(spec, pluginItem);//放入hash表当中

        if (parentItem)
            parentItem->addChild(pluginItem);
        else
            m_items.append(pluginItem);

    }

    //设置插件组的状态
    if (loadCount == plugins.length())
    {
        groupState = Qt::Checked;
        ret |= ParsedAll;
    }
    else if (loadCount == 0)
    {
        groupState = Qt::Unchecked;
        ret |= ParsedNone;
    }
    else
    {
        groupState = Qt::PartiallyChecked;
        ret = ret | ParsedPartial;
    }
    return ret;
}

void PluginView::updatePluginDependencies()
{
    foreach (PluginSpec *spec, PluginManager::instance()->loadQueue())
    {
        bool disableIndirectly = false;//间接不可以用flag
        if (m_whitelist.contains(spec->name()))
            continue;

        QHashIterator<PluginDependency, PluginSpec *> it(spec->dependencySpecs());
        while (it.hasNext()) //查看spec所依赖的插件是否不可用（或间接不可用）
        {
            it.next();
            if (it.key().type == PluginDependency::Optional)
                continue;
            PluginSpec *depSpec = it.value();
            if (!depSpec->isEnabled() || depSpec->isDisabledIndirectly())
            {
                disableIndirectly = true;
                break;
            }
        }
        //?????这里是有疑问的，怎么要进行两次判断
        //找到spec所对应的TreeItem，并根据它所依赖的插件的状况设置其可用状态
        QTreeWidgetItem *childItem = m_specToItem.value(spec);
        childItem->setDisabled(disableIndirectly);
        //这里看本身是不是
        if (disableIndirectly == spec->isDisabledIndirectly())
            continue;
        spec->setDisabledIndirectly(disableIndirectly);

        if (childItem->parent() && !childItem->parent()->isExpanded())
            childItem->parent()->setExpanded(true);
    }
}

void PluginView::updatePluginSettings(QTreeWidgetItem *item, int column)
{
    if (!m_allowCheckStateUpdate)//不允许更新插件状态
        return;

    m_allowCheckStateUpdate = false;

    bool loadOnStartup = item->data(1, Qt::CheckStateRole).toBool();

    if (item->data(0, Qt::UserRole).canConvert<PluginSpec*>())
    {
        PluginSpec *spec = item->data(0, Qt::UserRole).value<PluginSpec *>();

        if (column == 1)
        {
            spec->setEnabled(loadOnStartup);
            updatePluginDependencies();

            if (item->parent()) //修改其父节点的状态
            {
                PluginCollection *collection = item->parent()->data(0, Qt::UserRole).value<PluginCollection *>();
                Qt::CheckState state = Qt::PartiallyChecked;
                int loadCount = 0;
                for (int i = 0; i < collection->plugins().length(); ++i)
                {
                    if (collection->plugins().at(i)->isEnabled())
                        ++loadCount;
                }
                if (loadCount == collection->plugins().length())
                    state = Qt::Checked;
                else if (loadCount == 0)
                    state = Qt::Unchecked;

                item->parent()->setData(1, Qt::CheckStateRole, state);
            }

            emit pluginSettingsChanged(spec);
        }

    }
    else
    {
        PluginCollection *collection = item->data(0, Qt::UserRole).value<PluginCollection *>();
        for (int i = 0; i < collection->plugins().length(); ++i)
        {
            PluginSpec *spec = collection->plugins().at(i);
            QTreeWidgetItem *child = m_specToItem.value(spec);

            if (!m_whitelist.contains(spec->name()))
            {
                spec->setEnabled(loadOnStartup);
                Qt::CheckState state = (loadOnStartup ? Qt::Checked : Qt::Unchecked);
                child->setData(1, Qt::CheckStateRole, state);
            }
            else
            {
                child->setData(1, Qt::CheckStateRole, Qt::Checked);
                child->setFlags(Qt::ItemIsSelectable);
            }
        }
        updatePluginDependencies();
        emit pluginSettingsChanged(collection->plugins().first());
    }

    m_allowCheckStateUpdate = true;
}