
#pragma once

namespace LitchiRuntime
{
	/**
	 * @brief Some cursor modes.
	 * They defines if the mouse pointer should be visible, locked or normal
	 */
	enum class ECursorMode
	{
		NORMAL		= 0x00034001,
		DISABLED	= 0x00034003,
		HIDDEN		= 0x00034002
	};
}