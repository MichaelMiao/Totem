/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/
#include "stdafx.h"
#include "shortcutsettings.h"
#include <QAction>
#include <QCoreApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLineEdit>
#include <QShortcut>
#include <QtDebug>
#include <QTreeWidgetItem>
#include "../actionmanager/actionmanager.h"
#include "../actionmanager/command.h"
#include "../actionmanager/commandsfile.h"
#include "../document/documentmanager.h"
#include "../constants.h"
#include "../icore.h"
#include "utils/treewidgetcolumnstretcher.h"


Q_DECLARE_METATYPE(Core::Internal::ShortcutItem*)

using namespace Core;
using namespace Core::Internal;

ShortcutSettings::ShortcutSettings(QObject *parent)
    : CommandMappings(parent), m_initialized(false)
{
    connect(ActionManager::instance(), SIGNAL(commandListChanged()),
            this, SLOT(initialize()));

    setId(QLatin1String(Core::Constants::SETTINGS_ID_SHORTCUTS));
    setDisplayName(tr("Keyboard"));
    setCategory(QLatin1String(Core::Constants::SETTINGS_CATEGORY_CORE));
    setDisplayCategory(QCoreApplication::translate("Core", Core::Constants::SETTINGS_TR_CATEGORY_CORE));
	setCategoryIcon(QLatin1String(Core::Constants::ICON_CATEGORY_SHORTCUT));
}

QWidget *ShortcutSettings::createPage(QWidget *parent)
{
    m_initialized = true;
    m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;

    QWidget *w = CommandMappings::createPage(parent);

    const QString pageTitle = tr("Keyboard Shortcuts");
    const QString targetLabelText = tr("Key sequence:");
    const QString editTitle = tr("Shortcut");

    setPageTitle(pageTitle);
    setTargetLabelText(targetLabelText);
    setTargetEditTitle(editTitle);
    setTargetHeader(editTitle);

    if (m_searchKeywords.isEmpty())
    {
        QTextStream(&m_searchKeywords) << ' ' << pageTitle
                << ' ' << targetLabelText
                << ' ' << editTitle;
    }

    return w;
}

void ShortcutSettings::apply()
{
    foreach (ShortcutItem *item, m_scitems)
        item->m_cmd->setKeySequence(item->m_key);
}

void ShortcutSettings::finish()
{
    qDeleteAll(m_scitems);
    m_scitems.clear();

    CommandMappings::finish();
    m_initialized = false;
}

bool ShortcutSettings::matches(const QString &s) const
{
    return m_searchKeywords.contains(s, Qt::CaseInsensitive);
}

bool ShortcutSettings::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o)

    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent *k = static_cast<QKeyEvent*>(e);
        handleKeyEvent(k);
        return true;
    }

    if ( e->type() == QEvent::Shortcut ||
         e->type() == QEvent::KeyRelease )
    {
        return true;
    }

    if (e->type() == QEvent::ShortcutOverride) {
        // for shortcut overrides, we need to accept as well
        e->accept();
        return true;
    }

    return false;
}

void ShortcutSettings::commandChanged(QTreeWidgetItem *current)
{
    CommandMappings::commandChanged(current);
    if (!current || !current->data(0, Qt::UserRole).isValid())
        return;
	ShortcutItem *scitem = current->data(0, Qt::UserRole).value<ShortcutItem *>();
    setKeySequence(scitem->m_key);
}

void ShortcutSettings::targetIdentifierChanged()
{
    QTreeWidgetItem *current = commandList()->currentItem();
    if (current && current->data(0, Qt::UserRole).isValid())
    {
        ShortcutItem *scitem = current->data(0, Qt::UserRole).value<ShortcutItem *>();
        scitem->m_key = QKeySequence(m_key[0], m_key[1], m_key[2], m_key[3]);
        if (scitem->m_cmd->defaultKeySequence() != scitem->m_key)
            setModified(current, true);
        else
            setModified(current, false);
        current->setText(2, scitem->m_key.toString());
        resetCollisionMarker(scitem);
        markPossibleCollisions(scitem);
    }
}

void ShortcutSettings::setKeySequence(const QKeySequence &key)
{
    m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    m_keyNum = key.count();
    for (int i = 0; i < m_keyNum; ++i)
    {
        m_key[i] = key[i];
    }
    targetEdit()->setText(key.toString());
}

void ShortcutSettings::resetTargetIdentifier()
{
    QTreeWidgetItem *current = commandList()->currentItem();
    if (current && current->data(0, Qt::UserRole).isValid())
    {
		ShortcutItem *scitem = current->data(0, Qt::UserRole).value<ShortcutItem *>();
        setKeySequence(scitem->m_cmd->defaultKeySequence());
    }
}

void ShortcutSettings::removeTargetIdentifier()
{
    m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    targetEdit()->clear();
}

void ShortcutSettings::importAction()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("Import Keyboard Mapping Scheme"),
        ICore::instance()->resourcePath() + QLatin1String("/schemes/"),
        tr("Keyboard Mapping Scheme (*.kms)"));
    if (!fileName.isEmpty())
    {
        CommandsFile cf(fileName);
        QMap<QString, QKeySequence> mapping = cf.importCommands();

        foreach (ShortcutItem *item, m_scitems)
        {
            QString sid = item->m_cmd->id().toString();
            if (mapping.contains(sid))
            {
                item->m_key = mapping.value(sid);
                item->m_item->setText(2, item->m_key.toString());
                if (item->m_item == commandList()->currentItem())
                    commandChanged(item->m_item);

                if (item->m_cmd->defaultKeySequence() != item->m_key)
                    setModified(item->m_item, true);
                else
                    setModified(item->m_item, false);
            }
        }

        foreach (ShortcutItem *item, m_scitems)
        {
            resetCollisionMarker(item);
            markPossibleCollisions(item);
        }
    }
}

