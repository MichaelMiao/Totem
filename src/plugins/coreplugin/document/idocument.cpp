#include "stdafx.h"
#include "idocument.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextDocument>


namespace Core{

IDocument::IDocument(QObject *parent) :
    QObject(parent),
    m_hasWriteWarning(false),
    m_restored(false),
    m_document(new QTextDocument(this))
{
}

IDocument::~IDocument()
{
    removeAutoSaveFile();
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
		title = fi.fileName();
        m_fileName = QDir::cleanPath(fi.absoluteFilePath());

        m_document->setModified(false);

        m_document->setModified(fileName != realFileName);
		emit titleChanged(title);
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
        return BehaviorSilent;
    if(type == TypeContents
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
}

void IDocument::removeAutoSaveFile()
{
    if(!m_autoSaveName.isEmpty())
    {
        QFile::remove(m_autoSaveName);
        m_autoSaveName.clear();
        if(m_restored)
            m_restored = false;
    }
}
}
