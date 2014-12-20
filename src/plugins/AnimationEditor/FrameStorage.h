// Author: miaojinquan
#pragma once


class AnimationControlPane;
class FrameInfoWidget;
class AniPreviewWnd;
class AniMainWindow;
struct FrameStorageStruct
{
	FrameStorageStruct() : m_pMainWnd(0), m_pPreviewWnd(0), m_pFrameInfo(0), m_pCtrlPane(0)
	{
	}
	AniMainWindow*		m_pMainWnd;
	AniPreviewWnd*		m_pPreviewWnd;
	FrameInfoWidget*	m_pFrameInfo;
	AnimationControlPane* m_pCtrlPane;
};

#define FRAME_STORAGE_STRUCT_PROPERTY	"FrameStorageSturct"

class FSUtil
{
public:

	static FrameStorageStruct* FS(QObject* pObj)
	{
		QObject* pTemp = pObj;
		while (pTemp->parent())
			pTemp = pTemp->parent();
		if (pTemp)
		{
			FrameStorageStruct* pValue = (FrameStorageStruct*)pTemp->property(FRAME_STORAGE_STRUCT_PROPERTY).value<void*>();
			return pValue;
		}
		return NULL;
	}

	FrameStorageStruct* FS() { return m_pFS; }
	FSUtil(FrameStorageStruct* pFS) { m_pFS = pFS; }

private:

	FrameStorageStruct* m_pFS;
};
