#ifndef GRAPHICSTOOLBUTTON_H
#define GRAPHICSTOOLBUTTON_H

#include <QGraphicsObject>
#include "graphicsui_global.h"
#include "graphicsautoshowhideitem.h"
namespace GraphicsUI{
class TOTEM_GRAPHICSUI_EXPORT GraphicsToolButton : public GraphicsAutoShowHideItem
{
	Q_OBJECT
public:

    GraphicsToolButton(QGraphicsItem *parent = 0);
	virtual ~GraphicsToolButton();

};
}

#endif // GRAPHICSTOOLBUTTON_H
