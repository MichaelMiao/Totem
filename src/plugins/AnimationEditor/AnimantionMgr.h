#pragma once
#include <QAbstractTableModel>
#include <QEasingCurve>


class AnimationInfo
{
public:

	AnimationInfo()
		: id(0), strName(""),
		ptPosFrom(0, 0), ptPosTo(0, 0), iAlphaFrom(255), iAlphaTo(255),
		iScaleFrom(100), iScaleTo(100), iTimeFrom(0), iDuration(100),
		m_curvePosX(QEasingCurve::Linear),
		m_curvePosY(QEasingCurve::Linear),
		m_curveAlpha(QEasingCurve::Linear),
		m_curveScale(QEasingCurve::Linear)
	{
	}

	int		id;
	QString strName;
	QPoint	ptPosFrom;
	QPoint	ptPosTo;
	QEasingCurve m_curvePosX;
	QEasingCurve m_curvePosY;
	int		iAlphaFrom;
	int		iAlphaTo;
	QEasingCurve m_curveAlpha;
	int		iScaleFrom;
	int		iScaleTo;
	QEasingCurve m_curveScale;
	int		iTimeFrom;
	int		iDuration;
};

class AnimantionMgr : public QAbstractTableModel
{
	Q_OBJECT

public:

	static  AnimantionMgr* instance()
	{
		static AnimantionMgr mgr;
		return &mgr;
	}
	~AnimantionMgr();

	void addAnimation(AnimationInfo* pAnimation);
	void removeAnimation(const int iId);
	AnimationInfo* getAnimationById(const int iId);
	AnimationInfo* getAnimationByIndex(const int iIndex);
	QVector<AnimationInfo*> getAllAnimation() { return m_aniVec; }

	
	// override
	int			columnCount(const QModelIndex & parent = QModelIndex()) const;
	int			rowCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant	data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	bool		setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

signals:

	void animationAdded(AnimationInfo*);
	void animationRemoved(AnimationInfo*);
	void onAnimationChanged();

public slots:
	
	bool submit();
	void onCurrentAnimationChanged(int iIndex);

private:

	AnimantionMgr();

	QVector<AnimationInfo*> m_aniVec;
	int						m_iCurIndex;
};
