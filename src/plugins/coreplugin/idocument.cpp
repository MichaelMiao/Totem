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
#include "idocument.h"
#include "infobar.h"
#include <QTextDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>
namespace Core{

IDocument::IDocument(QObject *parent) :
    QObject(parent),
    m_infoBar(0),
    m_hasWriteWarning(false),
    m_restored(false),
    m_document(new QTextDocument(this))
{
}

IDocument::~IDocument()
{
    removeAutoSaveFile();
    delete m_infoBar;
}

QString IDocument::fileName() const
{
    return m_fileName;
}

bool IDocument::isFileReadOnly() const
{
    if(fileName().isEmpty())
        return false;
    return !QFileInfo(fileName()).isWritable();
}

bool IDocument::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    QString title = tr("untitled");
    if (!fileName.isEmpty())
    {
        const QFileInfo fi(fileName);
        m_bReadOnly = !fi.isWritable();
        m_fileName = QDir::cleanPath(fi.absoluteFilePath());

        m_document->setModified(false);

        m_document->setModified(fileName != realFileName);
		emit titleChanged(m_fileName);
        emit changed();
    }
    return true;
}

bool IDocument::reload(QString *errorString)
{
    emit aboutToReload();
    bool success = open(errorString, m_fileName, m_fileName);
    emit reloaded();
    return success;
}

bool IDocument::shouldAutoSave() const
{
    return false;
}

IDocument::ReloadBehavior IDocument::reloadBehavior(IDocument::ChangeTrigger state,
                                                    IDocument::ChangeType type) const
{
    if(type == TypePermissions)
        return BehaviorSilent;//如果是权限改变，就不用做什么改变
    if(type == TypeContents //如果是内部改变内容就不需要问用户了
            && state == TriggerInternal
            && !isModified())
    {
        return BehaviorSilent;
    }
    return BehaviorAsk;
}

void IDocument::checkPermissions()
{
}
//调用自动保存
bool IDocument::autoSave(QString *errorString, const QString &fileName)
{
    if (!save(errorString, fileName, true))
        return false;
    m_autoSaveName = fileName;
    return true;
}

static const char kRestoredAutoSave[] = "RestoredAutoSave";

void IDocument::setRestoredFrom(const QString &name)
{
    m_autoSaveName = name;
    m_restored = true;
    InfoBarEntry info(QLatin1String(kRestoredAutoSave),
          tr("File was restored from auto-saved copy. "
             "Use <i>Save</i> to confirm, or <i>Revert to Saved</i> to discard changes."));
    infoBar()->addInfo(info);
}

//移除自动保存的临时文件
void IDocument::removeAutoSaveFile()
{
    if(!m_autoSaveName.isEmpty())
    {
        QFile::remove(m_autoSaveName);
        m_autoSaveName.clear();
        if(m_restored)
        {
            m_restored = false;
            infoBar()->removeInfo(QLatin1String(kRestoredAutoSave));
        }
    }
}

InfoBar *IDocument::infoBar()
{
    if(!m_infoBar)
        m_infoBar = new InfoBar;
    return m_infoBar;
}


}
