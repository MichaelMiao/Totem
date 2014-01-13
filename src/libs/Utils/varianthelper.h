#pragma once
#include <QVariant>

/*!
	���ָ�뵽QVariant���໥ת��
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