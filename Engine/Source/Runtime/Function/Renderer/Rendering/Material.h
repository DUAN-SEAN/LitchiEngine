
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
			return UniformType::UNIFORM_Texture;
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
		std::string vertexShaderPath;
		std::string piexlShaderPath;
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
		void SetTexture(const int slot, RHI_Texture* texture);

		/* Set Variable with not resource */
		void SetValue(const int slot, const std::vector<std::string>& pathArr, const UniformType uniformType, const std::any value);

		MaterialRes* GetMaterialRes() { return m_materialRes; }
		RHI_Shader* GetVertexShader() { return m_vertex_shader; }
		RHI_Shader* GetPixelShader() { return m_piexl_shader; }

	private:

		/* serialize data */
		MaterialRes* m_materialRes;

		/* shader */
		RHI_Shader* m_vertex_shader;
		RHI_Shader* m_piexl_shader;

	};
}
