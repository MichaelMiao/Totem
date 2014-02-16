#include "LabelLoader.h"


namespace InputLoader{

LabelLoader::LabelLoader(DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/)
{
	setName(tr("Image Folder Loader"));
	setIcon(QLatin1String(ImageFolderIcon));
	addPort(Port::OUT_PORT, DATATYPE_IMAGE, OUTPUT_IMAGE);
	addPort(Port::OUT_PORT, DATATYPE_STRING, OUTPUT_FILE_PATH);
}

LabelLoader::~LabelLoader(void)
{

}

Processor* LabelLoader::create(DesignNet::DesignNetSpace *space) const
{

}

QString LabelLoader::title() const
{

}

QString LabelLoader::category() const
{

}

bool LabelLoader::prepareProcess()
{

}

bool LabelLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{

}

bool LabelLoader::finishProcess()
{

}

void LabelLoader::setPath(const QString &p)
{

}

QString LabelLoader::path() const
{

}

void LabelLoader::serialize(Utils::XmlSerializer& s) const
{

}

void LabelLoader::deserialize(Utils::XmlDeserializer& s)
{

}

}


