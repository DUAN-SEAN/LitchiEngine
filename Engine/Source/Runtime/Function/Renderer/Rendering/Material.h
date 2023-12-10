
#pragma once

//= INCLUDES ======================
#include <any>
#include <memory>
#include <array>

#include "MaterialShader.h"
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
		std::map<std::string, std::any>& GetUniformsData() { return m_uniformDataList; }

		void* GetValues4DescriptorSet(uint32_t& size);
		std::shared_ptr<RHI_ConstantBuffer> GetValuesCBuffer() { return m_valueConstantBuffer; }
		std::map<int, RHI_Texture*> GetTextures4DescriptorSet();

		void PostResourceModify() override;
		void PostResourceLoaded() override;

		void Tick();

	private:

		void UpdateValue(const std::string& name);
		int CalcValueSize();

		/* shader */
		MaterialShader* m_shader = nullptr;

		std::shared_ptr<RHI_ConstantBuffer> m_valueConstantBuffer;

		/* material resource */
		std::map<std::string, std::any> m_uniformDataList;

		/* global cbuffer */
		void* m_value = nullptr;
		int m_valueSize = 0;

		bool m_isValueDirty = true;

	private:

		/* serialize data */
		MaterialRes* m_materialRes;
		RTTR_ENABLE(Object)
	};

	template<typename T>
	inline void Material::SetValue(const std::string& name, const T& value)
	{
		if (m_uniformDataList.find(name) != m_uniformDataList.end())
		{
			m_uniformDataList[name] = std::any(value);
			UpdateValue(name);
		}
	}

	template<typename T>
	inline const T& Material::GetValue(const std::string p_key)
	{
		if (m_uniformDataList.find(p_key) != m_uniformDataList.end())
			return T();
		else
			return std::any_cast<T>(m_uniformDataList.at(p_key));
	}

	/*
	 * standard pbr
	 *
	 *
	 */
}
