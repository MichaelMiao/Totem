﻿#ifndef FILEDIALOGPROPERTY_H
#define FILEDIALOGPROPERTY_H
#include "property.h"
#include "utils/fileutils.h"
#include <QStringList>
#include <QFileDialog>


namespace DesignNet
{
class PathDialogPropertyWidget;
class PathDialogProperty : public Property
{
	Q_OBJECT
	friend class PathDialogPropertyWidget;

	DECLARE_PROPERTY_TYPE(DesignNet::Property_Path)

public:

	QDir::Filters filters() const;
	explicit PathDialogProperty(const QString& id, QString openPath = "",
	                            QStringList nameFilters = QStringList(),
	                            QDir::Filters filters = QDir::AllEntries,
	                            bool bSinglePath = true,
	                            QObject* parent = 0);

	QList<Utils::Path> paths() const;
	void setPaths(const QList<Utils::Path>& pathlist);

	void setNameFilters(const QStringList& filters);
	QStringList nameFilters() const;

	QFileDialog::Option dialogOption() const;
	void setDialogOption(const QFileDialog::Option& option);

	void setSinglePath(bool single) { m_bSinglePath = single; }
	bool isSinglePath() const { return m_bSinglePath; }

protected:

	QList<Utils::Path>  m_paths;         //!< 用来存最终的路径
	QString             m_openPath;     //!< 默认打开路径
	QStringList         m_nameFilters;  //!< 文件filter
	QDir::Filters       m_filters;      //!< 类型：文件/文件夹
	QFileDialog::Option m_dialogOption;
	bool                m_bSinglePath;  //!< 单选路径
};
}

#endif // FILEDIALOGPROPERTY_H
