#include "stdafx.h"
#include "splineeditor.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDebug>
#include <QApplication>

const int canvasWidth = 500;
const int canvasHeight = 250;

const int canvasMargin = 160;

SplineEditor::SplineEditor(QWidget *parent) :
    QWidget(parent), m_block(false)
{
    setFixedSize(canvasWidth + canvasMargin * 2, canvasHeight + canvasMargin * 2);

    m_controlPoints.append(QPointF(0.4, 0.075));
    m_controlPoints.append(QPointF(0.45,0.24));
    m_controlPoints.append(QPointF(0.5,0.5));

    m_controlPoints.append(QPointF(0.55,0.76));
    m_controlPoints.append(QPointF(0.7,0.9));
    m_controlPoints.append(QPointF(1.0, 1.0));

    m_numberOfSegments = 2;

    m_activeControlPoint = -1;

    m_mouseDrag = false;

    m_pointContextMenu = new QMenu(this);
    m_deleteAction = new QAction(tr("Delete point"), m_pointContextMenu);
    m_smoothAction = new QAction(tr("Smooth point"), m_pointContextMenu);
    m_cornerAction = new QAction(tr("Corner point"), m_pointContextMenu);

    m_smoothAction->setCheckable(true);

    m_pointContextMenu->addAction(m_deleteAction);
    m_pointContextMenu->addAction(m_smoothAction);
    m_pointContextMenu->addAction(m_cornerAction);

    m_curveContextMenu = new QMenu(this);

    m_addPoint = new QAction(tr("Add point"), m_pointContextMenu);

    m_curveContextMenu->addAction(m_addPoint);

    invalidateSmoothList();
}

static inline QPointF mapToCanvas(const QPointF &point)
{
    return QPointF(point.x() * canvasWidth + canvasMargin,
                   canvasHeight - point.y() * canvasHeight + canvasMargin);
}

static inline QPointF mapFromCanvas(const QPointF &point)
{
    return QPointF((point.x() - canvasMargin) / canvasWidth ,
                   1 - (point.y() - canvasMargin) / canvasHeight);
}

static inline void paintControlPoint(const QPointF &controlPoint, QPainter *painter, bool edit,
                                     bool realPoint, bool active, bool smooth)
{
    int pointSize = 4;

    if (active)
        painter->setBrush(QColor(140, 140, 240, 255));
    else
        painter->setBrush(QColor(120, 120, 220, 255));

    if (realPoint)
	{
        pointSize = 6;
        painter->setBrush(QColor(80, 80, 210, 150));
    }

    painter->setPen(QColor(50, 50, 50, 140));

    if (!edit)
        painter->setBrush(QColor(160, 80, 80, 250));

    if (smooth)
	{
        painter->drawEllipse(QRectF(mapToCanvas(controlPoint).x() - pointSize + 0.5,
                                    mapToCanvas(controlPoint).y() - pointSize + 0.5,
                                    pointSize * 2, pointSize * 2));
    }
	else
	{
        painter->drawRect(QRectF(mapToCanvas(controlPoint).x() - pointSize + 0.5,
                                 mapToCanvas(controlPoint).y() - pointSize + 0.5,
                                 pointSize * 2, pointSize * 2));
    }
}

static inline bool indexIsRealPoint(int i)
{
    return  !((i + 1) % 3);
}

static inline int pointForControlPoint(int i)
{
    if ((i % 3) == 0)
        return i - 1;

    if ((i % 3) == 1)
        return i + 1;

    return i;
}

void drawCleanLine(QPainter *painter, const QPoint p1, QPoint p2)
{
    painter->drawLine(p1 + QPointF(0.5 , 0.5), p2 + QPointF(0.5, 0.5));
}

void SplineEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.fillRect(0,0,width() - 1, height() - 1, QBrush(Qt::white));
    painter.drawRect(0,0,width() - 1, height() - 1);

    painter.setRenderHint(QPainter::Antialiasing);

    pen = QPen(Qt::gray);
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    drawCleanLine(&painter,mapToCanvas(QPoint(0, 0)).toPoint(), mapToCanvas(QPoint(1, 0)).toPoint());
    drawCleanLine(&painter,mapToCanvas(QPoint(0, 1)).toPoint(), mapToCanvas(QPoint(1, 1)).toPoint());

    for (int i = 0; i < m_numberOfSegments; i++)
	{
        QPainterPath path;
        QPointF p0;

        if (i == 0)
            p0 = mapToCanvas(QPointF(0.0, 0.0));
        else
            p0 = mapToCanvas(m_controlPoints.at(i * 3 - 1));

        path.moveTo(p0);

        QPointF p1 = mapToCanvas(m_controlPoints.at(i * 3));
        QPointF p2 = mapToCanvas(m_controlPoints.at(i * 3 + 1));
        QPointF p3 = mapToCanvas(m_controlPoints.at(i * 3 + 2));
        path.cubicTo(p1, p2, p3);
        painter.strokePath(path, QPen(QBrush(Qt::black), 2));

        QPen pen(Qt::black);
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawLine(p0, p1);
        painter.drawLine(p3, p2);
    }

    paintControlPoint(QPointF(0.0, 0.0), &painter, false, true, false, false);
    paintControlPoint(QPointF(1.0, 1.0), &painter, false, true, false, false);

    for (int i = 0; i < m_controlPoints.count() - 1; ++i)
        paintControlPoint(m_controlPoints.at(i),
                          &painter,
                          true,
                          indexIsRealPoint(i),
                          i == m_activeControlPoint,
                          isControlPointSmooth(i));
}

void SplineEditor::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_activeControlPoint = findControlPoint(e->pos());

        if (m_activeControlPoint != -1) {
            mouseMoveEvent(e);
        }
        m_mousePress = e->pos();
        e->accept();
    }
}

void SplineEditor::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_activeControlPoint = -1;

        m_mouseDrag = false;
        e->accept();
    }
}

void SplineEditor::contextMenuEvent(QContextMenuEvent *e)
{
    int index = findControlPoint(e->pos());

    if (index > 0 && indexIsRealPoint(index))
	{
        m_smoothAction->setChecked(isControlPointSmooth(index));
        QAction* action = m_pointContextMenu->exec(e->globalPos());
        if (action == m_deleteAction)
            deletePoint(index);
        else if (action == m_smoothAction)
            smoothPoint(index);
        else if (action == m_cornerAction)
            cornerPoint(index);
    } else {
        QAction* action = m_curveContextMenu->exec(e->globalPos());
        if (action == m_addPoint)
            addPoint(e->pos());
    }
}

void SplineEditor::invalidate()
{
    QEasingCurve easingCurve(QEasingCurve::BezierSpline);

    for (int i = 0; i < m_numberOfSegments; ++i) {
        easingCurve.addCubicBezierSegment(m_controlPoints.at(i * 3),
                                          m_controlPoints.at(i * 3 + 1),
                                          m_controlPoints.at(i * 3 + 2));
    }
    setEasingCurve(easingCurve);
}

void SplineEditor::invalidateSmoothList()
{
    m_smoothList.clear();

    for (int i = 0; i < (m_numberOfSegments - 1); ++i)
        m_smoothList.append(isSmooth(i * 3 + 2));

}

int SplineEditor::findControlPoint(const QPoint &point)
{
    int pointIndex = -1;
    qreal distance = -1;
    for (int i = 0; i<m_controlPoints.size() - 1; ++i) {
        qreal d = QLineF(point, mapToCanvas(m_controlPoints.at(i))).length();
        if ((distance < 0 && d < 10) || d < distance) {
            distance = d;
            pointIndex = i;
        }
    }
    return pointIndex;
}

static inline bool veryFuzzyCompare(qreal r1, qreal r2)
{
    if (qFuzzyCompare(r1, 2))
        return true;

    int r1i = qRound(r1 * 20);
    int r2i = qRound(r2 * 20);

    if (qFuzzyCompare(qreal(r1i) / 20, qreal(r2i) / 20))
        return true;

    return false;
}

