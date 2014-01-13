#pragma once
#include "datatype.h"
#include <QtCore/QtCore>

struct DataTypeInfo
{
	DataType	eDataType;
	QString		strDataTypeName;
};

const DataTypeInfo g_DataTypeInfo[] =
{
	{ DATATYPE_INT, QLatin1String("int") },
	{ DATATYPE_MATRIX, QLatin1String("matrix") },
	{ DATATYPE_HISTOGRAM, QLatin1String("histogram") },
	{ DATATYPE_GRAYIMAGE, QLatin1String("grayimage") },
	{ DATATYPE_8UC3IMAGE, QLatin1String("image_8uc3") },
	{ DATATYPE_STRING, QLatin1String("string") },
	{ DATATYPE_USERTYPE, QLatin1String("usertype") },
	{ DATATYPE_INVALID, QLatin1String("invalid") },
};