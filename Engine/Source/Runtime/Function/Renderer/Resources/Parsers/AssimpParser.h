
#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "EModelParserFlags.h"
#include "IModelParser.h"
#include "Runtime/Function/Renderer/Resources/Mesh.h"
#include <Runtime/Function/Renderer/Resources/Model.h>

#pragma once

namespace LitchiRuntime::Parsers
{
	/**
	* A simple class to load assimp model data (Vertices only)
	*/
	class AssimpParser : public IModelParser
	{
	public:

		bool LoadModel(const std::string& p_fileName, Model* p_model, EModelParserFlags p_parserFlags) override;


	private:

		void ReadNodeHierarchy(Model* model,const aiNode* node, uint32_t parentIndex);

		void LoadBones(Model* model,const aiMesh* mesh, std::vector<Model::BoneData>& bones);
		void ProcessMaterials(Model* model,const struct aiScene* p_scene);
		void ProcessNode(Model* model,void* p_transform, struct aiNode* p_node, const struct aiScene* p_scene);
		Mesh* ProcessMesh(Model* model,void* p_transform, struct aiMesh* p_mesh, const struct aiScene* p_scene, unsigned materialIndex);
		void LoadAnimations(Model* model,const aiScene* scene);
	};
}