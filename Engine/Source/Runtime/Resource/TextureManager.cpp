//
//#include "TextureManager.h"
//
//#include "Runtime/Core/Tools/FileSystem/IniFile.h"
//#include "Runtime/Function/Renderer/Resources/Loaders/TextureLoader.h"
//
//std::tuple<LitchiRuntime::ETextureFilteringMode, LitchiRuntime::ETextureFilteringMode, bool> GetAssetMetadata(const std::string& p_path)
//{
//	auto metaFile = LitchiRuntime::IniFile(p_path + ".meta");
//
//	auto min = metaFile.GetOrDefault("MIN_FILTER", static_cast<int>(LitchiRuntime::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR));
//	auto mag = metaFile.GetOrDefault("MAG_FILTER", static_cast<int>(LitchiRuntime::ETextureFilteringMode::LINEAR));
//	auto mipmap = metaFile.GetOrDefault("ENABLE_MIPMAPPING", true);
//
//	return { static_cast<LitchiRuntime::ETextureFilteringMode>(min), static_cast<LitchiRuntime::ETextureFilteringMode>(mag), mipmap };
//}
//
//LitchiRuntime::Texture* LitchiRuntime::TextureManager::CreateResource(const std::string & p_path)
//{
//	std::string realPath = GetRealPath(p_path);
//
//	auto [min, mag, mipmap] = GetAssetMetadata(realPath);
//
//	LitchiRuntime::Texture* texture = Loaders::TextureLoader::Create(realPath, min, mag, mipmap);
//	if (texture)
//		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(texture) + offsetof(LitchiRuntime::Texture, path)) = p_path; // Force the resource path to fit the given path
//
//	return texture;
//}
//
//void LitchiRuntime::TextureManager::DestroyResource(LitchiRuntime::Texture* p_resource)
//{
//	Loaders::TextureLoader::Destroy(p_resource);
//}
//
//void LitchiRuntime::TextureManager::ReloadResource(LitchiRuntime::Texture* p_resource, const std::string& p_path)
//{
//	std::string realPath = GetRealPath(p_path);
//
//	auto [min, mag, mipmap] = GetAssetMetadata(realPath);
//
//	Loaders::TextureLoader::Reload(*p_resource, realPath, min, mag, mipmap);
//}
