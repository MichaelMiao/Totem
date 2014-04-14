#pragma once
#include <QMetaType>
#include "opencv2/opencv.hpp"


enum DataType
{
	DATATYPE_INT,
	DATATYPE_MATRIX,
	DATATYPE_HISTOGRAM,
	DATATYPE_GRAYIMAGE,
	DATATYPE_8UC3IMAGE,
	DATATYPE_BINARYIMAGE,
	END_DATATYPE_MATRIX,

	DATATYPE_STRING,
	DATATYPE_USERTYPE,
	DATATYPE_INVALID,
};

struct InnerDataType{
	DataType	dt;
	char*		strFile;
};
extern const InnerDataType g_InnerDataType[];

enum ERROR_CODE
{
	ERROR_,
};

Q_DECLARE_METATYPE(cv::Mat)