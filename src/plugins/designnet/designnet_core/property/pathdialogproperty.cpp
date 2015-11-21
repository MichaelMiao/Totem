#include "stdafx.h"
#include "pathdialogproperty.h"
#include <QFileInfo>

using namespace Utils;
namespace DesignNet
{

PathDialogProperty::PathDialogProperty(const QString& id,
                                       QString openPath,
                                       QStringList nameFilters,
                                       QDir::Filters filters,
                                       bool bSinglePath,
                                       QObject* parent) :
	Property(id, "PathDialogProperty", parent),
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

void PathDialogProperty::setPaths(const QList<Utils::Path>& pathlist)
{
	m_paths = pathlist;
	emit changed();
}

void PathDialogProperty::setNameFilters(const QStringList& filters)
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

void PathDialogProperty::setDialogOption(const QFileDialog::Option& option)
{
	m_dialogOption = option;
}

void PathDialogProperty::serialize(Utils::XmlSerializer& s) const
{
	Property::serialize(s);
	s.serialize("Paths", m_paths, "Path");
}

void PathDialogProperty::deserialize(Utils::XmlDeserializer& s)
{
	QList<Utils::Path> pathList;
	s.deserializeCollection("Paths", m_paths, "Path");
}

}
