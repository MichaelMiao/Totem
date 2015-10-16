#pragma once

namespace DesignNet
{
	namespace Constants
	{
		const char	C_DESIGNNET[]	= "Totem.DesignNet.DesignNet";
		const char	ID_EDITMODE[]	= "Totem.DesignNet.EditMode";
		const char	NETEDITOR_FILETYPE[] = "txt";
		const char	NETEDITOR_ID[]	= "Totem.DesignNet.EditorId";
	}

	enum PropertyType
	{
		Property_Bool,
		Property_Option,
		Property_Path,
		Property_Range,
	};
}