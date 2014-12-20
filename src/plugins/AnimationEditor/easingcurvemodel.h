#ifndef EASINGCURVEMODEL_H
#define EASINGCURVEMODEL_H

#include <QAbstractListModel>
#include <QEasingCurve>
#define ICON_SIZE  50
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

public slots:
	
	void onCustomEasingCurveChanged();

private:
	
	QVector<QIcon>	m_vecPixmap;
	QEasingCurve	m_customCurve;
};

#endif // EASINGCURVEMODEL_H
