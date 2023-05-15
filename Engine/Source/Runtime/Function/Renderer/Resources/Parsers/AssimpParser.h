
#include <vector>

#include "EModelParserFlags.h"
#include "IModelParser.h"
#include "Runtime/Function/Renderer/Resources/Mesh.h"

#pragma once

namespace LitchiRuntime::Parsers
{
	/**
	* A simple class to load assimp model data (Vertices only)
	*/
	class AssimpParser : public IModelParser
	{
	public:
		/**
		* Simply load meshes from a file using assimp
		* Return true on success
		* @param p_filename
		* @param p_meshes
		* @param p_parserFlags
		*/
		bool LoadModel
		(
			const std::string& p_fileName,
			std::vector<Mesh*>& p_meshes,
			std::vector<std::string>& p_materials,
			EModelParserFlags p_parserFlags
		) override;

	private:
		void ProcessMaterials(const struct aiScene* p_scene, std::vector<std::string>& p_materials);;
		void ProcessNode(void* p_transform, struct aiNode* p_node, const struct aiScene* p_scene, std::vector<Mesh*>& p_meshes);
		void ProcessMesh(void* p_transform, struct aiMesh* p_mesh, const struct aiScene* p_scene, std::vector<Vertex>& p_outVertices, std::vector<uint32_t>& p_outIndices);
	};
}