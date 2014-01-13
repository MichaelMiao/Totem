#ifndef DESIGNNETCONSTANTS_H
#define DESIGNNETCONSTANTS_H
#include <QtGlobal>
namespace DesignNet{
namespace Constants{

const char INFO_READ_ONLY[] = "DesignNetXmlEditor.ReadOnly";


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

const char DESIGNNET_EDITSTATE_MOVE_ACTION[]	= "DesignNet.EditState.Move";
const char DESIGNNET_EDITSTATE_LINK_ACTION[]	= "DesignNet.EditState.Link";

/// DockWidget Name
const char DESIGN_DOCKNAME_TOOL[]       = "DockName.Tools";
const char DESIGN_DOCKNAME_PROPERTY[]   = "DockName.Properties";
const char DESIGN_DOCKNAME_AVAILIABLEDATA[] = "DockName.AvailiableData";

/// GraphicsItem Image
const char ITEM_IMAGE_PORT_CONNECTED[]        = ":/media/port_connected.png";
const char ITEM_IMAGE_PORT_NOTCONNECTED[]     = ":/media/port_notconnected.png";

/// Data Type Image
const char DATA_IMAGE_COLOR_IMAGE[]			= ":/media/colorimage.png";
const char DATA_IMAGE_GRAY_IMAGE[]			= ":/media/grayimages.jpg";
const char DATA_IMAGE_BINARY_IMAGE[]		= ":/media/binaryimage.png";
const char DATA_IMAGE_MATRIX[]				= ":/media/matrix.jpg";
const char DATA_IMAGE_INT[]					= ":/media/intdata.png";
//MIME Type
const char MIME_TYPE_TOOLITEM[] = "DesignNet/ToolItem";

/// Normal Property Type
const char PROPERTY_TYPE_PATHDIALOG[]		= "PropertyType/PathDialog";
const char PROPERTY_TYPE_OPTION[]			= "PropertyType/Option";
const char PROPERTY_TYPE_DOUBLERANGE[]		= "PropertyType/DoubleRange";
const char PROPERTY_TYPE_BOOL[]				= "PropertyType/Bool";
const char PROPERTY_TYPE_INT[]				= "PropertyType/Int";
/// Data Type
const char DATA_TYPE_IMAGE[]	= "DataType/Image";
const char DATA_TYPE_LABEL[]	= "DataType/Label";
const char DATA_TYPE_HISTOGRAM[] = "DataType/Histogram";
const char DATA_TYPE_MATRIX[]	= "DataType/Matrix";
const char DATA_TYPE_INT[]		= "DataType/Int";
const char DATA_TYPE_CUSTOM[]	= "DataType/Custom";
/// Data Widget Factory ID
const char DATA_WIDGET_FACTORY_NORMAL[] = "DataWidgetFactory/Normal";
/// ICON

const char ICON_LIST_ADD[]		= "list-add.png";
const char ICON_LIST_REMOVE[]	= "list-remove.png";

const float ZValue_GraphicsBlock_Normal     = 20.0f;
const float ZValue_GraphicsBlock_Emphasize  = 30.0f;
const float ZValue_ArrowLink				= 20.0f;
const float ZValue_Tooltip = 40.0f;
}


///
/// GraphicsItem的Type
const int PortGraphicsItemType = 1;         //!< 端口类型
const int PortArrowLinkItemType = 2;        //!< 端口箭头类型
const int ProcessorGraphicsBlockType = 3;   //!< 处理器类型
}//namespace DesignNet


#define _T(x) QLatin1String(x)
#define PORTITEM_WIDTH		10

#endif // DESIGNNETCONSTANTS_H
