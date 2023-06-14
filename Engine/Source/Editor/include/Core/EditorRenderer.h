//
//#pragma once
//#include "Runtime/Function/Renderer/material.h"
//
//namespace LitchiEditor { enum class EGizmoOperation; }
//namespace LitchiEditor { class AView; }
//using namespace LitchiRuntime;
//namespace LitchiEditor
//{
//	/**
//	* Handle the rendering of the editor
//	*/
//	class EditorRenderer
//	{
//	public:
//		/**
//		* Create the EditorRenderer
//		* @param p_context
//		*/
//		EditorRenderer(Context& p_context);
//
//		/**
//		* Initialize custom materials
//		*/
//		void InitMaterials();
//
//		/**
//		* Prepare the picking material by send it the color corresponding to the given actor
//		* @param p_actor
//		* @param p_material
//		*/
//		void PreparePickingMaterial(LitchiRuntime::GameObject& p_actor, Material*& p_material);
//
//		/**
//		* Calculate the model matrix for a camera attached to the given actor
//		* @param p_actor
//		*/
//		OvMaths::FMatrix4 CalculateCameraModelMatrix(LitchiRuntime::GameObject& p_actor);
//
//		/**
//		* Render the scene
//		* @param p_cameraPosition
//		* @param p_camera
//		*/
//		void RenderScene(const glm::vec3& p_cameraPosition, const LitchiRuntime::LowRenderer::Camera& p_camera, const LitchiRuntime::Data::Frustum* p_customFrustum = nullptr);
//
//		/**
//		* Render the scene for actor picking (Unlit version of the scene with colors indicating actor IDs)
//		*/
//		void RenderSceneForActorPicking();
//
//		/**
//		* Render the User Interface
//		*/
//		void RenderUI();
//
//		/**
//		* Render every scene cameras
//		*/
//		void RenderCameras();
//
//		/**
//		* Render every scene lights as billboards
//		*/
//		void RenderLights();
//
//		/**
//		* Render a gizmo at position
//		* @param p_position
//		* @param p_rotation
//		* @param p_operation
//		* @param p_pickable (Determine the shader to use to render the gizmo)
//		* @param p_highlightedAxis (-1 to highlight no axis, 0 for X, 1 for Y, 2 for Z)
//		*/
//		void RenderGizmo(const glm::vec3& p_position, const OvMaths::FQuaternion& p_rotation, LitchiEditor::EGizmoOperation p_operation, bool p_pickable, int p_highlightedAxis = -1);
//
//		/**
//		* Render a model to the stencil buffer
//		*/
//		void RenderModelToStencil(const OvMaths::FMatrix4& p_worldMatrix, LitchiRuntime::Model& p_model);
//
//		/**
//		* Render a model outline using the data stored in the stencil buffer
//		* @param p_worldMatrix
//		* @param p_model
//		* @param p_width
//		*/
//		void RenderModelOutline(const OvMaths::FMatrix4& p_worldMatrix, LitchiRuntime::Model& p_model, float p_width);
//
//		/**
//		* Render the actor as a selected actor (Outline)
//		* @param p_actor
//		* @param p_toStencil
//		* @param p_isSelected
//		*/
//		void RenderActorOutlinePass(LitchiRuntime::GameObject& p_actor, bool p_toStencil, bool p_isSelected = false);
//
//        /**
//        * Render the camera perspective frustum
//        * @param p_size
//        * @param p_camera
//        */
//        void RenderCameraPerspectiveFrustum(std::pair<uint16_t, uint16_t>& p_size, OvCore::ECS::Components::CCamera& p_camera);
//
//        /**
//        * Render the camera orthographic frustum
//        * @param p_size
//        * @param p_camera
//        */
//        void RenderCameraOrthographicFrustum(std::pair<uint16_t, uint16_t>& p_size, OvCore::ECS::Components::CCamera& p_camera);
//
//		/**
//		* Render the camera frustum
//		*/
//		void RenderCameraFrustum(OvCore::ECS::Components::CCamera& p_camera);
//
//		/**
//		* Render an actor collider
//		*/
//		void RenderActorCollider(LitchiRuntime::GameObject& p_actor);
//
//		/**
//		* Render light bounds
//		*/
//		void RenderLightBounds(OvCore::ECS::Components::CLight& p_light);
//
//		/**
//		* Render ambient box volume
//		*/
//		void RenderAmbientBoxVolume(OvCore::ECS::Components::CAmbientBoxLight& p_ambientBoxLight);
//
//		/**
//		* Render ambient sphere volume
//		*/
//		void RenderAmbientSphereVolume(OvCore::ECS::Components::CAmbientSphereLight& p_ambientSphereLight);
//
//		/**
//		* Render the the bounding spheres of the given model renderer
//		*/
//		void RenderBoundingSpheres(OvCore::ECS::Components::CModelRenderer& p_modelRenderer);
//
//		/**
//		* Render model
//		*/
//		void RenderModelAsset(LitchiRuntime::Model& p_model);
//
//		/**
//		* Render texture
//		*/
//		void RenderTextureAsset(LitchiRuntime::Texture& p_texture);
//
//		/**
//		* Render material
//		*/
//		void RenderMaterialAsset(Material*& p_material);
//
//		/**
//		* Render the grid
//		*/
//		void RenderGrid(const glm::vec3& p_viewPos, const glm::vec3& p_color);
//
//		/**
//		* Update the light SSBO with the current scene
//		* @param p_scene
//		*/
//		void UpdateLights(OvCore::SceneSystem::Scene& p_scene);
//
//		/**
//		* Update the light SSBO with the current scene (Lights outside of the given frustum are culled)
//		* @param p_scene
//		*/
//		void UpdateLightsInFrustum(OvCore::SceneSystem::Scene& p_scene, const LitchiRuntime::Data::Frustum& p_frustum);
//
//	private:
//
//		Material* m_gridMaterial;
//		Material* m_stencilFillMaterial;
//		Material* m_textureMaterial;
//		Material* m_outlineMaterial;
//		Material* m_emptyMaterial;
//		Material* m_defaultMaterial;
//		Material* m_cameraMaterial;
//		Material* m_lightMaterial;
//		Material* m_gizmoArrowMaterial;
//		Material* m_gizmoBallMaterial;
//		Material* m_gizmoPickingMaterial;
//		Material* m_actorPickingMaterial;
//	};
//}