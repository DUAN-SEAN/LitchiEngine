
//= INCLUDES =================================
#include "Runtime/Core/pch.h"
#include "ModelImporter.h"
#include "Runtime/Core/Tools/Utils//ProgressTracker.h"
#include "../../RHI/RHI_Texture.h"
#include "../../Rendering/Animation.h"
#include "../../Rendering/Mesh.h"
#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Renderer/Light/Light.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Function/UI/ImGui/Extensions/ImGuiExtension.h"
LC_WARNINGS_OFF
#include "assimp/scene.h"
#include "assimp/ProgressHandler.hpp"
#include "assimp/version.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
LC_WARNINGS_ON
//============================================

//= NAMESPACES ===============
using namespace std;
using namespace LitchiRuntime::Math;
using namespace Assimp;
//============================

namespace LitchiRuntime
{
	namespace
	{
		std::string model_file_path;
		std::string model_name;
		Mesh* mesh = nullptr;
		bool model_has_animation = false;
		bool model_is_gltf = false;
		const aiScene* scene = nullptr;

	}

	static Matrix convert_matrix(const aiMatrix4x4& transform)
	{
		return Matrix
		(
			transform.a1, transform.b1, transform.c1, transform.d1,
			transform.a2, transform.b2, transform.c2, transform.d2,
			transform.a3, transform.b3, transform.c3, transform.d3,
			transform.a4, transform.b4, transform.c4, transform.d4
		);
	}

	static Color convert_color(const aiColor4D& ai_color)
	{
		return Color(ai_color.r, ai_color.g, ai_color.b, ai_color.a);
	}

	static Color convert_color(const aiColor3D& ai_color)
	{
		return Color(ai_color.r, ai_color.g, ai_color.b, 1.0f);
	}

	static Vector3 convert_vector3(const aiVector3D& ai_vector)
	{
		return Vector3(ai_vector.x, ai_vector.y, ai_vector.z);
	}

	static Vector2 convert_vector2(const aiVector2D& ai_vector)
	{
		return Vector2(ai_vector.x, ai_vector.y);
	}

	static Quaternion convert_quaternion(const aiQuaternion& ai_quaternion)
	{
		return Quaternion(ai_quaternion.x, ai_quaternion.y, ai_quaternion.z, ai_quaternion.w);
	}

	static void set_entity_transform(const aiNode* node, GameObject* entity)
	{
		LC_ASSERT_MSG(node != nullptr && entity != nullptr, "Invalid parameter(s)");

		// Convert to engine matrix
		const Matrix matrix_engine = convert_matrix(node->mTransformation);

		// Apply position, rotation and scale
		entity->GetComponent<Transform>()->SetPositionLocal(matrix_engine.GetTranslation());
		entity->GetComponent<Transform>()->SetRotationLocal(matrix_engine.GetRotation());
		entity->GetComponent<Transform>()->SetScaleLocal(matrix_engine.GetScale());
	}

