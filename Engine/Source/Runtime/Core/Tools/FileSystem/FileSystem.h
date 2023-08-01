#pragma once

#include <filesystem>
#include "Buffer.h"

namespace LitchiRuntime {

	class FileSystem
	{
	public:
		// TODO: move to FileSystem class
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};

}
