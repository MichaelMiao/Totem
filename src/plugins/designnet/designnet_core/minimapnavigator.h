#pragma once

#include <QGraphicsView>

namespace DesignNet{

class DesignNetEditor;
class MinimapNavigator : public QGraphicsView
{
public:

	MinimapNavigator(QWidget* pParent);
	~MinimapNavigator();

	void setEditor(DesignNetEditor *pEditor);

public slots:

	void showScene();

protected:

	void paintEvent (QPaintEvent * event) override;
	void drawForeground(QPainter *painter, const QRectF &rect) override;
	void mousePressEvent (QMouseEvent * event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private:

	DesignNetEditor*	m_pCurEditor;
	bool				m_bPressed;
};
}
