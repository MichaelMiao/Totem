#ifndef EASINGCURVEMODEL_H
#define EASINGCURVEMODEL_H

#include <QAbstractListModel>
#include <QEasingCurve>
#define ICON_SIZE  50

class SplineEditor;
class EasingCurveModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EasingCurveModel(QObject *parent);
	~EasingCurveModel();

	void createEasingCurveIcon();
	// override
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	SplineEditor* GetCustomEditor() { return m_pSplineEditor; }

public slots:
	
	void onCustomEasingCurveChanged();

private:
	
	QVector<QIcon>	m_vecPixmap;
	QEasingCurve	m_customCurve;
	SplineEditor*	m_pSplineEditor;
};

#endif // EASINGCURVEMODEL_H
