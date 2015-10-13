#pragma once

namespace Core
{
	namespace Constants
	{
		const char ALL_FILES_FILTER[] = QT_TRANSLATE_NOOP("Core", "All Files (*)");

		// Context
		const char C_GLOBAL[]			= "Global Context";

		// Default groups
		const char G_DEFAULT_ONE[]			= "Totem.Group.Default.One";
		const char G_DEFAULT_TWO[]			= "Totem.Group.Default.Two";
		const char G_DEFAULT_THREE[]		= "Totem.Group.Default.Three";
		const char G_FILE[]					= "Totem.Group.File";
		const char G_HELP[]					= "Totem.Group.HELP";
		const char G_TOOLS[]				= "Totem.Group.Tools";

		// menu
		const char MENU_BAR[]				= "Totem.MenuBar";
		const char M_HELP[]					= "Totem.Help";
		const char M_TOOLS[]				= "Totem.Tools";
		const char M_HELP_ABOUT[]			= "Totoem.Help.About";
		const char M_HELP_ABOUTPLUGINS[]	= "Totoem.Help.AboutPlugins";
		const char M_FILE[]					= "Totem.File";
		const char M_FILE_RECENT[]			= "Totem.File.Recent";
		const char M_FILE_NEW[]				= "Totem.File.New";
		const char M_FILE_OPEN[]			= "Totem.File.Open";
		const char M_FILE_SAVE[]			= "Totem.File.Save";
		const char M_FILE_SAVEAS[]			= "Totem.File.SaveAs";


		// Icon
		const char ICON_ABOUT_ACTION[]		= ":/medias/about-totem.png";
		const char ICON_OPTIONS_ACTION[]	= ":/medias/option.png";
		const char ICON_ABOUT_PLUGINS[]		= ":/medias/about-plugin.png";
		const char ICON_CATEGORY_SHORTCUT[] = ":/medias/category_shortcut.png";
		const char ICON_NEW_FILE[]			= ":/medias/document-new.png";
		//IDs
		const char SETTINGS_CATEGORY_CORE[] = "Environment.Core";
		const char SETTINGS_TR_CATEGORY_CORE[] = QT_TRANSLATE_NOOP("Core", "Environment");
		const char SETTINGS_ID_ENVIRONMENT[] = "Environment.General";
		const char SETTINGS_ID_SHORTCUTS[]	= "Environmet.Keyboard";
		const char SETTINGS_ID_TOOLS[]		= "Environment.ExternalTools";
		const char SETTINGS_ID_MIMETYPES[]	= "Environment.MimeTypes";

		const char C_EDITORMANAGER[]		= "Core.EditorManager";
	}
}
