#ifndef DESIGNNETCONSTANTS_H
#define DESIGNNETCONSTANTS_H

namespace DesignNet{
namespace Constants{

const char C_DESIGNNET[]            = "DesignNet.DesignNet";
const char TOOLFILE_EXTENSION[]        = "tool";

const char GRAPHICS_DESIGNNET_ID[]  = "Editor.DesignNet";
const char NETEDITOR_ID[]           = "DesignNet.DesignNetEditor";
const char NETEDITOREDITOR_DISPLAYNAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors","DesignNet Editor");
const char NETEDITOR_FILETYPE[]     = "txt";
const char DEFAULT_DOCK_AREA[]      = "DesignNet.DefaultDockArea";

const char DESIGNNET_MODE[]         = "Mode.DesignNetMode";
const char DESIGNNET_MODETYPE[]     = "ModeType.DesignNetModeType";
const char DESIGNNET_ICON[]         = ":/media/DesignNet.png";
const char DESIGNNET_MODE_DISPLAYNAME[] = QT_TRANSLATE_NOOP("DesignNet", "DesignNet");

const char DESIGNNET_PROCESS_ID[]   = "DesignNet.Process";
/// DockWidget Name
const char DESIGN_DOCKNAME_TOOL[]       = "DockName.Tools";
const char DESIGN_DOCKNAME_PROPERTY[]   = "DockName.Properties";
/// GraphicsItem Image
const char ITEM_IMAGE_PORT_CONNECTED[]        = ":/media/port_connected.png";
const char ITEM_IMAGE_PORT_NOTCONNECTED[]     = ":/media/port_notconnected.png";

//MIME Type
const char MIME_TYPE_TOOLITEM[] = "DesignNet/ToolItem";

/// Normal Property Type
const char PROPERTY_TYPE_PATHDIALOG[]	= "PropertyType/PathDialog";
const char PROPERTY_TYPE_OPTION[]		= "PropertyType/Option";
/// Data Type
const char DATA_TYPE_IMAGE[] = "DataType/Image";
const char DATA_TYPE_LABEL[] = "DataType/Label";
/// Data Widget Factory ID
const char DATA_WIDGET_FACTORY_NORMAL[] = "DataWidgetFactory/Normal";

const float ZValue_GraphicsBlock_Normal     = 3.0f;
const float ZValue_GraphicsBlock_Emphasize  = 30.0f;
const float ZValue_Tooltip = 40.0f;
}


///
/// GraphicsItem的Type
const int PortGraphicsItemType = 1;         //!< 端口类型
const int PortArrowLinkItemType = 2;        //!< 端口箭头类型
const int ProcessorGraphicsBlockType = 3;   //!< 处理器类型
}//namespace DesignNet
#define _T(x) QLatin1String(x)

#endif // DESIGNNETCONSTANTS_H
