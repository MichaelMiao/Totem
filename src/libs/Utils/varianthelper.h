#pragma once
#include <QVariant>

/*!
	完成指针到QVariant的相互转换
*/
template <class T>
class VariantPointCvt
{
public:

	static QVariant fromPtr(T* pData)
	{
		return qVariantFromValue((void*)pData);
	}

	static T* toPtr(QVariant var)
	{
		return (T*)var.value<void*>();
	}
};