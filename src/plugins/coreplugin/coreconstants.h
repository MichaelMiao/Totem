#ifndef CORECONSTANTS_H
#define CORECONSTANTS_H

#include <QtGlobal>

namespace Core{
namespace Constants{

const char APP_NAME[]              = "Totem";
//Mode
const int  P_MODE_DESIGN           = 70;
const int  P_MODE_VIEW             = 80;
const int  P_MODE_EDIT             = 90;
const char MODE_EDIT[]             = "Edit";
const char MODE_EDIT_TYPE[]        = "Type.Edit";
const char MODE_DESIGN[]           = "DesignNet";
const char MODE_DESIGN_TYPE[]           = "Type.Design";
const char MODE_VIEW[]             = "View";
const char MODE_VIEW_TYPE[]        = "Type.View";

const char ICON_TOTEM_128[] = ":/core/images/totem.png";

//MenuBar
const char MENU_BAR[]              = "Totem.MenuBar";

//MenuBar groups
const char G_FILE[]                = "Totem.Group.File";
const char G_VIEW[]                = "Totem.Group.View";
const char G_HELP[]                = "Totem.Group.Help";
const char G_WINDOW[]              = "Totem.Group.Window";
const char G_TOOLS[]                = "Totem.Group.Tool";
const char G_FILE_SAVE[]			= "Tote.Group.Save";
//SubMenuBar IDs
const char M_FILE[]                = "Totem.Menu.File";
const char M_VIEW[]                = "Totem.Menu.View";
const char M_HELP[]                = "Totem.Menu.Help";
const char M_WINDOW[]              = "Totem.Menu.Window";
const char M_TOOLS[]                = "Totem.Menu.Tool";

const char M_WINDOW_VIEWS[]        = "Totem.Menu.Window.Views";
const char M_WINDOW_PANES[]        = "Totem.Menu.Window.Panes";


//Menu Groups

const char G_WINDOW_PANES[]        = "Totem.Group.Window.Panes";
const char G_FILE_NEW[]            = "Totem.Group.File.New";
const char G_FILE_OPEN[]           = "Totem.Group.File.Open";
//Tools Menu groups
const char G_TOOLS_OPTIONS[]       = "Totem.Group.Tools.Options";
//Help Menu groups
const char G_HELP_ABOUT[]        = "Totem.Group.Help.About";
const char G_HELP_ABOUT_PLUGIN[]       = "Totem.Group.Help.AboutPlugin";

// Default groups
const char G_DEFAULT_ONE[]         = "Totem.Group.Default.One";
const char G_DEFAULT_TWO[]         = "Totem.Group.Default.Two";
const char G_DEFAULT_THREE[]       = "Totem.Group.Default.Three";

//Help Actions
const char ABOUT_TOTEM[]           = "Totem.About";
const char ABOUT_PLUGINS[]         = "Totem.AboutPlugin";

//actions
const char NEW[]                   = "Totem.New";
const char OPEN[]                  = "Totem.Open";
const char UNDO[]                  = "Totem.Undo";
const char REDO[]                  = "Totem.Redo";
const char COPY[]                  = "Totem.Copy";
const char PASTE[]                 = "Totem.Paste";
const char CUT[]                   = "Totem.Cut";
const char SELECTALL[]             = "Totem.SelectAll";
const char OPTIONS[]               = "Totem.Options";
const char CLOSE[]				   = "Totem.CloseDocument";
const char SAVE[]				   = "Totem.Save";
const char SAVEAS[]				   = "Totem.SaveAs";
// Contexts
const char C_GLOBAL[]              = "Global Context";
const char C_GENERAL_OUTPUT_PANE[] = "Core.GeneralOutputPane";
const char C_DESIGN_MODE[]         = "Core.DesignMode";
const char C_EDITORMANAGER[]	   = "Core.EditorManager";

const unsigned int DEFAULT_BASE_COLOR = 0x666666;
//ICON
const char ICON_CLEAN_PANE[]		= ":/core/images/edit-clear.png";
const char ICON_CLOSE_DOCUMENT[]	= ":/core/images/close.png";
const char ICON_ABOUT_PLUGINS[]		= ":/core/images/stock-plugin-16.png";
const char ICON_OPTION[]			= ":/core/images/option.png";
const char ICON_NEW_FILE[]			= ":/core/images/document-new.png";
const char ICON_OPEN_FILE[]			= ":/core/images/document-open.png";
const char ICON_SAVE_FILE[]			= ":/core/images/save.png";
const char ICON_SAVEAS_FILE[]		= ":/core/images/saveas.png";
//IDs
const char SETTINGS_CATEGORY_CORE[] = "Environment.Core";
const char SETTINGS_CATEGORY_CORE_ICON[] = ":/core/images/category_core.png";
const char SETTINGS_TR_CATEGORY_CORE[] = QT_TRANSLATE_NOOP("Core", "Environment");
const char SETTINGS_ID_ENVIRONMENT[] = "Environment.General";
const char SETTINGS_ID_SHORTCUTS[] = "Environmet.Keyboard";
const char SETTINGS_ID_TOOLS[] = "Environment.ExternalTools";
const char SETTINGS_ID_MIMETYPES[] = "Environment.MimeTypes";

//ALL_FILES_FILTER
const char ALL_FILES_FILTER[] = QT_TRANSLATE_NOOP("Core", "All Files (*)");

//Wizard
const char WIZARD_SEPARATOR = '/';
}
}

#endif // CORECONSTANTS_H
