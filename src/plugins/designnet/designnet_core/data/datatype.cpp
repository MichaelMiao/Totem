#pragma once
#include "datatype.h"
#include <QtCore/QtCore>
#include "designnetconstants.h"
using namespace DesignNet;

/// Data Type Image
const InnerDataType g_InnerDataType[] =
{
	{ DATATYPE_INT,			":/media/intdata.png" },
	{ DATATYPE_RECT,		"" },
	{ DATATYPE_POINT,		"" },
	{ DATATYPE_MATRIX,		":/media/matrix.jpg" },
	{ DATATYPE_HISTOGRAM,	":/media/histogramimage.png" },
	{ DATATYPE_GRAYIMAGE,	":/media/grayimages.jpg" },
	{ DATATYPE_8UC3IMAGE,	":/media/colorimage.png" },
	{ DATATYPE_BINARYIMAGE, ":/media/binaryimage.png" },
	{ DATATYPE_STRING,		":/media/textimage.png" },
};