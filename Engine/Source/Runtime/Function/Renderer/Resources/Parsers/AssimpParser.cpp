
#include "AssimpParser.h"

#include "Runtime/Core/Log/debug.h"

bool LitchiRuntime::Parsers::AssimpParser::LoadModel(
	const std::string& p_fileName,
	Model* p_model,
	EModelParserFlags p_parserFlags
)
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

	bool isSkinned = scene->HasAnimations();// 暂时用有没有动画来表示这个模型是不是支持蒙皮动画的模型

	ReadNodeHierarchy(p_model, scene->mRootNode, -1);
	ProcessMaterials(p_model, scene);

	aiMatrix4x4 identity;

	ProcessNode(p_model, &identity, scene->mRootNode, scene, isSkinned);

	// 加载动画
	LoadAnimations(p_model, scene);

	// 释放场景
	// aiReleaseImport(scene);

	return true;
}

void LitchiRuntime::Parsers::AssimpParser::ReadNodeHierarchy(Model* model, const aiNode* node, uint32_t parentIndex)
{
	Model::BoneInfo boneInfo;

	uint32_t boneIndex = model->boneInfoArr.size();
	model->boneMap[node->mName.C_Str()] = boneIndex;

	boneInfo.boneOffset = glm::mat4(1.0f);
	boneInfo.parentIndex = parentIndex;
	boneInfo.defaultOffset = {
		node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
		node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
		node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
		node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4
	};
	model->boneInfoArr.push_back(boneInfo);

	for (size_t i = 0; i < node->mNumChildren; i++)
		ReadNodeHierarchy(model, node->mChildren[i], boneIndex);
}

void LitchiRuntime::Parsers::AssimpParser::LoadBones(Model* model, const aiMesh* mesh, std::vector<Model::BoneData>& boneData)
{
	for (size_t i = 0; i < mesh->mNumBones; i++) {
		uint32_t boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.C_Str());

		if (model->boneMap.find(boneName) == model->boneMap.end()) {
			//insert error program
			DEBUG_LOG_ERROR("cannot find node");
		}
		else
			boneIndex = model->boneMap[boneName];

		model->boneMap[boneName] = boneIndex;

		if (!model->boneInfoArr[boneIndex].isSkinned) {
			aiMatrix4x4 offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
			model->boneInfoArr[boneIndex].boneOffset = {
				offsetMatrix.a1, offsetMatrix.b1, offsetMatrix.c1, offsetMatrix.d1,
				offsetMatrix.a2, offsetMatrix.b2, offsetMatrix.c2, offsetMatrix.d2,
				offsetMatrix.a3, offsetMatrix.b3, offsetMatrix.c3, offsetMatrix.d3,
				offsetMatrix.a4, offsetMatrix.b4, offsetMatrix.c4, offsetMatrix.d4
			};
			model->boneInfoArr[boneIndex].isSkinned = true;
		}

		for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			uint32_t vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			boneData[vertexID].Add(boneIndex, weight);
		}
	}
}

void LitchiRuntime::Parsers::AssimpParser::ProcessMaterials(Model* model, const struct aiScene* p_scene)
{
	for (uint32_t i = 0; i < p_scene->mNumMaterials; ++i)
	{
		aiMaterial* material = p_scene->mMaterials[i];
		if (material)
		{
			aiString name;
			aiGetMaterialString(material, AI_MATKEY_NAME, &name);
			model->materialNameArr.push_back(name.C_Str());
		}
	}
}

void LitchiRuntime::Parsers::AssimpParser::ProcessNode(Model* model, void* p_transform, struct aiNode* p_node, const struct aiScene* p_scene, bool isSkinned)
{
	aiMatrix4x4 nodeTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform) * p_node->mTransformation;

	// Process all the node's meshes (if any)
	for (uint32_t i = 0; i < p_node->mNumMeshes; ++i)
	{
		aiMesh* aiMesh = p_scene->mMeshes[p_node->mMeshes[i]];
		model->mesheArr.push_back(ProcessMesh(model, &nodeTransformation, isSkinned, aiMesh, p_scene, aiMesh->mMaterialIndex)); // The model will handle mesh destruction
	}

	// Then do the same for each of its children
	for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
	{
		ProcessNode(model, &nodeTransformation, p_node->mChildren[i], p_scene, isSkinned);
	}
}

