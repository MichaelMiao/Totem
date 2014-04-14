#pragma once
#ifndef NORMALIMAGELOADERWIDGET_H
#define NORMALIMAGELOADERWIDGET_H

#include <QWidget>
#include <QLabel>
#include "../../designnet_core/widgets/processorfrontwidget.h"


namespace InputLoader {
class GraphicsNormalImageLoader;
class NormalImageLoaderWidget : public DesignNet::ProcessorFrontWidget
{
	Q_OBJECT

public:

	NormalImageLoaderWidget(GraphicsNormalImageLoader *processor);
	~NormalImageLoaderWidget();


protected:

	void paintEvent(QPaintEvent * event);

	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);

private:

	GraphicsNormalImageLoader*	m_processor;
	QString						m_text;
};
}
#endif // NORMALIMAGELOADERWIDGET_H