void ShortcutSettings::defaultAction()
{
    foreach (ShortcutItem *item, m_scitems)
    {
        item->m_key = item->m_cmd->defaultKeySequence();
        item->m_item->setText(2, item->m_key.toString());
        setModified(item->m_item, false);
        if (item->m_item == commandList()->currentItem())
            commandChanged(item->m_item);
    }

    foreach (ShortcutItem *item, m_scitems)
    {
        resetCollisionMarker(item);
        markPossibleCollisions(item);
    }
}

void ShortcutSettings::exportAction()
{
    QString fileName = DocumentManager::getSaveFileNameWithExtension(
        tr("Export Keyboard Mapping Scheme"),
        ICore::instance()->resourcePath() + QLatin1String("/schemes/"),
        tr("Keyboard Mapping Scheme (*.kms)"));
    if (!fileName.isEmpty()) {
        CommandsFile cf(fileName);
        cf.exportCommands(m_scitems);
    }
}

void ShortcutSettings::clear()
{
    QTreeWidget *tree = commandList();
    for (int i = tree->topLevelItemCount()-1; i >= 0 ; --i)
    {
        delete tree->takeTopLevelItem(i);
    }
    qDeleteAll(m_scitems);
    m_scitems.clear();
}

void ShortcutSettings::initialize()
{
    if (!m_initialized)
        return;
    clear();
    QMap<QString, QTreeWidgetItem *> sections;

    foreach (Command *c, ActionManager::commands())
    {
        if (c->hasAttribute(Command::CA_NonConfigurable))
            continue;
        if (c->action() && c->action()->isSeparator())
            continue;

        QTreeWidgetItem *item = 0;
        ShortcutItem *s = new ShortcutItem;
        m_scitems << s;
        item = new QTreeWidgetItem;
        s->m_cmd = c;
        s->m_item = item;

        const QString identifier = c->id().toString();
        int pos = identifier.indexOf(QLatin1Char('.'));
        const QString section = identifier.left(pos);
        const QString subId = identifier.mid(pos + 1);
        if (!sections.contains(section))
        {
            QTreeWidgetItem *categoryItem =
                    new QTreeWidgetItem(commandList(),
                                        QStringList() << section);
            QFont f = categoryItem->font(0);
            f.setBold(true);
            categoryItem->setFont(0, f);
            sections.insert(section, categoryItem);
            commandList()->expandItem(categoryItem);
        }
        sections[section]->addChild(item);

        s->m_key = c->keySequence();
        item->setText(0, subId);
        item->setText(1, c->description());
        item->setText(2, s->m_key.toString());
        if (s->m_cmd->defaultKeySequence() != s->m_key)
            setModified(item, true);

        item->setData(0, Qt::UserRole, qVariantFromValue(s));

        markPossibleCollisions(s);
    }
    filterChanged(filterText());
}

void ShortcutSettings::handleKeyEvent(QKeyEvent *e)
{
    int nextKey = e->key();
    if ( m_keyNum > 3 ||
         nextKey == Qt::Key_Control ||
         nextKey == Qt::Key_Shift ||
         nextKey == Qt::Key_Meta ||
         nextKey == Qt::Key_Alt )
         return;

    nextKey |= translateModifiers(e->modifiers(), e->text());
    switch (m_keyNum) {
        case 0:
            m_key[0] = nextKey;
            break;
        case 1:
            m_key[1] = nextKey;
            break;
        case 2:
            m_key[2] = nextKey;
            break;
        case 3:
            m_key[3] = nextKey;
            break;
        default:
            break;
    }
    m_keyNum++;
    QKeySequence ks(m_key[0], m_key[1], m_key[2], m_key[3]);
    targetEdit()->setText(ks.toString());
    e->accept();
}

int ShortcutSettings::translateModifiers(Qt::KeyboardModifiers state,
                                         const QString &text)
{
    int result = 0;
    // The shift modifier only counts when it is not used to type a symbol
    // that is only reachable using the shift key anyway
    if ((state & Qt::ShiftModifier) && (text.size() == 0
                                        || !text.at(0).isPrint()
                                        || text.at(0).isLetterOrNumber()
                                        || text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}

void ShortcutSettings::markPossibleCollisions(ShortcutItem *item)
{
    if (item->m_key.isEmpty())
        return;

    int globalId = Context(Constants::C_GLOBAL).at(0);

    foreach (ShortcutItem *currentItem, m_scitems)
    {

        if (currentItem->m_key.isEmpty() || item == currentItem ||
            item->m_key != currentItem->m_key)
        {
            continue;
        }

        foreach (int context, currentItem->m_cmd->context())
        {

            // conflict if context is identical, OR if one
            // of the contexts is the global context
            if (item->m_cmd->context().contains(context) ||
               (item->m_cmd->context().contains(globalId) &&
                !currentItem->m_cmd->context().isEmpty()) ||
                (currentItem->m_cmd->context().contains(globalId) &&
                !item->m_cmd->context().isEmpty()))
            {
                currentItem->m_item->setForeground(2, Qt::red);
                item->m_item->setForeground(2, Qt::red);
            }
        }
    }
}

void ShortcutSettings::resetCollisionMarker(ShortcutItem *item)
{
    item->m_item->setForeground(2, commandList()->palette().foreground());
}


void ShortcutSettings::resetCollisionMarkers()
{
    foreach (ShortcutItem *item, m_scitems)
        resetCollisionMarker(item);
}
