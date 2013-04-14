#include "designnetdocument.h"
#include "designneteditor.h"

#include <QFileInfo>
#include "designnetconstants.h"
using namespace Core;
namespace DesignNet{

struct DesignNetDocumentPrivate
{
    QString fileName;
    DesignNetEditor *editor;
    QString suffexType;
    bool bModified;
};

//----------------------------------------------
DesignNetDocument::DesignNetDocument(DesignNetEditor *parent)
    : IDocument(parent),
      d(new DesignNetDocumentPrivate())
{
    d->editor       = parent;
    d->suffexType   = DesignNet::Constants::NETEDITOR_FILETYPE;
    d->bModified    = false;
}

DesignNetDocument::~DesignNetDocument()
{
    if(d)
    {
        delete d;
        d = 0;
    }
}

QString DesignNetDocument::defaultPath() const
{
    return QString();
}

QString DesignNetDocument::suggestedFileName() const
{
    return QString();
}

QString DesignNetDocument::suffixType() const
{
    return d->suffexType;
}

bool DesignNetDocument::shouldAutoSave() const
{
    return true;
}

bool DesignNetDocument::isModified() const
{
    return d->bModified;
}

bool DesignNetDocument::isSaveAsAllowed() const
{
    return true;
}

void DesignNetDocument::rename(const QString &newName)
{
    const QString oldFilename = d->fileName;
    d->fileName = newName;
    d->editor->setDisplayName(QFileInfo(d->fileName).fileName());
    emit fileNameChanged(oldFilename, newName);
    emit changed();
}

void DesignNetDocument::setModified(const bool &bModified)
{
    if(bModified == d->bModified)
        return;
    d->bModified = bModified;
    emit changed();
}

bool DesignNetDocument::reload(QString *errorString, IDocument::ReloadFlag flag, IDocument::ChangeType type)
{
    if (flag == FlagIgnore)
        return true;
    if (type == TypePermissions)
    {
        emit changed();
        return true;
    }
    return d->editor->open(errorString, d->fileName, d->fileName);
}

bool DesignNetDocument::save(QString *errorString, const QString &fileName, bool autoSave)
{
    return true;
}

}//namespace DesignNet
