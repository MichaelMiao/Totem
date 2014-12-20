#include "stdafx.h"
#include "AnimantionMgr.h"
#include "animationdelegate.h"


AnimantionMgr::AnimantionMgr() : m_iCurIndex(-1)
{
}


AnimantionMgr::~AnimantionMgr()
{
}

AnimationInfo* AnimantionMgr::getAnimationById(const int iId)
{
	foreach(AnimationInfo* pInfo, m_aniVec)
	{
		if (pInfo->id == iId)
			return pInfo;
	}
	return NULL;
}

AnimationInfo* AnimantionMgr::getAnimationByIndex(const int iIndex)
{
	if (iIndex >= 0 && iIndex <= (int)m_aniVec.size())
		return m_aniVec[iIndex];
	
	return 0;
}

void AnimantionMgr::addAnimation(AnimationInfo* pAnimation)
{
	if (pAnimation->id == -1)
		pAnimation->id = m_aniVec.count();
	foreach(AnimationInfo* pInfo, m_aniVec)
	{
		if (pInfo->id == pAnimation->id)
			return ;
	}
	m_aniVec.push_back(pAnimation);
	emit animationAdded(pAnimation);
}

void AnimantionMgr::removeAnimation(const int iId)
{
	int iIndex = 0;
	for (int i = 0; i < m_aniVec.count(); i++)
	{
		if (m_aniVec[i]->id == iId)
		{
			AnimationInfo* pInfo = m_aniVec[i];
			m_aniVec.remove(i);
			emit animationRemoved(pInfo);
			delete pInfo;
			return;
		}
	}
}

int AnimantionMgr::rowCount(const QModelIndex & parent) const
{
	return m_aniVec.size();
}

QVariant AnimantionMgr::data(const QModelIndex & index, int role) const
{
	if (index.row() < m_aniVec.size() && index.column() < columnCount())
	{
		AnimationInfo* pInfo = m_aniVec.at(index.row());
		switch ((AnimationEditWidget)index.column())
		{
		case AniEditWidget_Name:
			return pInfo->strName;
		case AniEditWidget_TimeFrom:
			return pInfo->iTimeFrom;
		case AniEditWidget_Duration:
			return pInfo->iDuration;
		case AniEditWidget_PosFromX:
			return pInfo->ptPosFrom.x();
		case AniEditWidget_PosFromY:
			return pInfo->ptPosFrom.y();
		case AniEditWidget_PosToX:
			return pInfo->ptPosTo.x();
		case AniEditWidget_PosToY:
			return pInfo->ptPosTo.y();
		case AniEditWidget_CurvePosX:
			return pInfo->m_curvePosX;
		case AniEditWidget_CurvePosY:
			return pInfo->m_curvePosY;
		case AniEditWidget_AlphaFrom:
			return pInfo->iAlphaFrom;
		case AniEditWidget_AlphaTo:
			return pInfo->iAlphaTo;
		case AniEditWidget_CurveAlpha:
			return pInfo->m_curveAlpha;
		case AniEditWidget_ScaleFrom:
			return pInfo->iScaleFrom;
		case AniEditWidget_ScaleTo:
			return pInfo->iScaleTo;
		case AniEditWidget_CurveScale:
			return pInfo->m_curveScale;
		}
	}
	return QVariant();
}

int AnimantionMgr::columnCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
	return AniEditWidget_End;
}

bool AnimantionMgr::submit()
{
	return true;
}

void AnimantionMgr::onCurrentAnimationChanged(int iIndex)
{
	m_iCurIndex = iIndex;
}

bool AnimantionMgr::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
	AnimationInfo* pInfo = m_aniVec.at(index.row());
	switch (index.column())
	{
	case AniEditWidget_Name:
		pInfo->strName = value.toString();
		break;
	case AniEditWidget_PosFromX:
		pInfo->ptPosFrom.setX(value.toInt());
		break;
	case AniEditWidget_PosFromY:
		pInfo->ptPosFrom.setY(value.toInt());
		break;
	case AniEditWidget_PosToX:
		pInfo->ptPosTo.setX(value.toInt());
		break;
	case AniEditWidget_PosToY:
		pInfo->ptPosTo.setY(value.toInt());
		break;
	case AniEditWidget_AlphaFrom:
		pInfo->iAlphaFrom = value.toInt();
		break;
	case AniEditWidget_AlphaTo:
		pInfo->iAlphaTo = value.toInt();
		break;
	case AniEditWidget_TimeFrom:
		pInfo->iTimeFrom = value.toInt();
		break;
	case AniEditWidget_Duration:
		pInfo->iDuration = value.toInt();
		break;
	case AniEditWidget_ScaleFrom:
		pInfo->iScaleFrom = value.toInt();
		break;
	case AniEditWidget_ScaleTo:
		pInfo->iScaleTo = value.toInt();
		break;
	case AniEditWidget_CurvePosX:
	{
		pInfo->m_curvePosX = value.toEasingCurve();
	}
		break;
	case AniEditWidget_CurvePosY:
		pInfo->m_curvePosY = value.toEasingCurve();
		break;
	case AniEditWidget_CurveAlpha:
		pInfo->m_curveAlpha = value.toEasingCurve();
		break;
	case AniEditWidget_CurveScale:
	{
		pInfo->m_curveScale = value.toEasingCurve();
	}
		break;
	default:
		break;
	}
	emit onAnimationChanged();
	return true;
}
