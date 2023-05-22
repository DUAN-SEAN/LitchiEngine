
#pragma once

#include <string>

namespace LitchiRuntime
{
	namespace Resource
	{
		class Shader;
	}
}

namespace LitchiRuntime::Loaders
{
	/**
	* Handle the Shader creation and destruction
	*/
	class ShaderLoader
	{
	public:
		/**
		* Disabled constructor
		*/
		ShaderLoader() = delete;

		/**
		* Create a shader
		* @param p_filePath
		*/
		static LitchiRuntime::Resource::Shader* Create(const std::string& p_filePath);

		/**
		* Create a shader from source
		* @param p_vertexShader
		* @param p_fragmentShader
		*/
		static LitchiRuntime::Resource::Shader* CreateFromSource(const std::string& p_vertexShader, const std::string& p_fragmentShader);

		/**
		* Recompile a shader
		* @param p_shader
		* @param p_filePath
		*/
		static void	Recompile(LitchiRuntime::Resource::Shader& p_shader, const std::string& p_filePath);

		/**
		* Destroy a shader
		* @param p_shader
		*/
		static bool Destroy(LitchiRuntime::Resource::Shader*& p_shader);

	private:
		static std::pair<std::string, std::string> ParseShader(const std::string& p_filePath);
		static uint32_t CreateProgram(const std::string& p_vertexShader, const std::string& p_fragmentShader);
		static uint32_t CompileShader(uint32_t p_type, const std::string& p_source);

		static std::string __FILE_TRACE;
	};
}
