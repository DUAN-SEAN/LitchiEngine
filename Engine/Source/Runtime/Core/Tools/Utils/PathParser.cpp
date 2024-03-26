
#include "Runtime/Core/pch.h"
#include <algorithm>
#include <string>

#include "PathParser.h"

std::string LitchiRuntime::PathParser::MakeWindowsStyle(const std::string & p_path)
{
	std::string result;
	result.resize(p_path.size());

	for (size_t i = 0; i < p_path.size(); ++i)
		result[i] = p_path[i] == '/' ? '\\' : p_path[i];

	return result;
}

std::string LitchiRuntime::PathParser::MakeNonWindowsStyle(const std::string & p_path)
{
	std::string result;
	result.resize(p_path.size());

	for (size_t i = 0; i < p_path.size(); ++i)
		result[i] = p_path[i] == '\\' ? '/' : p_path[i];

	return result;
}

std::string LitchiRuntime::PathParser::GetContainingFolder(const std::string & p_path)
{
	std::string result;

	bool extraction = false;

	for (auto it = p_path.rbegin(); it != p_path.rend(); ++it)
	{
		if (extraction)
			result += *it;

		if (!extraction && it != p_path.rbegin() && (*it == '\\' || *it == '/'))
			extraction = true;
	}

	std::reverse(result.begin(), result.end());

	if (!result.empty() && result.back() != '\\'&& result.back() != '/')
		result += '\\';

	return result;
}

std::string LitchiRuntime::PathParser::GetElementName(const std::string & p_path)
{
	std::string result;

	std::string path = p_path;
	if (!path.empty() && (path.back() == '\\'|| path.back() == '/'))
		path.pop_back();

	for (auto it = path.rbegin(); it != path.rend() && *it != '\\' && *it != '/'; ++it)
		result += *it;

	std::reverse(result.begin(), result.end());

	return result;
}

std::string LitchiRuntime::PathParser::GetExtension(const std::string & p_path)
{
	std::string result;

	for (auto it = p_path.rbegin(); it != p_path.rend() && *it != '.'; ++it)
		result += *it;

	std::reverse(result.begin(), result.end());

	return result;
}

std::string LitchiRuntime::PathParser::FileTypeToString(EFileType p_fileType)
{
	switch (p_fileType)
	{
	case LitchiRuntime::PathParser::EFileType::MODEL:		return "Model";
	case LitchiRuntime::PathParser::EFileType::TEXTURE:	return "Texture";
	case LitchiRuntime::PathParser::EFileType::SHADER:		return "Shader";
	case LitchiRuntime::PathParser::EFileType::MATERIAL:	return "Material";
	case LitchiRuntime::PathParser::EFileType::SOUND:		return "Sound";
	case LitchiRuntime::PathParser::EFileType::SCENE:		return "Scene";
	case LitchiRuntime::PathParser::EFileType::SCRIPT:		return "Script";
	case LitchiRuntime::PathParser::EFileType::FONT:		return "Font";
	case LitchiRuntime::PathParser::EFileType::PREFAB:		return "Prefab";
	}

	return "Unknown";
}

LitchiRuntime::PathParser::EFileType LitchiRuntime::PathParser::GetFileType(const std::string & p_path)
{
	std::string ext = GetExtension(p_path);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if (ext == "fbx" || ext == "obj")											return EFileType::MODEL;
	else if (ext == "png" || ext == "jpeg" || ext == "jpg" || ext == "tga")		return EFileType::TEXTURE;
	else if (ext == "glsl" || ext == "vs" || ext  == "fs")														return EFileType::SHADER;
	else if (ext == "mat")													return EFileType::MATERIAL;
	else if (ext == "wav" || ext == "mp3" || ext == "ogg")						return EFileType::SOUND;
	else if (ext == "scene")													return EFileType::SCENE;
	else if (ext == "cs")														return EFileType::SCRIPT;
	else if (ext == "ttf")														return EFileType::FONT;
	else if (ext == "prefab")														return EFileType::PREFAB;

	return EFileType::UNKNOWN;
}