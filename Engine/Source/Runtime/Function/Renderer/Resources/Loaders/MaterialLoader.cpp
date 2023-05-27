
#include "rttr/registration"

#include "MaterialLoader.h"
#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Renderer/Resources/Material.h"
#include "Runtime/Resource/asset_manager.h"

LitchiRuntime::Resource::Material* LitchiRuntime::Loaders::MaterialLoader::Create(const std::string & p_path)
{
	Resource::MaterialRes* materialRes = new Resource::MaterialRes();
	if(AssetManager::LoadAsset(p_path, *materialRes))
	{
		Resource::Material* material = new Resource::Material();
		material->materialRes = materialRes;
		material->path = p_path;
		material->PostResourceLoaded();

		return material;
	}
	
	return nullptr;
}

void LitchiRuntime::Loaders::MaterialLoader::Reload(LitchiRuntime::Resource::Material& p_material, const std::string& p_path)
{
	if (SerializerManager::DeserializeFromJson(p_path, p_material))
	{
		p_material.PostResourceLoaded();
		
	}
}

void LitchiRuntime::Loaders::MaterialLoader::Save(LitchiRuntime::Resource::Material& p_material, const std::string& p_path)
{
	/*tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* node = doc.NewElement("root");
	doc.InsertFirstChild(node);

	p_material.OnSerialize(doc, node);

	if (doc.SaveFile(p_path.c_str()) == tinyxml2::XML_SUCCESS)
		OVLOG_INFO("[MATERIAL] \"" + p_path + "\": Saved");
	else
		OVLOG_ERROR("[MATERIAL] \"" + p_path + "\": Failed to save");*/
}

bool LitchiRuntime::Loaders::MaterialLoader::Destroy(LitchiRuntime::Resource::Material*& p_material)
{
	if (p_material)
	{
		delete p_material;
		p_material = nullptr;

		return true;
	}

	return false;
}