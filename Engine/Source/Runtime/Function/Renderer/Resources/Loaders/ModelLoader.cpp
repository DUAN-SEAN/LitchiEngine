

#include "Runtime/Function/Renderer/Resources/Parsers/AssimpParser.h"

#include "ModelLoader.h"

#include "Runtime/Function/Renderer/Resources/Model.h"

LitchiRuntime::Parsers::AssimpParser LitchiRuntime::Loaders::ModelLoader::__ASSIMP;

LitchiRuntime::Model* LitchiRuntime::Loaders::ModelLoader::Create(const std::string& p_filepath, Parsers::EModelParserFlags p_parserFlags)
{
	Model* result = new Model(p_filepath);
	
	if (__ASSIMP.LoadModel(p_filepath, result, p_parserFlags))
	{
		result->ComputeBoundingSphere();
		return result;
	}

	delete result;

	return nullptr;
}

void LitchiRuntime::Loaders::ModelLoader::Reload(Model& p_model, const std::string& p_filePath, Parsers::EModelParserFlags p_parserFlags)
{
	Model* newModel = Create(p_filePath, p_parserFlags);

	if (newModel)
	{
		p_model.mesheArr = newModel->mesheArr;
		p_model.materialNameArr = newModel->materialNameArr;
        p_model.boundingSphere = newModel->boundingSphere;
		newModel->mesheArr.clear();
		delete newModel;
	}
}

bool LitchiRuntime::Loaders::ModelLoader::Destroy(Model*& p_modelInstance)
{
	if (p_modelInstance)
	{
		delete p_modelInstance;
		p_modelInstance = nullptr;

		return true;
	}

	return false;
}
