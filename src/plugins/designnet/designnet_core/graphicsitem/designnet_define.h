#pragma once
#include <QGraphicsItem>

enum GraphicsItemType
{
GraphicsItemType_Process	= QGraphicsItem::UserType + 1,
GraphicsItemType_Port,
GraphicsItemType_ProcessConnect,
GraphicsItemType_PortConnect,
};