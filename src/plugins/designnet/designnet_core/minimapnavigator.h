#pragma once

#include <QGraphicsView>

namespace DesignNet{

class DesignNetEditor;
class MinimapNavigator : public QGraphicsView
{
	Q_OBJECT
public:

	MinimapNavigator(QWidget* pParent);
	~MinimapNavigator();

	void setEditor(DesignNetEditor *pEditor);

public slots:

	void showScene(QRectF rc);

protected:

	void drawForeground(QPainter *painter, const QRectF &rect) override;
	void mousePressEvent (QMouseEvent * event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	void resizeEvent(QResizeEvent *event);

private:

	DesignNetEditor*	m_pCurEditor;
	bool				m_bPressed;
};
}