bool SplineEditor::isSmooth(int i) const
{
    if (i == 0)
        return false;

    QPointF p = m_controlPoints.at(i);
    QPointF p_before = m_controlPoints.at(i - 1);
    QPointF p_after = m_controlPoints.at(i + 1);

    QPointF v1 = p_after - p;
    v1 = v1 / v1.manhattanLength(); //normalize

    QPointF v2 = p - p_before;
    v2 = v2 / v2.manhattanLength(); //normalize

    return veryFuzzyCompare(v1.x(), v2.x()) && veryFuzzyCompare(v1.y(), v2.y());
}

void SplineEditor::smoothPoint(int index)
{
    if (m_smoothAction->isChecked()) {

        QPointF before = QPointF(0,0);
        if (index > 3)
            before = m_controlPoints.at(index - 3);

        QPointF after = QPointF(1.0, 1.0);
        if ((index + 3) < m_controlPoints.count())
            after = m_controlPoints.at(index + 3);

        QPointF tangent = (after - before) / 6;

        QPointF thisPoint =  m_controlPoints.at(index);

        if (index > 0)
            m_controlPoints[index - 1] = thisPoint - tangent;

        if (index + 1  < m_controlPoints.count())
            m_controlPoints[index + 1] = thisPoint + tangent;

        m_smoothList[index / 3] = true;
    } else {
        m_smoothList[index / 3] = false;
    }
    invalidate();
    update();
}

void SplineEditor::cornerPoint(int index)
{
    QPointF before = QPointF(0,0);
    if (index > 3)
        before = m_controlPoints.at(index - 3);

    QPointF after = QPointF(1.0, 1.0);
    if ((index + 3) < m_controlPoints.count())
        after = m_controlPoints.at(index + 3);

    QPointF thisPoint =  m_controlPoints.at(index);

    if (index > 0)
        m_controlPoints[index - 1] = (before - thisPoint) / 3 + thisPoint;

    if (index + 1  < m_controlPoints.count())
        m_controlPoints[index + 1] = (after - thisPoint) / 3 + thisPoint;

    m_smoothList[(index) / 3] = false;
    invalidate();
}

void SplineEditor::deletePoint(int index)
{
    m_controlPoints.remove(index - 1, 3);
    m_numberOfSegments--;

    invalidateSmoothList();
    invalidate();
}

void SplineEditor::addPoint(const QPointF point)
{
    QPointF newPos = mapFromCanvas(point);
    int splitIndex = 0;
    for (int i=0; i < m_controlPoints.size() - 1; ++i) {
        if (indexIsRealPoint(i) && m_controlPoints.at(i).x() > newPos.x()) {
            break;
        } else if (indexIsRealPoint(i))
            splitIndex = i;
    }
    QPointF before = QPointF(0,0);
    if (splitIndex > 0)
        before = m_controlPoints.at(splitIndex);

    QPointF after = QPointF(1.0, 1.0);
    if ((splitIndex + 3) < m_controlPoints.count())
        after = m_controlPoints.at(splitIndex + 3);

    if (splitIndex > 0) {
        m_controlPoints.insert(splitIndex + 2, (newPos + after) / 2);
        m_controlPoints.insert(splitIndex + 2, newPos);
        m_controlPoints.insert(splitIndex + 2, (newPos + before) / 2);
    } else {
        m_controlPoints.insert(splitIndex + 1, (newPos + after) / 2);
        m_controlPoints.insert(splitIndex + 1, newPos);
        m_controlPoints.insert(splitIndex + 1, (newPos + before) / 2);
    }
    m_numberOfSegments++;

    invalidateSmoothList();
    invalidate();
}

bool SplineEditor::isControlPointSmooth(int i) const
{
    if (i == 0)
        return false;

    if (i == m_controlPoints.count() - 1)
        return false;

    if (m_numberOfSegments == 1)
        return false;

    int index = pointForControlPoint(i);

    if (index == 0)
        return false;

    if (index == m_controlPoints.count() - 1)
        return false;

    return m_smoothList.at(index / 3);
}