	constexpr void compute_node_count(const aiNode* node, uint32_t* count)
	{
		if (!node)
			return;

		(*count)++;

		// Process children
		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			compute_node_count(node->mChildren[i], count);
		}
	}

	// Implement Assimp's progress reporting interface
	class AssimpProgress : public ProgressHandler
	{
	public:
		AssimpProgress(const string& file_path)
		{
			m_file_path = file_path;
			m_file_name = FileSystem::GetFileNameFromFilePath(file_path);
		}
		~AssimpProgress() = default;

		bool Update(float percentage) override { return true; }

		void UpdateFileRead(int current_step, int number_of_steps) override
		{
			// Reading from drive file progress is ignored because it's not called in a consistent manner.
			// At least two calls are needed (start, end), but this can be called only once.
		}

		void UpdatePostProcess(int current_step, int number_of_steps) override
		{
			if (current_step == 0)
			{
				ProgressTracker::GetProgress(ProgressType::ModelImporter).JobDone(); // "Loading model from drive..."
				ProgressTracker::GetProgress(ProgressType::ModelImporter).Start(number_of_steps, "Post-processing model...");
			}
			else
			{
				ProgressTracker::GetProgress(ProgressType::ModelImporter).JobDone();
			}
		}

	private:
		string m_file_path;
		string m_file_name;
	};

	static string texture_try_multiple_extensions(const string& file_path)
	{
		// Remove extension
		const string file_path_no_ext = FileSystem::GetFilePathWithoutExtension(file_path);

		// Check if the file exists using all engine supported extensions
		for (const auto& supported_format : supported_formats_image)
		{
			string new_file_path = file_path_no_ext + supported_format;
			string new_file_path_upper = file_path_no_ext + FileSystem::ConvertToUppercase(supported_format);

			if (FileSystem::Exists(new_file_path))
			{
				return new_file_path;
			}

			if (FileSystem::Exists(new_file_path_upper))
			{
				return new_file_path_upper;
			}
		}

		return file_path;
	}

	static string texture_validate_path(string original_texture_path, const string& file_path)
	{
		// Models usually return a texture path which is relative to the model's directory.
		// However, to load anything, we'll need an absolute path, so we construct it here.
		const string model_dir = FileSystem::GetDirectoryFromFilePath(file_path);
		string full_texture_path = model_dir + original_texture_path;

		// 1. Check if the texture path is valid
		if (FileSystem::Exists(full_texture_path))
			return full_texture_path;

		// 2. Check the same texture path as previously but 
		// this time with different file extensions (jpg, png and so on).
		full_texture_path = texture_try_multiple_extensions(full_texture_path);
		if (FileSystem::Exists(full_texture_path))
			return full_texture_path;

		// At this point we know the provided path is wrong, we will make a few guesses.
		// The most common mistake is that the artist provided a path which is absolute to his computer.

		// 3. Check if the texture is in the same folder as the model
		full_texture_path = model_dir + FileSystem::GetFileNameFromFilePath(full_texture_path);
		if (FileSystem::Exists(full_texture_path))
			return full_texture_path;

		// 4. Check the same texture path as previously but 
		// this time with different file extensions (jpg, png and so on).
		full_texture_path = texture_try_multiple_extensions(full_texture_path);
		if (FileSystem::Exists(full_texture_path))
			return full_texture_path;

		// Give up, no valid texture path was found
		return "";
	}

	static bool load_material_texture(
		Mesh* mesh,
		const string& file_path,
		const bool is_gltf,
		Material* material,
		const aiMaterial* material_assimp,
		const MaterialTexture texture_type,
		const aiTextureType texture_type_assimp_pbr,
		const aiTextureType texture_type_assimp_legacy
	)
	{
		// Determine if this is a pbr material or not
		aiTextureType type_assimp = aiTextureType_NONE;
		type_assimp = material_assimp->GetTextureCount(texture_type_assimp_pbr) > 0 ? texture_type_assimp_pbr : type_assimp;
		type_assimp = (type_assimp == aiTextureType_NONE) ? (material_assimp->GetTextureCount(texture_type_assimp_legacy) > 0 ? texture_type_assimp_legacy : type_assimp) : type_assimp;

		// Check if the material has any textures
		if (material_assimp->GetTextureCount(type_assimp) == 0)
			return true;

		// Try to get the texture path
		aiString texture_path;
		if (material_assimp->GetTexture(type_assimp, 0, &texture_path) != AI_SUCCESS)
			return false;

		// See if the texture type is supported by the engine
		const string deduced_path = texture_validate_path(texture_path.data, file_path);
		if (!FileSystem::IsSupportedImageFile(deduced_path))
			return false;

		// Add the texture to the model
		mesh->AddTexture(material, texture_type, texture_validate_path(texture_path.data, file_path), is_gltf);

		// FIX: materials that have a diffuse texture should not be tinted black/gray
		if (type_assimp == aiTextureType_BASE_COLOR || type_assimp == aiTextureType_DIFFUSE)
		{
			// todo:
		   /* material->SetProperty(MaterialProperty::ColorR, 1.0f);
			material->SetProperty(MaterialProperty::ColorG, 1.0f);
			material->SetProperty(MaterialProperty::ColorB, 1.0f);
			material->SetProperty(MaterialProperty::ColorA, 1.0f);*/
		}

		// todo:
		// FIX: Some models pass a normal map as a height map and vice versa, we correct that.
	  /*  if (texture_type == MaterialTexture::Normal || texture_type == MaterialTexture::Height)
		{
			if (shared_ptr<RHI_Texture> texture = material->GetTexture_PtrShared(texture_type))
			{
				MaterialTexture proper_type = texture_type;
				proper_type = (proper_type == MaterialTexture::Normal && texture->IsGrayscale()) ? MaterialTexture::Height : proper_type;
				proper_type = (proper_type == MaterialTexture::Height && !texture->IsGrayscale()) ? MaterialTexture::Normal : proper_type;

				if (proper_type != texture_type)
				{
					material->SetTexture(texture_type, shared_ptr<RHI_Texture>(nullptr));
					material->SetTexture(proper_type, texture);
				}
			}
		}*/

		return true;
	}

	static Material* load_material(Mesh* mesh, const string& file_path, const bool is_gltf, const aiMaterial* material_assimp)
	{
		LC_ASSERT(material_assimp != nullptr);

		Material* material = new Material();

		// NAME
		aiString name;
		aiGetMaterialString(material_assimp, AI_MATKEY_NAME, &name);

		// Set a resource file path so it can be used by the resource cache
		material->SetResourceFilePath(FileSystem::RemoveIllegalCharacters(FileSystem::GetDirectoryFromFilePath(file_path) + string(name.C_Str()) + EXTENSION_MATERIAL));

		// COLOR
		aiColor4D color_diffuse(1.0f, 1.0f, 1.0f, 1.0f);
		aiGetMaterialColor(material_assimp, AI_MATKEY_COLOR_DIFFUSE, &color_diffuse);

		// OPACITY
		aiColor4D opacity(1.0f, 1.0f, 1.0f, 1.0f);
		aiGetMaterialColor(material_assimp, AI_MATKEY_OPACITY, &opacity);

		// todo:
		// Set color and opacity
	  /*  material->SetProperty(MaterialProperty::ColorR, color_diffuse.r);
		material->SetProperty(MaterialProperty::ColorG, color_diffuse.g);
		material->SetProperty(MaterialProperty::ColorB, color_diffuse.b);
		material->SetProperty(MaterialProperty::ColorA, opacity.r);*/

		//                                                                         Texture type,                Texture type Assimp (PBR),       Texture type Assimp (Legacy/fallback)
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Color, aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE);
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Roughness, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_SHININESS); // Use specular as fallback
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Metalness, aiTextureType_METALNESS, aiTextureType_AMBIENT);   // Use ambient as fallback
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Normal, aiTextureType_NORMAL_CAMERA, aiTextureType_NORMALS);
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Occlusion, aiTextureType_AMBIENT_OCCLUSION, aiTextureType_LIGHTMAP);
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Emission, aiTextureType_EMISSION_COLOR, aiTextureType_EMISSIVE);
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::Height, aiTextureType_HEIGHT, aiTextureType_NONE);
		load_material_texture(mesh, file_path, is_gltf, material, material_assimp, MaterialTexture::AlphaMask, aiTextureType_OPACITY, aiTextureType_NONE);

		// todo:
		// material->SetProperty(MaterialProperty::SingleTextureRoughnessMetalness, static_cast<float>(is_gltf));

		return material;
	}

	void ModelImporter::Initialize()
	{
		// Get version
		const int major = aiGetVersionMajor();
		const int minor = aiGetVersionMinor();
		const int rev = aiGetVersionRevision();
		// Settings::RegisterThirdPartyLib("Assimp", to_string(major) + "." + to_string(minor) + "." + to_string(rev), "https://github.com/assimp/assimp");
	}

	bool ModelImporter::Load(Mesh* mesh_in, const string& file_path)
	{
		LC_ASSERT_MSG(mesh_in != nullptr, "Invalid parameter");

		if (!FileSystem::IsFile(file_path))
		{
			DEBUG_LOG_ERROR("Provided file path doesn't point to an existing file");
			return false;
		}

		// model params
		model_file_path = file_path;
		model_name = FileSystem::GetFileNameWithoutExtensionFromFilePath(file_path);
		mesh = mesh_in;
		model_is_gltf = FileSystem::GetExtensionFromFilePath(file_path) == ".gltf";
		mesh->SetObjectName(model_name);

		// set up the importer
		Importer importer;
		{
			// remove points and lines.
			importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

			// remove cameras and lights
			{
				uint32_t component_flags = aiComponent_CAMERAS;
				if ((mesh->GetFlags() & (1U << static_cast<uint32_t>(MeshFlags::ImportLights))) == 0)
				{
					component_flags |= aiComponent_LIGHTS;
				}
				importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, component_flags);
			}

			// Enable progress tracking
			importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);
			importer.SetProgressHandler(new AssimpProgress(file_path));
		}

		// import flags
		uint32_t import_flags = 0;
		{
			import_flags |= aiProcess_ValidateDataStructure; // Validates the imported scene data structure.
			import_flags |= aiProcess_Triangulate;           // Triangulates all faces of all meshes.
			import_flags |= aiProcess_SortByPType;           // Splits meshes with more than one primitive type in homogeneous sub-meshes.

			// switch to engine conventions
			import_flags |= aiProcess_MakeLeftHanded;   // DirectX style.
			import_flags |= aiProcess_FlipUVs;          // DirectX style.
			import_flags |= aiProcess_FlipWindingOrder; // DirectX style.

			// generate missing normals or UVs
			import_flags |= aiProcess_CalcTangentSpace; // Calculates  tangents and bitangents
			import_flags |= aiProcess_GenSmoothNormals; // Ignored if the mesh already has normals
			import_flags |= aiProcess_GenUVCoords;      // Converts non-UV mappings (such as spherical or cylindrical mapping) to proper texture coordinate channels

			// Combine meshes
			if (mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::ImportCombineMeshes))
			{
				import_flags |= aiProcess_OptimizeMeshes;
				import_flags |= aiProcess_OptimizeGraph;
				import_flags |= aiProcess_PreTransformVertices;
			}

			// validate
			if (mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::ImportRemoveRedundantData))
			{
				import_flags |= aiProcess_RemoveRedundantMaterials; // Searches for redundant/unreferenced materials and removes them
				import_flags |= aiProcess_JoinIdenticalVertices;    // Identifies and joins identical vertex data sets within all imported meshes
				import_flags |= aiProcess_FindDegenerates;          // Convert degenerate primitives to proper lines or points.
				import_flags |= aiProcess_FindInvalidData;          // This step searches all meshes for invalid data, such as zeroed normal vectors or invalid UV coords and removes / fixes them
				import_flags |= aiProcess_FindInstances;            // This step searches for duplicate meshes and replaces them with references to the first mesh
			}
		}

		ProgressTracker::GetProgress(ProgressType::ModelImporter).Start(1, "Loading model from drive...");

		// read the 3D model file from drive
		if (scene = importer.ReadFile(file_path, import_flags))
		{
			// update progress tracking
			uint32_t job_count = 0;
			compute_node_count(scene->mRootNode, &job_count);
			ProgressTracker::GetProgress(ProgressType::ModelImporter).Start(job_count, "Parsing model...");

			model_has_animation = scene->mNumAnimations != 0;

			// pre parse node build node hierarchy
			if (model_has_animation)
			{
				ReadNodeHierarchy(scene->mRootNode, -1);
			}

			mesh->SetIsAnimationModel(model_has_animation);

			// recursively parse nodes
			ParseNode(scene->mRootNode);

			// update model geometry
			{
				while (ProgressTracker::GetProgress(ProgressType::ModelImporter).GetFraction() != 1.0f)
				{
					DEBUG_LOG_INFO("Waiting for node processing threads to finish before creating GPU buffers...");
					this_thread::sleep_for(std::chrono::milliseconds(16));
				}

				// optimize
				if ((mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::OptimizeVertexCache)) ||
					(mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::OptimizeVertexFetch)) ||
					(mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::OptimizeOverdraw)))
				{
					mesh->Optimize();
				}

				// aabb
				mesh->ComputeAabb();

				// normalize scale
				if (mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::ImportNormalizeScale))
				{
					float normalized_scale = mesh->ComputeNormalizedScale();
					mesh->GetRootEntity()->GetComponent<Transform>()->SetScale(normalized_scale);
				}

				mesh->CreateGpuBuffers();
			}

			// parse animation clip
			if (model_has_animation)
			{
				ParseAnimations();
			}

			// make the root entity active since it's now thread-safe
			mesh->GetRootEntity()->SetActive(true);
			// mesh->GetModelPrefab()->Resolve();
		}
		else
		{
			ProgressTracker::GetProgress(ProgressType::ModelImporter).JobDone();
			DEBUG_LOG_ERROR(importer.GetErrorString());
		}

		importer.FreeScene();
		mesh = nullptr;

		return scene != nullptr;
	}

	void ModelImporter::ReadNodeHierarchy(const aiNode* node, uint32_t parentIndex)
	{
		BoneInfo boneInfo;

		uint32_t boneIndex = mesh->GetBones().size();
		mesh->GetBoneMap()[node->mName.C_Str()] = boneIndex;

		boneInfo.boneOffset = Matrix::Identity;
		boneInfo.parentIndex = parentIndex;
		boneInfo.defaultOffset = convert_matrix(node->mTransformation);
		mesh->GetBones().push_back(boneInfo);

		for (size_t i = 0; i < node->mNumChildren; i++)
			ReadNodeHierarchy(node->mChildren[i], boneIndex);
	}

	void ModelImporter::ParseNode(const aiNode* node, GameObject* parent_entity)
	{
		// Create an entity that will match this node.
		GameObject* entity = mesh->GetModelPrefab()->CreateGameObject("Default");

		// Set root entity to mesh
		bool is_root_node = parent_entity == nullptr;
		if (is_root_node)
		{
			mesh->SetRootEntity(entity);

			// The root entity is created as inactive for thread-safety.
			entity->SetActive(false);
		}

		// Name the entity
		string node_name = is_root_node ? model_name : node->mName.C_Str();
		entity->SetObjectName(node_name);

		// Update progress tracking
		ProgressTracker::GetProgress(ProgressType::ModelImporter).SetText("Creating entity for " + entity->GetObjectName());

		// Set the transform of parent_node as the parent of the new_entity's transform
		Transform* parent_trans = parent_entity ? parent_entity->GetComponent<Transform>() : nullptr;
		entity->SetParent(parent_entity);

		// Apply node transformation
		set_entity_transform(node, entity);

		// Mesh components
		if (node->mNumMeshes > 0)
		{
			ParseNodeMeshes(node, entity);
		}

		// Light component
		if (mesh->GetFlags() & static_cast<uint32_t>(MeshFlags::ImportLights))
		{
			ParseNodeLight(node, entity);
		}

		// Children nodes
		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ParseNode(node->mChildren[i], entity);
		}

		// Update progress tracking
		ProgressTracker::GetProgress(ProgressType::ModelImporter).JobDone();
	}

	void ModelImporter::ParseNodeMeshes(const aiNode* assimp_node, GameObject* node_entity)
	{
		// An aiNode can have any number of meshes (albeit typically, it's one).
		// If it has more than one meshes, then we create children entities to store them.

		LC_ASSERT_MSG(assimp_node->mNumMeshes != 0, "No meshes to process");

		for (uint32_t i = 0; i < assimp_node->mNumMeshes; i++)
		{
			GameObject* entity = node_entity;
			aiMesh* node_mesh = scene->mMeshes[assimp_node->mMeshes[i]];
			string node_name = assimp_node->mName.C_Str();

			// if this node has more than one meshes, create an entity for each mesh, then make that entity a child of node_entity
			if (assimp_node->mNumMeshes > 1)
			{
				// Create entity
				entity = mesh->GetModelPrefab()->CreateGameObject("Node");

				// Set parent
				entity->SetParent(node_entity);

				// Set name
				node_name += "_" + to_string(i + 1); // set name
			}

			// Set entity name
			entity->SetObjectName(node_name);

			// Load the mesh onto the entity (via a Renderable component)
			ParseMesh(node_mesh, entity);
		}
	}

	void ModelImporter::ParseNodeLight(const aiNode* node, GameObject* new_entity)
	{
		for (uint32_t i = 0; i < scene->mNumLights; i++)
		{
			if (scene->mLights[i]->mName == node->mName)
			{
				// get assimp light
				const aiLight* light_assimp = scene->mLights[i];

				// add a light component
				Light* light = new_entity->AddComponent<Light>();

				// disable shadows (to avoid tanking the framerate)
				light->SetShadowsEnabled(false);
				light->SetShadowsTransparentEnabled(false);

				// color
				light->SetColor(convert_color(light_assimp->mColorDiffuse));

				// type
				if (light_assimp->mType == aiLightSource_DIRECTIONAL)
				{
					light->SetLightType(LightType::Directional);
				}
				else if (light_assimp->mType == aiLightSource_POINT)
				{
					light->SetLightType(LightType::Point);
				}
				else if (light_assimp->mType == aiLightSource_SPOT)
				{
					light->SetLightType(LightType::Spot);
				}

				// local transform
				new_entity->GetComponent<Transform>()->SetPositionLocal(convert_vector3(light_assimp->mPosition));
				new_entity->GetComponent<Transform>()->SetRotationLocal(Quaternion::FromLookRotation(convert_vector3(light_assimp->mDirection)));

				// intensity
				light->SetIntensity(LightIntensity::bulb_150_watt);
			}
		}
	}

	void ModelImporter::ParseMesh(aiMesh* assimp_mesh, GameObject* entity_parent)
	{
		LC_ASSERT(assimp_mesh != nullptr);
		LC_ASSERT(entity_parent != nullptr);

		if (model_has_animation)
		{
			ParseMeshWithBone(assimp_mesh, entity_parent);
		}
		else
		{
			ParseMeshWithoutBone(assimp_mesh, entity_parent);
		}

		// todo temp
		if (!model_has_animation)
		{
			// Create a Renderable and pass the material to it

			auto material = ApplicationBase::Instance()->materialManager->LoadResource(":Materials/Standard4Phong.mat");
			entity_parent->AddComponent<MeshRenderer>()->SetMaterial(material);;
		}
		else
		{
			// Create a Renderable and pass the material to it

			auto material = ApplicationBase::Instance()->materialManager->LoadResource(":Materials/StandardSkinn4Phong.mat");
			entity_parent->AddComponent<SkinnedMeshRenderer>()->SetMaterial(material);
		}

		// material
		if (scene->HasMaterials())
		{
			// todo:

			// get aiMaterial
			const aiMaterial* assimp_material = scene->mMaterials[assimp_mesh->mMaterialIndex];

			// convert it and add it to the model
			Material* material = load_material(mesh, model_file_path, model_is_gltf, assimp_material);

			mesh->AddMaterial(material, entity_parent);

			
		}

	}

	void ModelImporter::ParseMeshWithBone(aiMesh* assimp_mesh, GameObject* entity_parent)
	{
		// Bones
		std::vector<BoneData> vertexBoneData(assimp_mesh->mNumVertices);
		ParseBones(assimp_mesh, vertexBoneData);

		const uint32_t vertex_count = assimp_mesh->mNumVertices;
		const uint32_t index_count = assimp_mesh->mNumFaces * 3;
		// vertices
		vector<RHI_Vertex_PosTexNorTanBone> vertices = vector<RHI_Vertex_PosTexNorTanBone>(vertex_count);
		{
			for (uint32_t i = 0; i < vertex_count; i++)
			{
				RHI_Vertex_PosTexNorTanBone& vertex = vertices[i];

				// position
				const aiVector3D& pos = assimp_mesh->mVertices[i];
				vertex.pos[0] = pos.x;
				vertex.pos[1] = pos.y;
				vertex.pos[2] = pos.z;

				// normal
				if (assimp_mesh->mNormals)
				{
					const aiVector3D& normal = assimp_mesh->mNormals[i];
					vertex.nor[0] = normal.x;
					vertex.nor[1] = normal.y;
					vertex.nor[2] = normal.z;
				}

				// tangent
				if (assimp_mesh->mTangents)
				{
					const aiVector3D& tangent = assimp_mesh->mTangents[i];
					vertex.tan[0] = tangent.x;
					vertex.tan[1] = tangent.y;
					vertex.tan[2] = tangent.z;
				}

				// texture coordinates
				const uint32_t uv_channel = 0;
				if (assimp_mesh->HasTextureCoords(uv_channel))
				{
					const auto& tex_coords = assimp_mesh->mTextureCoords[uv_channel][i];
					vertex.tex[0] = tex_coords.x;
					vertex.tex[1] = tex_coords.y;
				}

				// 如果是蒙皮模型, 则存储顶点权重和骨骼索引信息
				vertex.boneWeights[0] = vertexBoneData[i].weights[0];
				vertex.boneWeights[1] = vertexBoneData[i].weights[1];
				vertex.boneWeights[2] = vertexBoneData[i].weights[2];

				for (size_t j = 0; j < NUM_BONES_PER_VERTEX; j++)
					vertex.boneIndices[j] = static_cast<float>(static_cast<int32_t>(vertexBoneData[i].boneIndex[j]));
			}
		}

		// indices
		vector<uint32_t> indices = vector<uint32_t>(index_count);
		{
			// get indices by iterating through each face of the mesh.
			for (uint32_t face_index = 0; face_index < assimp_mesh->mNumFaces; face_index++)
			{
				// if (aiPrimitiveType_LINE | aiPrimitiveType_POINT) && aiProcess_Triangulate) then (face.mNumIndices == 3)
				const aiFace& face = assimp_mesh->mFaces[face_index];
				const uint32_t indices_index = (face_index * 3);
				indices[indices_index + 0] = face.mIndices[0];
				indices[indices_index + 1] = face.mIndices[1];
				indices[indices_index + 2] = face.mIndices[2];
			}
		}

		// compute AABB (before doing move operation on vertices)
		const BoundingBox aabb = BoundingBox(vertices.data(), static_cast<uint32_t>(vertices.size()));

		// add vertex and index data to the mesh
		uint32_t index_offset = 0;
		uint32_t vertex_offset = 0;
		mesh->AddIndices(indices, &index_offset);
		mesh->AddVertices(vertices, &vertex_offset);
		SubMesh subMesh;
		subMesh.m_geometryIndexOffset = index_offset;
		subMesh.m_geometryIndexCount = indices.size();
		subMesh.m_geometryVertexOffset = vertex_offset;
		subMesh.m_geometryVertexCount = vertices.size();
		int subMeshIndex;
		mesh->AddSubMesh(subMesh, subMeshIndex);
		// add a renderable component to this entity
		MeshFilter* renderable  = entity_parent->AddComponent<MeshFilter>();

		// set the geometry
		renderable->SetGeometry(mesh, subMeshIndex, aabb);
	}

	void ModelImporter::ParseMeshWithoutBone(aiMesh* assimp_mesh, GameObject* entity_parent)
	{
		const uint32_t vertex_count = assimp_mesh->mNumVertices;
		const uint32_t index_count = assimp_mesh->mNumFaces * 3;
		// vertices
		vector<RHI_Vertex_PosTexNorTan> vertices = vector<RHI_Vertex_PosTexNorTan>(vertex_count);
		{
			for (uint32_t i = 0; i < vertex_count; i++)
			{
				RHI_Vertex_PosTexNorTan& vertex = vertices[i];

				// position
				const aiVector3D& pos = assimp_mesh->mVertices[i];
				vertex.pos[0] = pos.x;
				vertex.pos[1] = pos.y;
				vertex.pos[2] = pos.z;

				// normal
				if (assimp_mesh->mNormals)
				{
					const aiVector3D& normal = assimp_mesh->mNormals[i];
					vertex.nor[0] = normal.x;
					vertex.nor[1] = normal.y;
					vertex.nor[2] = normal.z;
				}

				// tangent
				if (assimp_mesh->mTangents)
				{
					const aiVector3D& tangent = assimp_mesh->mTangents[i];
					vertex.tan[0] = tangent.x;
					vertex.tan[1] = tangent.y;
					vertex.tan[2] = tangent.z;
				}

				// texture coordinates
				const uint32_t uv_channel = 0;
				if (assimp_mesh->HasTextureCoords(uv_channel))
				{
					const auto& tex_coords = assimp_mesh->mTextureCoords[uv_channel][i];
					vertex.tex[0] = tex_coords.x;
					vertex.tex[1] = tex_coords.y;
				}
			}
		}

		// indices
		vector<uint32_t> indices = vector<uint32_t>(index_count);
		{
			// get indices by iterating through each face of the mesh.
			for (uint32_t face_index = 0; face_index < assimp_mesh->mNumFaces; face_index++)
			{
				// if (aiPrimitiveType_LINE | aiPrimitiveType_POINT) && aiProcess_Triangulate) then (face.mNumIndices == 3)
				const aiFace& face = assimp_mesh->mFaces[face_index];
				const uint32_t indices_index = (face_index * 3);
				indices[indices_index + 0] = face.mIndices[0];
				indices[indices_index + 1] = face.mIndices[1];
				indices[indices_index + 2] = face.mIndices[2];
			}
		}

		// compute AABB (before doing move operation on vertices)
		const BoundingBox aabb = BoundingBox(vertices.data(), static_cast<uint32_t>(vertices.size()));

		// add vertex and index data to the mesh
		uint32_t index_offset = 0;
		uint32_t vertex_offset = 0;
		mesh->AddIndices(indices, &index_offset);
		mesh->AddVertices(vertices, &vertex_offset);
		SubMesh subMesh;
		subMesh.m_geometryIndexOffset = index_offset;
		subMesh.m_geometryIndexCount = indices.size();
		subMesh.m_geometryVertexOffset = vertex_offset;
		subMesh.m_geometryVertexCount = vertices.size();
		int subMeshIndex;
		mesh->AddSubMesh(subMesh, subMeshIndex);
		// add a renderable component to this entity
		MeshFilter* renderable = nullptr;
		//if(model_has_animation)
		{
			renderable = entity_parent->AddComponent<MeshFilter>();
		}

		// set the geometry
		renderable->SetGeometry(mesh, subMeshIndex, aabb);

	}

	void ModelImporter::ParseBones(const aiMesh* assimp_mesh, std::vector<BoneData>& boneData)
	{
		for (size_t i = 0; i < assimp_mesh->mNumBones; i++) {
			uint32_t boneIndex = 0;
			std::string boneName(assimp_mesh->mBones[i]->mName.C_Str());

			if (mesh->GetBoneMap().find(boneName) == mesh->GetBoneMap().end()) {
				//insert error program
				DEBUG_LOG_ERROR("cannot find node");
			}
			else
				boneIndex = mesh->GetBoneMap()[boneName];

			mesh->GetBoneMap()[boneName] = boneIndex;

			if (!mesh->GetBones()[boneIndex].isSkinned) {
				aiMatrix4x4 offsetMatrix = assimp_mesh->mBones[i]->mOffsetMatrix;
				mesh->GetBones()[boneIndex].boneOffset = convert_matrix(offsetMatrix);
				mesh->GetBones()[boneIndex].isSkinned = true;
			}

			for (size_t j = 0; j < assimp_mesh->mBones[i]->mNumWeights; j++) {
				uint32_t vertexID = assimp_mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = assimp_mesh->mBones[i]->mWeights[j].mWeight;
				boneData[vertexID].Add(boneIndex, weight);
			}
		}
	}

	void ModelImporter::ParseAnimations()
	{
		for (size_t i = 0; i < scene->mNumAnimations; i++) {
			AnimationClip animation;
			std::vector<BoneAnimation> boneAnims(mesh->GetBones().size());
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
				boneAnims[mesh->GetBoneMap()[nodeAnim->mNodeName.C_Str()]] = boneAnim;
			}
			animation.boneAnimations = boneAnims;

			std::string animName(anim->mName.C_Str());
			animName = animName.substr(animName.find_last_of('|') + 1, animName.length() - 1);

			mesh->GetAnimationClipMap()[animName] = animation;
		}
	}

}
