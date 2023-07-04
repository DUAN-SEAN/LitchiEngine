
#include <string>
#include "ModelManager.h"

#include "Runtime/Core/Tools/FileSystem/IniFile.h"
#include "Runtime/Function/Renderer/Resources/Parsers/EModelParserFlags.h"
#include "Runtime/Function/Renderer/Resources/Loaders/ModelLoader.h"

LitchiRuntime::Parsers::EModelParserFlags GetAssetMetadata(const std::string& p_path)
{
	auto metaFile = LitchiRuntime::IniFile(p_path + ".meta");

	LitchiRuntime::Parsers::EModelParserFlags flags = LitchiRuntime::Parsers::EModelParserFlags::NONE;

	if (metaFile.GetOrDefault("CALC_TANGENT_SPACE",			true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::CALC_TANGENT_SPACE;
	if (metaFile.GetOrDefault("JOIN_IDENTICAL_VERTICES",	true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::JOIN_IDENTICAL_VERTICES;
	if (metaFile.GetOrDefault("MAKE_LEFT_HANDED",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::MAKE_LEFT_HANDED;
	if (metaFile.GetOrDefault("TRIANGULATE",				true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::TRIANGULATE;
	if (metaFile.GetOrDefault("REMOVE_COMPONENT",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::REMOVE_COMPONENT;
	if (metaFile.GetOrDefault("GEN_NORMALS",				false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::GEN_NORMALS;
	if (metaFile.GetOrDefault("GEN_SMOOTH_NORMALS",			true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::GEN_SMOOTH_NORMALS;
	if (metaFile.GetOrDefault("SPLIT_LARGE_MESHES",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::SPLIT_LARGE_MESHES;
	if (metaFile.GetOrDefault("PRE_TRANSFORM_VERTICES",		false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::PRE_TRANSFORM_VERTICES;
	if (metaFile.GetOrDefault("LIMIT_BONE_WEIGHTS",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::LIMIT_BONE_WEIGHTS;
	if (metaFile.GetOrDefault("VALIDATE_DATA_STRUCTURE",	false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::VALIDATE_DATA_STRUCTURE;
	if (metaFile.GetOrDefault("IMPROVE_CACHE_LOCALITY",		true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::IMPROVE_CACHE_LOCALITY;
	if (metaFile.GetOrDefault("REMOVE_REDUNDANT_MATERIALS", false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::REMOVE_REDUNDANT_MATERIALS;
	if (metaFile.GetOrDefault("FIX_INFACING_NORMALS",		false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FIX_INFACING_NORMALS;
	if (metaFile.GetOrDefault("SORT_BY_PTYPE",				false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::SORT_BY_PTYPE;
	if (metaFile.GetOrDefault("FIND_DEGENERATES",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FIND_DEGENERATES;
	if (metaFile.GetOrDefault("FIND_INVALID_DATA",			true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FIND_INVALID_DATA;
	if (metaFile.GetOrDefault("GEN_UV_COORDS",				true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::GEN_UV_COORDS;
	if (metaFile.GetOrDefault("TRANSFORM_UV_COORDS",		false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::TRANSFORM_UV_COORDS;
	if (metaFile.GetOrDefault("FIND_INSTANCES",				true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FIND_INSTANCES;
	if (metaFile.GetOrDefault("OPTIMIZE_MESHES",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::OPTIMIZE_MESHES;
	if (metaFile.GetOrDefault("OPTIMIZE_GRAPH",				false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::OPTIMIZE_GRAPH;
	if (metaFile.GetOrDefault("FLIP_UVS",					false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FLIP_UVS;
	if (metaFile.GetOrDefault("FLIP_WINDING_ORDER",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FLIP_WINDING_ORDER;
	if (metaFile.GetOrDefault("SPLIT_BY_BONE_COUNT",		false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::SPLIT_BY_BONE_COUNT;
	if (metaFile.GetOrDefault("DEBONE", false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::DEBONE;
	if (metaFile.GetOrDefault("GLOBAL_SCALE",				true))	flags |= LitchiRuntime::Parsers::EModelParserFlags::GLOBAL_SCALE;
	if (metaFile.GetOrDefault("EMBED_TEXTURES",				false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::EMBED_TEXTURES;
	if (metaFile.GetOrDefault("FORCE_GEN_NORMALS",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::FORCE_GEN_NORMALS;
	if (metaFile.GetOrDefault("DROP_NORMALS",				false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::DROP_NORMALS;
	if (metaFile.GetOrDefault("GEN_BOUNDING_BOXES",			false))	flags |= LitchiRuntime::Parsers::EModelParserFlags::GEN_BOUNDING_BOXES;

	return { flags };
}

LitchiRuntime::Model* LitchiRuntime::ModelManager::CreateResource(const std::string& p_path)
{
	std::string realPath = GetRealPath(p_path);
	auto model = Loaders::ModelLoader::Create(realPath, GetAssetMetadata(realPath));
	if (model)
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(model) + offsetof(Model, path)) = p_path; // Force the resource path to fit the given path

	return model;
}

void LitchiRuntime::ModelManager::DestroyResource(Model* p_resource)
{
	Loaders::ModelLoader::Destroy(p_resource);
}

void LitchiRuntime::ModelManager::ReloadResource(Model* p_resource, const std::string& p_path)
{
	std::string realPath = GetRealPath(p_path);
	Loaders::ModelLoader::Reload(*p_resource, realPath, GetAssetMetadata(realPath));
}