QPointF limitToCanvas(const QPointF point)
{
    qreal left = -qreal( canvasMargin) / qreal(canvasWidth);
    qreal width = 1.0 - 2.0 * left;

    qreal top = -qreal( canvasMargin) / qreal(canvasHeight);
    qreal height = 1.0 - 2.0 * top;

    QPointF p = point;
    QRectF r(left, top, width, height);

    if (p.x() > r.right()) {
        p.setX(r.right());
    }
    if (p.x() < r.left()) {
        p.setX(r.left());
    }
    if (p.y() < r.top()) {
        p.setY(r.top());
    }
    if (p.y() > r.bottom()) {
        p.setY(r.bottom());
    }
    return p;
}

void SplineEditor::mouseMoveEvent(QMouseEvent *e)
{
    // If we've moved more then 25 pixels, assume user is dragging
    if (!m_mouseDrag && QPoint(m_mousePress - e->pos()).manhattanLength() > qApp->startDragDistance())
        m_mouseDrag = true;

    QPointF p = mapFromCanvas(e->pos());

    if (m_mouseDrag && m_activeControlPoint >= 0 && m_activeControlPoint < m_controlPoints.size()) {
        p = limitToCanvas(p);
        if (indexIsRealPoint(m_activeControlPoint)) {
            //move also the tangents
            QPointF targetPoint = p;
            QPointF distance = targetPoint - m_controlPoints[m_activeControlPoint];
            m_controlPoints[m_activeControlPoint] = targetPoint;
            m_controlPoints[m_activeControlPoint - 1] += distance;
            m_controlPoints[m_activeControlPoint + 1] += distance;
        } else {
            if (!isControlPointSmooth(m_activeControlPoint)) {
                m_controlPoints[m_activeControlPoint] = p;
            } else {
                QPointF targetPoint = p;
                QPointF distance = targetPoint - m_controlPoints[m_activeControlPoint];
                m_controlPoints[m_activeControlPoint] = p;

                if ((m_activeControlPoint > 1) && (m_activeControlPoint % 3) == 0) { //right control point
                    m_controlPoints[m_activeControlPoint - 2] -= distance;
                } else if ((m_activeControlPoint < (m_controlPoints.count() - 2)) //left control point
                           && (m_activeControlPoint % 3) == 1) {
                    m_controlPoints[m_activeControlPoint + 2] -= distance;
                }
            }
        }
        invalidate();
    }
}

void SplineEditor::setEasingCurve(const QEasingCurve &easingCurve)
{
    if (m_easingCurve == easingCurve)
        return;
    m_block = true;
    m_easingCurve = easingCurve;
    m_controlPoints = m_easingCurve.toCubicSpline();
    m_numberOfSegments = m_controlPoints.count() / 3;
    update();
    emit easingCurveChanged();

    m_block = false;
}

void SplineEditor::setEasingCurve(const QString &code)
{
    if (m_block)
        return;
    if (code.left(1) == QLatin1String("[") && code.right(1) == QLatin1String("]")) {
        QString cleanCode = code;
        cleanCode.remove(0, 1);
        cleanCode.chop(1);
        const QStringList stringList = cleanCode.split(QLatin1Char(','), QString::SkipEmptyParts);
        if (stringList.count() >= 6 && (stringList.count() % 6 == 0)) {
            QList<qreal> realList;
            foreach (const QString &string, stringList) {
                bool ok;
                realList.append(string.toDouble(&ok));
                if (!ok)
                    return;
            }
            QList<QPointF> points;
            for (int i = 0; i < realList.count() / 2; ++i)
                points.append(QPointF(realList.at(i * 2), realList.at(i * 2 + 1)));
            if (points.last() == QPointF(1.0, 1.0)) {
                QEasingCurve easingCurve(QEasingCurve::BezierSpline);

                for (int i = 0; i < points.count() / 3; ++i) {
                    easingCurve.addCubicBezierSegment(points.at(i * 3),
                                                      points.at(i * 3 + 1),
                                                      points.at(i * 3 + 2));
                }
                setEasingCurve(easingCurve);
                invalidateSmoothList();
            }
        }
    }
}
