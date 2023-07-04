
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include "AssimpParser.h"

#include "Runtime/Core/Log/debug.h"

bool LitchiRuntime::Parsers::AssimpParser::LoadModel(const std::string& p_fileName, std::vector<Mesh*>& p_meshes, std::vector<std::string>& p_materials, EModelParserFlags p_parserFlags)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(p_fileName, static_cast<unsigned int>(p_parserFlags));
	if (scene == nullptr)
	{
		std::string errorMsg(import.GetErrorString());
		DEBUG_LOG_ERROR(errorMsg);
	}
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		return false;

	ProcessMaterials(scene, p_materials);

	aiMatrix4x4 identity;

	ProcessNode(&identity, scene->mRootNode, scene, p_meshes);

	// 释放场景
	aiReleaseImport(scene);

	return true;
}

void LitchiRuntime::Parsers::AssimpParser::ProcessMaterials(const aiScene* p_scene, std::vector<std::string>& p_materials)
{
	for (uint32_t i = 0; i < p_scene->mNumMaterials; ++i)
	{
		aiMaterial* material = p_scene->mMaterials[i];
		if (material)
		{
			aiString name;
			aiGetMaterialString(material, AI_MATKEY_NAME, &name);
			p_materials.push_back(name.C_Str());
		}
	}
}

void LitchiRuntime::Parsers::AssimpParser::ProcessNode(void* p_transform, aiNode* p_node, const aiScene* p_scene, std::vector<Mesh*>& p_meshes)
{
	aiMatrix4x4 nodeTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform) * p_node->mTransformation;

	// Process all the node's meshes (if any)
	for (uint32_t i = 0; i < p_node->mNumMeshes; ++i)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		aiMesh* mesh = p_scene->mMeshes[p_node->mMeshes[i]];
		ProcessMesh(&nodeTransformation, mesh, p_scene, vertices, indices);
		p_meshes.push_back(new Mesh(vertices, indices, mesh->mMaterialIndex)); // The model will handle mesh destruction
	}

	// Then do the same for each of its children
	for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
	{
		ProcessNode(&nodeTransformation, p_node->mChildren[i], p_scene, p_meshes);
	}
}

void LitchiRuntime::Parsers::AssimpParser::ProcessMesh(void* p_transform, aiMesh* p_mesh, const aiScene* p_scene, std::vector<Vertex>& p_outVertices, std::vector<uint32_t>& p_outIndices)
{
	aiMatrix4x4 meshTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform);

	for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
	{
		aiVector3D position = meshTransformation * p_mesh->mVertices[i];
		aiVector3D normal = meshTransformation * (p_mesh->mNormals ? p_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		aiVector3D texCoords = p_mesh->mTextureCoords[0] ? p_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
		aiVector3D tangent = p_mesh->mTangents ? meshTransformation * p_mesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);
		aiVector3D bitangent = p_mesh->mBitangents ? meshTransformation * p_mesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);

		p_outVertices.push_back
		(
			{
				position.x,
				position.y,
				position.z,
				texCoords.x,
				texCoords.y,
				normal.x,
				normal.y,
				normal.z,
				tangent.x,
				tangent.y,
				tangent.z,
				bitangent.x,
				bitangent.y,
				bitangent.z
			}
		);
	}

	for (uint32_t faceID = 0; faceID < p_mesh->mNumFaces; ++faceID)
	{
		auto& face = p_mesh->mFaces[faceID];

		for (size_t indexID = 0; indexID < 3; ++indexID)
			p_outIndices.push_back(face.mIndices[indexID]);
	}
}

// 数据结构定义

// 关节信息
struct Bone {
	std::string name;// 关节名
	uint32_t idx_parent;// 父Bone Id
	uint32_t n_anim_keys;// 关节的动画帧列表
	uint32_t last_anim_key;// 最后一个关键帧
	glm::mat4 offset;// T-Pose矩阵 顶点到关节的变换矩阵
};

// 动画帧信息
struct AnimationClip
{
	// 骨骼列表 （父子关系）

	// 骨骼的T-Pose矩阵



	// 每一帧的骨骼的变换矩阵
};


class Animator
{
public:
	// 获取当前帧
	AnimationClip GetCurrentClip();
};

bool IsBoneOnlyNode(aiNode* node)
{
	if (node->mNumMeshes > 0)
	{
		return false;
	}

	bool haveWeFoundIt = true;
	for (int index; index < node->mNumChildren; index++)
	{
		if(!IsBoneOnlyNode(node->mChildren[index]))
		{
			haveWeFoundIt = false;
		}
	}
	return haveWeFoundIt;
}


aiNode* FindRootNode(const aiScene* scene)
{
	// 递归遍历所有的node, 找到某个node的所有子node都没有mesh的根node
	aiNode* rootNode = scene->mRootNode;
	for (int index = 0; index < rootNode->mNumChildren; index++)
	{
		aiNode* firstLevelNode = rootNode->mChildren[index];
		if (IsBoneOnlyNode(firstLevelNode))
		{
			return firstLevelNode;
		}
	}

	return nullptr;
}

void BuildBoneTree(const aiNode* rootBone, unsigned int parentNodeIndex)
{
	// 递归遍历rootBone的所有子node

	// Bone0 <- Bone1 <- Bone2 每个Bone都关联其parent Bone (<-)

}

// 加载骨骼
void LoadBones(const aiScene* scene)
{
	// 找到骨骼的根 boneRoot
	auto rootBone = FindRootNode(scene);

	// 没有骨骼就返回
	if(!rootBone)
	{
		return;
	}

	// 构建骨骼树
	BuildBoneTree(rootBone, 0);
}

// 加载动画
void LoadAnimation(const aiScene* scene,std::vector<AnimationClip>& animationClipList)
{
	
}

