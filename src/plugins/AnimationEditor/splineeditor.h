#ifndef SPLINEEDITOR_H
#define SPLINEEDITOR_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QScrollArea>

#include <QEasingCurve>
#include <QHash>


class EasingCurveModel;
class SplineEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QEasingCurve easingCurve READ easingCurve WRITE setEasingCurve NOTIFY easingCurveChanged);

public:

	explicit SplineEditor(QWidget *parent = 0);
    void setControlPoint(int index, const QPointF &point)
    {
        m_controlPoints[index] = point;
        update();
    }

    void setSmooth(int index, bool smooth)
    {
        m_smoothAction->setChecked(smooth);
        smoothPoint(index * 3 + 2);
        //update();
    }
	
	QEasingCurve easingCurve() const
	{
		return m_easingCurve;
	}


signals:
    void easingCurveChanged();
    void easingCurveCodeChanged(const QString &code);


public slots:
    void setEasingCurve(const QEasingCurve &easingCurve);
    void setEasingCurve(const QString &code);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);

    void invalidate();
    void invalidateSmoothList();

private:
    int findControlPoint(const QPoint &point);
    bool isSmooth(int i) const;

    void smoothPoint( int index);
    void cornerPoint( int index);
    void deletePoint(int index);
    void addPoint(const QPointF point);

    bool isControlPointSmooth(int i) const;

    QEasingCurve m_easingCurve;
    QVector<QPointF> m_controlPoints;
    QVector<bool> m_smoothList;
    int m_numberOfSegments;
    int m_activeControlPoint;
    bool m_mouseDrag;
    QPoint m_mousePress;
    QHash<QString, QEasingCurve> m_presets;

    QMenu *m_pointContextMenu;
    QMenu *m_curveContextMenu;
    QAction *m_deleteAction;
    QAction *m_smoothAction;
    QAction *m_cornerAction;
    QAction *m_addPoint;
    bool m_block;
};

#endif // SPLINEEDITOR_H