LitchiRuntime::Mesh* LitchiRuntime::Parsers::AssimpParser::ProcessMesh(Model* model, void* p_transform, bool isSkinned, struct aiMesh* p_mesh, const struct aiScene* p_scene, unsigned materialIndex)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	aiMatrix4x4 meshTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform);

	std::vector<Model::BoneData> vertexBoneData(p_mesh->mNumVertices);

	if (isSkinned)
	{
		LoadBones(model, p_mesh, vertexBoneData);
	}

	for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
	{
		aiVector3D position = p_mesh->mVertices[i];
		aiVector3D normal = (p_mesh->mNormals ? p_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		aiVector3D texCoords = p_mesh->mTextureCoords[0] ? p_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
		aiVector3D tangent = p_mesh->mTangents ? p_mesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);
		aiVector3D bitangent = p_mesh->mBitangents ? p_mesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);

		// 如果不包含蒙皮 简单的将mesh的顶点坐标切换到模型坐标系中
		if (!isSkinned)
		{
			position = meshTransformation * position;
			normal = meshTransformation * normal;
			tangent = meshTransformation * tangent;
			bitangent = meshTransformation * bitangent;
		}

		Vertex vertex = {
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
		};

		// 如果是蒙皮模型, 则存储顶点权重和骨骼索引信息
		if (isSkinned)
		{
			vertex.boneWeights[0] = vertexBoneData[i].weights[0];
			vertex.boneWeights[1] = vertexBoneData[i].weights[1];
			vertex.boneWeights[2] = vertexBoneData[i].weights[2];

			for (size_t j = 0; j < NUM_BONES_PER_VERTEX; j++)
				vertex.boneIndices[j] = static_cast<int32_t>(vertexBoneData[i].boneIndex[j]);
		}
		vertices.push_back(vertex);

	}

	for (uint32_t faceID = 0; faceID < p_mesh->mNumFaces; ++faceID)
	{
		auto& face = p_mesh->mFaces[faceID];

		for (size_t indexID = 0; indexID < 3; ++indexID)
			indices.push_back(face.mIndices[indexID]);
	}

	return new Mesh(vertices, indices, materialIndex, isSkinned);
}

void LitchiRuntime::Parsers::AssimpParser::LoadAnimations(Model* model, const aiScene* scene)
{
	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		AnimationClip animation;
		std::vector<BoneAnimation> boneAnims(model->boneInfoArr.size());
		aiAnimation* anim = scene->mAnimations[i];

		float ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f;
		float timeInTicks = 1.0f / ticksPerSecond;

		for (size_t j = 0; j < anim->mNumChannels; j++) {
			BoneAnimation boneAnim;
			aiNodeAnim* nodeAnim = anim->mChannels[j];

			for (size_t k = 0; k < nodeAnim->mNumPositionKeys; k++) {
				VectorKey keyframe;
				keyframe.timePos = nodeAnim->mPositionKeys[k].mTime * timeInTicks;
				keyframe.value.x = nodeAnim->mPositionKeys[k].mValue.x;
				keyframe.value.y = nodeAnim->mPositionKeys[k].mValue.y;
				keyframe.value.z = nodeAnim->mPositionKeys[k].mValue.z;
				boneAnim.translation.push_back(keyframe);
			}
			for (size_t k = 0; k < nodeAnim->mNumScalingKeys; k++) {
				VectorKey keyframe;
				keyframe.timePos = nodeAnim->mScalingKeys[k].mTime * timeInTicks;
				keyframe.value.x = nodeAnim->mScalingKeys[k].mValue.x;
				keyframe.value.y = nodeAnim->mScalingKeys[k].mValue.y;
				keyframe.value.z = nodeAnim->mScalingKeys[k].mValue.z;
				boneAnim.scale.push_back(keyframe);
			}
			for (size_t k = 0; k < nodeAnim->mNumRotationKeys; k++) {
				QuatKey keyframe;
				keyframe.timePos = nodeAnim->mRotationKeys[k].mTime * timeInTicks;
				keyframe.value.x = nodeAnim->mRotationKeys[k].mValue.x;
				keyframe.value.y = nodeAnim->mRotationKeys[k].mValue.y;
				keyframe.value.z = nodeAnim->mRotationKeys[k].mValue.z;
				keyframe.value.w = nodeAnim->mRotationKeys[k].mValue.w;
				boneAnim.rotationQuat.push_back(keyframe);
			}
			boneAnims[model->boneMap[nodeAnim->mNodeName.C_Str()]] = boneAnim;
		}
		animation.boneAnimations = boneAnims;

		std::string animName(anim->mName.C_Str());
		animName = animName.substr(animName.find_last_of('|') + 1, animName.length() - 1);

		model->animationClipMap[animName] = animation;
	}
}
