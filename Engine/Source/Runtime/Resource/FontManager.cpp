
#include "Runtime/Core/pch.h"
#include "FontManager.h"
#include "Runtime/Core/Tools/FileSystem/IniFile.h"

std::tuple<int> GetAssetMetadata(const std::string& p_path)
{
	auto metaFile = LitchiRuntime::IniFile(p_path + ".meta");

	auto fontSize = metaFile.GetOrDefault("FONT_SIZE", 20);

	return { fontSize };
}

LitchiRuntime::Font* LitchiRuntime::FontManager::CreateResource(const std::string& p_path)
{
	std::string realPath = GetRealPath(p_path);

	auto [fontSize] = GetAssetMetadata(realPath);

	//LitchiRuntime::Font* font = Loaders::FontLoader::Create(realPath, fontSize);
	//if (font)
	//	*reinterpret_cast<std::string*>(reinterpret_cast<char*>(font) + offsetof(LitchiRuntime::Font, path)) = p_path; // Force the resource path to fit the given path

	auto color = Color::Black;
	auto font = new Font(realPath,fontSize,{color.r,color.g,color.b,color.a});
	if(!font->LoadFromFile(realPath))
	{
		delete font;
		return nullptr;
	}

	return font;
}

void LitchiRuntime::FontManager::DestroyResource(LitchiRuntime::Font* p_resource)
{
	// do nothing
	delete p_resource;
}

void LitchiRuntime::FontManager::ReloadResource(LitchiRuntime::Font* p_resource, const std::string& p_path)
{
	std::string realPath = GetRealPath(p_path);
	if (!p_resource->LoadFromFile(realPath))
	{
		DEBUG_LOG_ERROR("ReloadResource Fail path:{}", p_path);
	}
}
