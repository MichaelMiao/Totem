#include "designnetdocument.h"
#include "designneteditor.h"

#include "designnetconstants.h"
#include "designnetspace.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializable.h"
#include <QFileInfo>
using namespace Core;
namespace DesignNet{

class DesignNetDocumentPrivate
{
public:
	DesignNetDocumentPrivate(){}
	~DesignNetDocumentPrivate();
    QString fileName;
    DesignNetEditor *editor;
	DesignNetSpace	*space;
    QString suffexType;
    bool bModified;
};

DesignNetDocumentPrivate::~DesignNetDocumentPrivate()
{
	if (space)
	{
		delete space;
		space = 0;
	}
}

//----------------------------------------------
DesignNetDocument::DesignNetDocument(DesignNetEditor *parent)
    : IDocument(parent),
      d(new DesignNetDocumentPrivate())
{
    d->editor       = parent;
    d->suffexType   = DesignNet::Constants::NETEDITOR_FILETYPE;
	d->bModified    = false;
	d->space		= new DesignNetSpace(0, this);
	connect(d->space, SIGNAL(modified()), this, SLOT(onModified()));
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
	d->space->setObjectName(newName);
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
	Utils::XmlSerializer x;
	x.serialize("DesignNetSpace", *(d->space));
	emit serialized(x);
	x.write(fileName);
    return true;
}

DesignNetSpace * DesignNetDocument::designNetSpace() const
{
	return d->space;
}

void DesignNetDocument::onModified()
{
	setModified(true);
}

bool DesignNetDocument::open( QString *errorString, const QString &fileName, const QString &realFileName )
{
	d->space->setObjectName(realFileName);
	Utils::XmlDeserializer deserializer(realFileName);
	deserializer.deserialize("DesignNetSpace", *(d->space));
	emit deserialized(deserializer);
	return IDocument::open(errorString, fileName, realFileName);
}

}//namespace DesignNet
