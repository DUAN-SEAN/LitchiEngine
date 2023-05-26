
#pragma once

#include <any>
#include <map>

#include "Shader.h"
#include "Texture.h"
#include "Runtime/Core/Meta/Reflection/object.h"
#include "Runtime/Function/Renderer/shader.h"

namespace LitchiRuntime::Resource
{
	class UniformInfoBase
	{
	public:
		std::string name;
		RTTR_ENABLE()
	};

	class UniformInfoVector4 :public UniformInfoBase
	{
	public:
		glm::vec4 vector;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoPath :public UniformInfoBase
	{
	public:
		std::string path;
		RTTR_ENABLE(UniformInfoBase)
	};

	class UniformInfoFloat :public UniformInfoBase
	{
	public:
		float value;
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
		MaterialResSetting settings;
		std::vector<UniformInfoBase*> uniformInfoList;

		RTTR_ENABLE()
	};


	/**
	* A material is a combination of a shader and some settings (Material settings and shader settings)
	*/
	class Material : public Object
	{
	public:
		/**
		* Defines the shader to attach to this material instance
		* @param p_shader
		*/
		void SetShader(LitchiRuntime::Resource::Shader* p_shader);

		/**
		* Fill uniform with default uniform values
		*/
		void FillUniform();

		/**
		* Bind the material and send its uniform data to the GPU
		* @parma p_emptyTexture (The texture to use if a texture uniform is nullptr)
		*/
		void Bind(LitchiRuntime::Texture* p_emptyTexture);

		/**
		* Unbind the material
		*/
		void UnBind();

		/**
		* Set a shader uniform value
		* @param p_key
		* @param p_value
		*/
		template<typename T> void Set(const std::string p_key, const T& p_value);

		/**
		* Set a shader uniform value
		* @param p_key
		*/
		template<typename T> const T& Get(const std::string p_key);

		/**
		* Returns the attached shader
		*/
		LitchiRuntime::Resource::Shader*& GetShader();

		/**
		* Returns true if the material has a shader attached
		*/
		bool HasShader() const;

		/**
		* Defines if the material is blendable
		* @param p_blendable
		*/
		void SetBlendable(bool p_blendable);

		/**
		* Defines if the material has backface culling
		* @param p_backfaceCulling
		*/
		void SetBackfaceCulling(bool p_backfaceCulling);

		/**
		* Defines if the material has frontface culling
		* @param p_frontfaceCulling
		*/
		void SetFrontfaceCulling(bool p_frontfaceCulling);

		/**
		* Defines if the material has depth test
		* @param p_depthTest
		*/
		void SetDepthTest(bool p_depthTest);

		/**
		* Defines if the material has depth writting
		* @param p_depthWriting
		*/
		void SetDepthWriting(bool p_depthWriting);

		/**
		* Defines if the material has color writting
		* @param p_colorWriting
		*/
		void SetColorWriting(bool p_colorWriting);

		/**
		* Defines the number of instances
		* @param p_instances
		*/
		void SetGPUInstances(int p_instances);

		/**
		* Returns true if the material is blendable
		*/
		bool IsBlendable() const;

		/**
		* Returns true if the material has backface culling
		*/
		bool HasBackfaceCulling() const;

		/**
		* Returns true if the material has frontface culling
		*/
		bool HasFrontfaceCulling() const;

		/**
		* Returns true if the material has depth test
		*/
		bool HasDepthTest() const;

		/**
		* Returns true if the material has depth writing
		*/
		bool HasDepthWriting() const;

		/**
		* Returns true if the material has color writing
		*/
		bool HasColorWriting() const;

		/**
		* Returns the number of instances
		*/
		int GetGPUInstances() const;

		/**
		* Generate an OpenGL state mask with the current material settings
		*/
		uint8_t GenerateStateMask() const;

		/**
		* Returns the uniforms data of the material
		*/
		std::map<std::string, std::any>& GetUniformsData();

	public:

		void PostResourceModify() override;

		void PostResourceLoaded() override;

		MaterialRes* materialRes;

		const std::string path;

		RTTR_ENABLE(Object)

	private:
		LitchiRuntime::Resource::Shader* m_shader = nullptr;
		std::map<std::string, std::any> m_uniformsData;

		bool m_blendable = false;
		bool m_backfaceCulling = true;
		bool m_frontfaceCulling = false;
		bool m_depthTest = true;
		bool m_depthWriting = true;
		bool m_colorWriting = true;
		int m_gpuInstances = 1;
	};
}

#include "Material.inl"