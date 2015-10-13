#pragma once

#include <QtQuickWidgets/QQuickWidget>


class DesignNetView : public QQuickWidget
{
	Q_OBJECT

public:

	DesignNetView(QWidget* parent) : QQuickWidget(parent) {}
};
