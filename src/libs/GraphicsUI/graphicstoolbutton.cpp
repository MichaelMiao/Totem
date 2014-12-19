#include "totem_gui_pch.h"
#include "graphicstoolbutton.h"
namespace GraphicsUI{

GraphicsToolButton::GraphicsToolButton(QGraphicsItem *parent)
	: GraphicsAutoShowHideItem(parent)
{
	setSize(QSize(16, 16));
	animateShow(true);
	setAutoHide(false);
}

GraphicsToolButton::~GraphicsToolButton()
{
	
}

}
