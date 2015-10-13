﻿/**************************************************************************
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

#include "saveitemsdialog.h"
#include "mainwindow.h"

#include <coreplugin/idocument.h>
#include <QFileIconProvider>
#include <QDir>
#include <QFileInfo>
#include <QPushButton>
#include <QTreeWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QtDebug>

Q_DECLARE_METATYPE(Core::IDocument*)

using namespace Core;
using namespace Core::Internal;

SaveItemsDialog::SaveItemsDialog(QWidget *parent,
                                 QList<IDocument *> items)
    : QDialog(parent)
{
    m_ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);//没有问号
#ifdef Q_OS_MAC
    // QDialogButtonBox's behavior for "destructive" is wrong, the "do not save" should be left-aligned
    QDialogButtonBox::ButtonRole discardButtonRole = QDialogButtonBox::ResetRole;
#else
    QDialogButtonBox::ButtonRole discardButtonRole = QDialogButtonBox::DestructiveRole;
#endif
    QPushButton *discardButton = m_ui.buttonBox->addButton(tr("Do not Save"), discardButtonRole);
    m_ui.buttonBox->button(QDialogButtonBox::Save)->setDefault(true);
    m_ui.treeWidget->setFocus();

    m_ui.saveBeforeBuildCheckBox->setVisible(false);
    QFileIconProvider provider;
    foreach (IDocument *document, items)
    {
        QString visibleName;
        QString directory;
        QString fileName = document->fileName();
        if (fileName.isEmpty())
        {
            visibleName = document->suggestedFileName();
        }
        else
        {
            QFileInfo info = QFileInfo(fileName);
            directory = info.absolutePath();
            visibleName = info.fileName();
        }
        QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.treeWidget, QStringList()
                                                    << visibleName << QDir::toNativeSeparators(directory));
        if (!fileName.isEmpty())
            item->setIcon(0, provider.icon(QFileInfo(fileName)));
        item->setData(0, Qt::UserRole, qVariantFromValue(document));
    }

    m_ui.treeWidget->resizeColumnToContents(0);
    m_ui.treeWidget->selectAll();
#ifdef Q_OS_MAC
    m_ui.treeWidget->setAlternatingRowColors(true);
#endif
    adjustButtonWidths();
    updateSaveButton();

    connect(m_ui.buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()),
            this, SLOT(collectItemsToSave()));
    connect(discardButton, SIGNAL(clicked()), this, SLOT(discardAll()));
    connect(m_ui.treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateSaveButton()));
}

void SaveItemsDialog::setMessage(const QString &msg)
{
    m_ui.msgLabel->setText(msg);
}

void SaveItemsDialog::updateSaveButton()
{
    int count = m_ui.treeWidget->selectedItems().count();
    QPushButton *button = m_ui.buttonBox->button(QDialogButtonBox::Save);
    if (count == m_ui.treeWidget->topLevelItemCount()) {
        button->setEnabled(true);
        button->setText(tr("Save All"));
    } else if (count == 0) {
        button->setEnabled(false);
        button->setText(tr("Save"));
    } else {
        button->setEnabled(true);
        button->setText(tr("Save Selected"));
    }
}

void SaveItemsDialog::adjustButtonWidths()
{
    // give save button a size that all texts fit in, so it doesn't get resized
    // Mac: make cancel + save button same size (work around dialog button box issue)
    QStringList possibleTexts;
    possibleTexts << tr("Save") << tr("Save All");
    if (m_ui.treeWidget->topLevelItemCount() > 1)
        possibleTexts << tr("Save Selected");
    int maxTextWidth = 0;
    QPushButton *saveButton = m_ui.buttonBox->button(QDialogButtonBox::Save);
    foreach (const QString &text, possibleTexts) {
        saveButton->setText(text);
        int hint = saveButton->sizeHint().width();
        if (hint > maxTextWidth)
            maxTextWidth = hint;
    }
#ifdef Q_OS_MAC
    QPushButton *cancelButton = m_ui.buttonBox->button(QDialogButtonBox::Cancel);
    int cancelButtonWidth = cancelButton->sizeHint().width();
    if (cancelButtonWidth > maxTextWidth)
        maxTextWidth = cancelButtonWidth;
    cancelButton->setMinimumWidth(maxTextWidth);
#endif
    saveButton->setMinimumWidth(maxTextWidth);
}

void SaveItemsDialog::collectItemsToSave()
{
    m_itemsToSave.clear();
    foreach (QTreeWidgetItem *item, m_ui.treeWidget->selectedItems()) {
        m_itemsToSave.append(item->data(0, Qt::UserRole).value<IDocument*>());
    }
    accept();
}

void SaveItemsDialog::discardAll()
{
    m_ui.treeWidget->clearSelection();
    collectItemsToSave();
}

QList<IDocument*> SaveItemsDialog::itemsToSave() const
{
    return m_itemsToSave;
}

void SaveItemsDialog::setAlwaysSaveMessage(const QString &msg)
{
    m_ui.saveBeforeBuildCheckBox->setText(msg);
    m_ui.saveBeforeBuildCheckBox->setVisible(true);
}

bool SaveItemsDialog::alwaysSaveChecked()
{
    return m_ui.saveBeforeBuildCheckBox->isChecked();
}
