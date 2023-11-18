
#pragma once

//= INCLUDES ======================
#include <any>
#include <memory>
#include <array>
#include "../RHI/RHI_Definitions.h"
#include "../Resource/IResource.h"
#include "Runtime/Function/Renderer/Resources/UniformType.h"
#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Vector4.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Function/Renderer/Resources/ShaderUniform.h"
#include "Runtime/Function/Renderer/RHI/RHI_Descriptor.h"
//=================================

namespace LitchiRuntime
{
	class UniformInfoBase
	{
	public:
		std::string name;
		virtual UniformType GetUniformType() {
			return UniformType
				::UNIFORM_Unknown;
		}
		RTTR_ENABLE()
	};


	class UniformInfoBool :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_BOOL;
		}
		float value;
		RTTR_ENABLE(UniformInfoBase)
	};


	class UniformInfoInt :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_INT;
		}
		int value;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoFloat :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_FLOAT;
		}
		float value;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoVector2 :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_FLOAT_VEC2;
		}
		Vector2 vector;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoVector3 :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_FLOAT_VEC3;
		}
		Vector3 vector;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoVector4 :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_FLOAT_VEC4;
		}
		LitchiRuntime::Vector4 vector;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoTexture :public UniformInfoBase
	{
	public:
		UniformType GetUniformType() override
		{
			return UniformType::UNIFORM_TEXTURE;
		}
		std::string path;
		RTTR_ENABLE(UniformInfoBase)
	};

	class MaterialResSetting
	{
	public:
		bool blendable;
		bool backfaceCulling;
		bool depthTest;
		int gpuInstances;

		RTTR_ENABLE()
	};


	class MaterialRes
	{
	public:
		std::string shaderPath;
		std::vector<UniformInfoBase*> uniformInfoList;

		RTTR_ENABLE()
	};

	enum class MaterialTexture
	{
		Color,
		Roughness,  // Specifies microfacet roughness of the surface for diffuse and specular reflection
		Metalness, // Blends between a non-metallic and metallic material model
		Normal,
		Occlusion,  // A texture that will be mixed with ssao.
		Emission,   // A texture that will cause a surface to be lit, works nice with bloom.
		Height,     // Perceived depth for parallax mapping.
		AlphaMask,  // A texture which will use pixel shader discards for transparent pixels.
		Undefined
	};

	enum class MaterialProperty
	{
		Clearcoat,           // Extra white specular layer on top of others
		Clearcoat_Roughness, // Roughness of clearcoat specular
		Anisotropic,         // Amount of anisotropy for specular reflection
		AnisotropicRotation, // Rotates the direction of anisotropy, with 1.0 going full circle
		Sheen,              // Amount of soft velvet like reflection near edges
		SheenTint,           // Mix between white and using base color for sheen reflection
		ColorTint,           // Diffuse or metal surface color
		ColorR,
		ColorG,
		ColorB,
		ColorA,
		RoughnessMultiplier,
		MetalnessMultiplier,
		NormalMultiplier,
		HeightMultiplier,
		UvTilingX,
		UvTilingY,
		UvOffsetX,
		UvOffsetY,
		SingleTextureRoughnessMetalness,
		CanBeEdited,
		Undefined
	};

	/* composite RHI_Shader Group */
	struct MaterialShader
	{
		MaterialShader(std::string shaderPath, RHI_Shader* vertex_shader, RHI_Shader* pixel_shader)
		{
			m_shaderPath = shaderPath;
			m_vertex_shader = vertex_shader;
			m_pixel_shader = pixel_shader;
		}

		/* Global Uniform  */
		const ShaderUniform GetGlobalUniformInfo(std::string name);
		const std::vector<ShaderUniform> GetGlobalShaderUniformList();
		const RHI_Descriptor& GetTextureDescriptor(std::string name);
		const std::vector<RHI_Descriptor> GetTextureDescriptorList();
		int GetGlobalSize();

		std::string m_shaderPath;

		/* shader */
		RHI_Shader* m_vertex_shader;
		RHI_Shader* m_pixel_shader;

		RHI_Descriptor m_invalidTextureDescriptor;
	};

	/**
	 * \brief only support simple struct global cbuffer as material data
	 */
	class SP_CLASS Material : public IResource
	{
	public:
		Material();
		~Material() = default;

		//= IResource ===========================================
		bool LoadFromFile(const std::string& file_path) override;
		bool SaveToFile(const std::string& file_path) override;

		/* Set Texture Resource */
		void SetTexture(const std::string& name, RHI_Texture* texture);

		/* Set Variable with not resource */
		template<typename T> void SetValue(const std::string& name, const T& value);

		/**
		* Set a shader uniform value
		* @param p_key
		*/
		template<typename T> const T& GetValue(const std::string p_key);

		MaterialRes* GetMaterialRes() { return m_materialRes; }
		RHI_Shader* GetVertexShader() { return m_shader->m_vertex_shader; }
		RHI_Shader* GetPixelShader() { return m_shader->m_pixel_shader; }
		void SetShader(MaterialShader* shader);
		MaterialShader* GetShader() { return m_shader; }
		std::map<std::string, std::any>& GetUniformsData() { return m_uniformsData; }

		void* GetValues4DescriptorSet();
		std::map<int, RHI_Texture*>& GetTextures4DescriptorSet();

		void PostResourceModify() override;
		void PostResourceLoaded() override;

		static Material* CreateMaterial4StandardPBR();
		static Material* CreateMaterial4StandardPhong();

	private:

		void UpdateValue(const std::string& name);
		int CalcValueSize();

		/* shader */
		MaterialShader* m_shader = nullptr;

		/* material resource */
		std::map<std::string, std::any> m_uniformsData;

		/* global cbuffer */
		void* m_value = nullptr;
		int m_valueSize = 0;

	private:

		/* serialize data */
		MaterialRes* m_materialRes;
		RTTR_ENABLE(Object)
	};

	template<typename T>
	inline void Material::SetValue(const std::string& name, const T& value)
	{
		if (m_uniformsData.find(name) != m_uniformsData.end())
		{
			m_uniformsData[name] = std::any(value);
			UpdateValue(name);
		}
	}

	template<typename T>
	inline const T& Material::GetValue(const std::string p_key)
	{
		if (m_uniformsData.find(p_key) != m_uniformsData.end())
			return T();
		else
			return std::any_cast<T>(m_uniformsData.at(p_key));
	}

	/*
	 * standard pbr
	 *
	 *
	 */
}
