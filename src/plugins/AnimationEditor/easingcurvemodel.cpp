#include "stdafx.h"
#include "easingcurvemodel.h"
#include <QEasingCurve>
#include <QIcon>
#include <QPainter>
#include "splineeditor.h"


EasingCurveModel::EasingCurveModel(QObject *parent)
	: QAbstractListModel(parent), m_customCurve(QEasingCurve::Custom)
{
	createEasingCurveIcon();
	m_pSplineEditor = new SplineEditor();
	connect(m_pSplineEditor, SIGNAL(easingCurveChanged()), this, SLOT(onCustomEasingCurveChanged()));
}

EasingCurveModel::~EasingCurveModel()
{
	delete m_pSplineEditor;
	m_pSplineEditor = NULL;
}

int EasingCurveModel::rowCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
	return m_vecPixmap.size();
}

static QEasingCurve createEasingCurve(QEasingCurve::Type curveType)
{
	QEasingCurve curve(curveType);

	if (curveType == QEasingCurve::BezierSpline)
	{
		curve.addCubicBezierSegment(QPointF(0.4, 0.1), QPointF(0.6, 0.9), QPointF(1.0, 1.0));
	}
	else if (curveType == QEasingCurve::TCBSpline)
	{
		curve.addTCBSegment(QPointF(0.0, 0.0), 0, 0, 0);
		curve.addTCBSegment(QPointF(0.3, 0.4), 0.2, 1, -0.2);
		curve.addTCBSegment(QPointF(0.7, 0.6), -0.2, 1, 0.2);
		curve.addTCBSegment(QPointF(1.0, 1.0), 0, 0, 0);
	}

	return curve;
}

void EasingCurveModel::createEasingCurveIcon()
{
	QPixmap pix(ICON_SIZE, ICON_SIZE);
	QPainter painter(&pix);
	QLinearGradient gradient(0, 0, 0, ICON_SIZE);
	gradient.setColorAt(0.0, QColor(240, 240, 240));
	gradient.setColorAt(1.0, QColor(224, 224, 224));
	QBrush brush(gradient);
	const QMetaObject &mo = QEasingCurve::staticMetaObject;
	QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
	for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i)
	{
		painter.fillRect(QRect(QPoint(0, 0), QSize(ICON_SIZE, ICON_SIZE)), brush);
		QEasingCurve curve = createEasingCurve((QEasingCurve::Type)i);
		painter.setPen(QColor(0, 0, 255, 64));
		qreal xAxis = ICON_SIZE / 1.5;
		qreal yAxis = ICON_SIZE / 3;
		painter.drawLine(0, xAxis, ICON_SIZE, xAxis);
		painter.drawLine(yAxis, 0, yAxis, ICON_SIZE);

		qreal curveScale = ICON_SIZE / 2;

		painter.setPen(Qt::NoPen);

		// start point
		painter.setBrush(Qt::red);
		QPoint start(yAxis, xAxis - curveScale * curve.valueForProgress(0));
		painter.drawRect(start.x() - 1, start.y() - 1, 3, 3);

		// end point
		painter.setBrush(Qt::blue);
		QPoint end(yAxis + curveScale, xAxis - curveScale * curve.valueForProgress(1));
		painter.drawRect(end.x() - 1, end.y() - 1, 3, 3);

		QPainterPath curvePath;
		curvePath.moveTo(start);
		for (qreal t = 0; t <= 1.0; t += 1.0 / curveScale)
		{
			QPoint to;
			to.setX(yAxis + curveScale * t);
			to.setY(xAxis - curveScale * curve.valueForProgress(t));
			curvePath.lineTo(to);
		}
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.strokePath(curvePath, QColor(32, 32, 32));
		painter.setRenderHint(QPainter::Antialiasing, false);
		m_vecPixmap.push_back(QIcon(pix));
	}
	m_vecPixmap.push_back(QIcon(":/AnimationEditor/Resources/more_easingcurve.png"));
}

QVariant EasingCurveModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DecorationRole)
	{
		return m_vecPixmap.at(index.row());
	}
	else if (role == Qt::UserRole)
	{
		if (index.row() < int(QEasingCurve::NCurveTypes - 1))
		{
			return QEasingCurve((QEasingCurve::Type)index.row());
		}
		else if (index.row() == QEasingCurve::Custom)
		{
			return m_customCurve;
		}
	}
	
	return QVariant();
}

void EasingCurveModel::onCustomEasingCurveChanged()
{
	SplineEditor* pEditor = qobject_cast<SplineEditor*>(sender());
	m_customCurve = pEditor->easingCurve();
	emit dataChanged(index(QEasingCurve::Custom, 0), index(QEasingCurve::Custom, 0));
}
