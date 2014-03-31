#include "stdafx.h"
#include "LabelLoader.h"
#include "../../../designnet/designnet_core/property/pathdialogproperty.h"
#include "Utils/fileutils.h"
#include "../../designnet_core/property/property.h"


static const char INPORT_FILENAME[] = "File Name";
static const char OUTPORT_DATALABEL[] = "Data Label";

using namespace DesignNet;
namespace InputLoader{

LabelLoader::LabelLoader(DesignNet::DesignNetSpace *space, QObject *parent)
{
	setName(tr("Label File Loader"));
	addPort(Port::IN_PORT, DATATYPE_STRING, INPORT_FILENAME);
	addPort(Port::OUT_PORT, DATATYPE_INT, OUTPORT_DATALABEL);

	QStringList fileTypes;
	fileTypes << "*.label";
	PathDialogProperty* pProperty = new PathDialogProperty("Path", "", fileTypes, QDir::Files, true, this);
	addProperty(pProperty);
}

LabelLoader::~LabelLoader(void)
{

}

Processor* LabelLoader::create(DesignNet::DesignNetSpace *space) const
{
	return new LabelLoader(space);
}

QString LabelLoader::title() const
{
	return tr("Label File Loader");
}

QString LabelLoader::category() const
{
	return tr("Loader");
}

bool LabelLoader::prepareProcess()
{
	return true;
}

bool LabelLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	Port* pPort = getPort(Port::IN_PORT, INPORT_FILENAME);
	QString fileName = pPort->data()->variant.toString();
	int iId = fileName.toInt();
	int iLabel = m_mapLabel.value(iId);
	pushData(QVariant::fromValue(iLabel), DATATYPE_INT, OUTPORT_DATALABEL);
	return true;
}

bool LabelLoader::finishProcess()
{
	return true;
}

void LabelLoader::setPath(const QString &p)
{

}

QString LabelLoader::path() const
{
	QList<Utils::Path> listPath;
	PathDialogProperty* pProperty = (PathDialogProperty*)getProperty("Path");
	listPath = pProperty->paths();
	if (listPath.count() > 0)
		return listPath.at(0).m_path;
	return tr("");
}

void LabelLoader::propertyChanged(Property *prop)
{
	PathDialogProperty* pProperty = (PathDialogProperty*)prop;
	if (pProperty)
	{
		QList<Utils::Path> listPath;
		listPath = pProperty->paths();
		if (listPath.count() > 0)
		{
			QFile fileLabel(listPath.at(0).m_path);
			if (!fileLabel.open(QIODevice::ReadOnly))
				return;
			QTextStream s(&fileLabel);
			int id, iLabel;
			while (!s.atEnd())
			{
				QString str = s.readLine();
				QTextStream sTemp(&str);
				sTemp >> id >> iLabel;
				m_mapLabel[id] = iLabel;
			}
			fileLabel.close();
		}
	}
	Processor::propertyChanged(prop);
}

}
