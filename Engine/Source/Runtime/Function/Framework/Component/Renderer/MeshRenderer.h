
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering//Material.h"
#include "Runtime/Function/UI/ImGui/Extensions/ImGuiExtension.h"

namespace LitchiRuntime
{
	class RenderCamera;
	class MeshFilter;

	enum class MeshRendererType
	{
		MeshRenderer,
		SkinMeshRenderer
	};

	/**
	 * @brief MeshRenderer Component
	 * @note A Mesh Renderer component renders a mesh.
	 *		 It works with a Mesh Filter component on the same GameObject;
	 *		 the Mesh Renderer renders the mesh that the Mesh Filter references.
	*/
	class MeshRenderer :public Component {
	public:

		/**
		 * @brief Default Constructor
		*/
		MeshRenderer();

		/**
		 * @brief Default Destructor
		*/
		~MeshRenderer() override;

		/**
		 * @brief Set Material resource path
		 * @param materialPath
		*/
		void SetMaterialPath(std::string materialPath) { m_materialPath = materialPath; }

		/**
		 * @brief get Material resource path
		 * @return
		*/
		std::string GetMaterialPath() { return m_materialPath; }

		/**
		 * @brief Sets a material from memory (adds it to the resource cache by default)
		 * @param material 
		 * @return 
		*/
		Material* SetMaterial(Material* material);

		/**
		 * @brief Loads a material and the sets it
		 * @param filePath 
		 * @return 
		*/
		Material* SetMaterial(const std::string& filePath);

		/**
		 * @brief Set use default Material
		*/
		virtual void SetDefaultMaterial();

		/**
		 * @brief Get Material pointer
		 * @return 
		*/
		Material* GetMaterial()       const { return m_material; }

		/**
		 * @brief Get Material name
		 * @return 
		*/
		std::string GetMaterialName() const;

		/**
		 * @brief Is load Material
		 * @return 
		*/
		auto HasMaterial()            const { return m_material != nullptr; }

		/**
		 * @brief Get MeshRendererType
		 * @return
		*/
		virtual MeshRendererType GetMeshRendererType() const { return MeshRendererType::MeshRenderer; }

		/**
		 * @brief Sets whether this Renderer casts a shadow
		 * @param castShadows 
		*/
		void SetCastShadows(const bool castShadows) { m_castShadows = castShadows; }

		/**
		 * @brief Get this Renderer can casts a shadow
		 * @return 
		*/
		auto GetCastShadows() const { return m_castShadows; }

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		void PostResourceLoaded() override;

	public:

		/**
		 * @brief Called every frame
		*/
		void OnUpdate() override;

		/**
		 * @brief Called every frame, only editor mode
		*/
		void OnEditorUpdate() override;

	protected:

		/**
		 * @brief Material resource path
		*/
		std::string m_materialPath;

		/**
		 * @brief Is Use default material
		*/
		bool m_useDefaultMaterial = false;

		/**
		 * @brief Material pointer
		*/
		Material* m_material = nullptr;

		/**
		 * @brief Is open shadow
		*/
		bool m_castShadows = true;

		RTTR_ENABLE(Component)
	};
}
