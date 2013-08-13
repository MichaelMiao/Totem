#include "pathdialogproperty.h"
#include "designnetconstants.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmldeserializer.h"
#include <QFileInfo>

using namespace Utils;
namespace DesignNet{

PathDialogProperty::PathDialogProperty(const QString &id,
                                       QString openPath,
                                       QStringList nameFilters,
                                       QDir::Filters filters,
                                       bool bSinglePath,
                                       QObject *parent) :
    Property(id, "PathDialogProperty",parent),
    m_filters(filters),
    m_openPath(openPath),
    m_nameFilters(nameFilters),
    m_bSinglePath(bSinglePath)
{
}

QList<Utils::Path> PathDialogProperty::paths() const
{
    return m_paths;
}

void PathDialogProperty::setPaths(const QList<Utils::Path> &pathlist)
{
    m_paths = pathlist;
	emit changed();
}

void PathDialogProperty::setNameFilters(const QStringList &filters)
{
    m_nameFilters = filters;
}

QStringList PathDialogProperty::nameFilters() const
{
    return m_nameFilters;
}

QFileDialog::Option PathDialogProperty::dialogOption() const
{
    return m_dialogOption;
}

void PathDialogProperty::setDialogOption(const QFileDialog::Option &option)
{
    m_dialogOption = option;
}

bool PathDialogProperty::isValid() const
{

    foreach(Path path, m_paths)
    {
        QFileInfo info(path.m_path);
        if(!info.exists())
            return false;
    }

    return true;
}

Core::Id PathDialogProperty::typeID() const
{
    return DesignNet::Constants::PROPERTY_TYPE_PATHDIALOG;
}

QDir::Filters PathDialogProperty::filters() const
{
    return m_filters;
}

void PathDialogProperty::serialize( Utils::XmlSerializer& s ) const
{
	Property::serialize(s);
	QList<Utils::Path> pathList = paths();
	foreach(Utils::Path path, pathList)
	{
		s.serialize("Path", path);
	}
}

void PathDialogProperty::deserialize( Utils::XmlDeserializer& s )const
{

}

}
