
#include "MaterialLoader.h"

#include "Runtime/Function/Renderer/Resources/Material.h"

LitchiRuntime::Resource::Material* LitchiRuntime::Loaders::MaterialLoader::Create(const std::string & p_path)
{
	/*tinyxml2::XMLDocument doc;
	doc.LoadFile(p_path.c_str());
	if (!doc.Error())
	{
		tinyxml2::XMLNode* root = doc.FirstChild();

		Material* material = new Material();

		material->OnDeserialize(doc, root);

		return material;
	}
	else
	{
		return nullptr;
	}*/


	return nullptr;
}

void LitchiRuntime::Loaders::MaterialLoader::Reload(LitchiRuntime::Resource::Material& p_material, const std::string& p_path)
{
	/*tinyxml2::XMLDocument doc;
	doc.LoadFile(p_path.c_str());
	if (!doc.Error())
	{
		tinyxml2::XMLNode* root = doc.FirstChild();

		p_material.OnDeserialize(doc, root);
	}*/
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