#pragma once

#include <rttr/registration.h>

#include "Runtime/Core/Meta/Reflection/object.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Gameplay/ThirdPersonController.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshFilter.h"
#include "Runtime/Function/Framework/Component/Renderer/MeshRenderer.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

#include "Runtime/Function/Framework/Component/Physcis/BoxCollider.h"
#include "Runtime/Function/Framework/Component/Physcis/CapsuleCollider.h"
#include "Runtime/Function/Framework/Component/Physcis/CharacterController.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidActor.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidDynamic.h"
#include "Runtime/Function/Framework/Component/Physcis/RigidStatic.h"
#include "Runtime/Function/Framework/Component/Physcis/SphereCollider.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Renderer/SkinnedMeshRenderer.h"
#include "Runtime/Function/Framework/Component/Script/ScriptComponent.h"
#include "Runtime/Function/Framework/Component/UI/RectTransform.h"
#include "Runtime/Function/Framework/Component/UI/UICanvas.h"
#include "Runtime/Function/Framework/Component/UI/UIImage.h"
#include "Runtime/Function/Framework/Component/UI/UIText.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Prefab/Prefab.h"
#include "Runtime/Function/Scene/SceneManager.h"
#include "Runtime/Resource/ConfigManager.h"

using namespace rttr;
namespace LitchiRuntime {
	RTTR_REGISTRATION 
	{ 
		/* Basic Struct Types */
		registration::class_<Vector2>("Vec2")
			.constructor()
			.property("x", &Vector2::x)
			.property("y", &Vector2::y);

		registration::class_<Vector3>("Vec3")
			.constructor()
			.property("x", &Vector3::x)
			.property("y", &Vector3::y)
			.property("z", &Vector3::z);

		registration::class_<Vector4>("Vec4")
			.constructor()
			.property("x", &Vector4::x)
			.property("y", &Vector4::y)
			.property("z", &Vector4::z)
			.property("w", &Vector4::w);

		registration::class_<Vector2>("Vector2")
			.constructor()
			.property("x", &Vector2::x)
			.property("y", &Vector2::y);

		registration::class_<Vector3>("Vector3")
			.constructor()
			.property("x", &Vector3::x)
			.property("y", &Vector3::y)
			.property("z", &Vector3::z);

		registration::class_<Vector4>("Vector4")
			.constructor()
			.property("x", &Vector4::x)
			.property("y", &Vector4::y)
			.property("z", &Vector4::z)
			.property("w", &Vector4::w);

		registration::class_<Quaternion>("Quaternion")
			.constructor()
			.property("x", &Quaternion::x)
			.property("y", &Quaternion::y)
			.property("z", &Quaternion::z)
			.property("w", &Quaternion::w);

		registration::class_<Color>("Color")
			.constructor()
			.property("r", &Color::r)
			.property("g", &Color::g)
			.property("b", &Color::b)
			.property("a", &Color::a);

		// Camera param
		registration::class_<CameraLightDesc>("CameraLightDesc")
			.constructor()
			.property("aperture", &CameraLightDesc::m_aperture)
			.property("shutter speed", &CameraLightDesc::m_shutter_speed)
			.property("iso", &CameraLightDesc::m_iso);

		registration::class_<CameraViewport>("Viewport")
			.constructor()
			.property("pos", &CameraViewport::m_pos)
			.property("size", &CameraViewport::m_size);

		/* Basic Enum Types Start */

		registration::enumeration<ClearFlags>("ClearFlags")
			(value("SolidColor", ClearFlags::SolidColor),
			value("SkyBox", ClearFlags::SkyBox),
			value("DepthOnly", ClearFlags::DepthOnly),
			value("DontClear", ClearFlags::DontClear));


		registration::enumeration<ProjectionType>("ProjectionType")
			(value("Perspective", ProjectionType::Projection_Perspective),
				value("Orthographic", ProjectionType::Projection_Orthographic));

		registration::enumeration<LightType>("LightType")(value("Directional", LightType::Directional),
														  value("Spot", LightType::Spot),
														  value("Point", LightType::Point));

		registration::enumeration<RHI_Vertex_Type>("VertexType")(
			value("Undefined", RHI_Vertex_Type::Undefined), 
			value("Pos", RHI_Vertex_Type::Pos),
			value("PosCol", RHI_Vertex_Type::PosCol), 
			value("PosUv", RHI_Vertex_Type::PosUv),
			value("PosUvNorTan", RHI_Vertex_Type::PosUvNorTan),
			value("Pos2dUvCol8", RHI_Vertex_Type::Pos2dUvCol8),
			value("PosUvNorTanBone", RHI_Vertex_Type::PosUvNorTanBone));

		/* Basic Enum Types End */

		/* Resource Types Start */

		registration::class_<PhysicMaterialRes>("PhysicMaterialRes")
			.constructor<>()
			.property("dynamicFriction", &PhysicMaterialRes::GetDynamicFriction,
					  &PhysicMaterialRes::SetDynamicFriction)
			.property("restitution", &PhysicMaterialRes::GetRestitution,
					  &PhysicMaterialRes::SetRestitution)
			.property("staticFriction", &PhysicMaterialRes::GetStaticFriction,
					  &PhysicMaterialRes::SetStaticFriction);

		registration::class_<ConfigRes>("ConfigRes")
			.constructor<>()
			.property("assetFolder", &ConfigRes::m_asset_folder)
			.property("scriptFolder", &ConfigRes::m_script_folder)
			.property("defaultScenePath", &ConfigRes::m_default_scene_path)
			.property("resolutionWidth", &ConfigRes::m_resolution_width)
			.property("resolutionHeight", &ConfigRes::m_resolution_height)
			.property("isFullScreen", &ConfigRes::m_is_fullScreen)
			.property("data", &ConfigRes::m_data);

		registration::class_<UniformInfoBase>("UniformInfoBase")(rttr::metadata("Serializable", true),
																 rttr::metadata("Polymorphic", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("name", &UniformInfoBase::name);

		registration::class_<UniformInfoBool>("UniformInfoBool")(rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("value", &UniformInfoBool::value);

		registration::class_<UniformInfoInt>("UniformInfoInt")(rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("value", &UniformInfoInt::value);

		registration::class_<UniformInfoFloat>("UniformInfoFloat")(rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("value", &UniformInfoFloat::value);

		registration::class_<UniformInfoVector2>("UniformInfoVector2")(
			rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("vector", &UniformInfoVector2::vector);

		registration::class_<UniformInfoVector3>("UniformInfoVector3")(
			rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("vector", &UniformInfoVector3::vector);

		registration::class_<UniformInfoVector4>("UniformInfoVector4")(
			rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("vector", &UniformInfoVector4::vector);

		registration::class_<UniformInfoTexture>("UniformInfoTexture")(
			rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("path", &UniformInfoTexture::path);

		registration::class_<MaterialResSetting>("MaterialResSetting")(
			rttr::metadata("Serializable", true))
			.constructor<>()
			.property("isTransparent", &MaterialResSetting::isTransparent);

		registration::class_<MaterialRes>("MaterialRes")(rttr::metadata("Serializable", true))
			.constructor<>()
			.property("vertexType", &MaterialRes::vertexType)
			.property("shaderPath", &MaterialRes::shaderPath)
			.property("materialSetting", &MaterialRes::materialSetting)
			.property("uniformInfoList", &MaterialRes::uniformInfoList);


		registration::class_<AnimationClipInfo>("MaterialRes")(rttr::metadata("Serializable", true))
			.constructor<>()
			.property("clipName", &AnimationClipInfo::m_clipName)
			.property("clipPath", &AnimationClipInfo::m_clipPath)(
				rttr::metadata("AssetPath", true),
				rttr::metadata("AssetType", PathParser::EFileType::MODEL))
			.property("selectClipResName", &AnimationClipInfo::m_selectClipResName);

		
		/* Resource Types End */

		/* Framework Object Types Start */

		registration::class_<Object>("Object")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("Id", &Object::GetObjectId,
					  &Object::SetObjectId)(rttr::metadata("NO_SERIALIZE", true))
			.property("Name", &Object::GetObjectName,
					  &Object::SetObjectName)(rttr::metadata("NO_SERIALIZE", true));

		registration::class_<ScriptObject>("ScriptObject")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr);

		registration::class_<ScriptComponent>("ScriptComponent")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("ClassName", &ScriptComponent::GetClassName, &ScriptComponent::SetClassName);

		// GameObject
		registration::class_<GameObject>("GameObject")(rttr::metadata("Polymorphic", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("id", &GameObject::m_id)
			.property("parentId", &GameObject::m_parentId)
			.property("layer", &GameObject::GetLayer, &GameObject::SetLayer)
			.property("name", &GameObject::GetName, &GameObject::SetName)
			.property("componentList", &GameObject::m_componentList);

		registration::class_<Scene>("Scene")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("name", &Scene::GetName, &Scene::SetName)
			.property("availableID", &Scene::m_availableID)
			.property("gameObjects", &Scene::m_gameObjectList);

		registration::class_<Prefab>("Prefab")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("availableID", &Prefab::m_availableID)
			.property("rootEntityId", &Prefab::m_root_entity_id)
			.property("gameObjects", &Prefab::m_gameObjectList);

		// Component
		registration::class_<Component>("Component")(rttr::metadata("Serializable", true),
													 rttr::metadata("Polymorphic", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr);

		// Transform
		registration::class_<Transform>("Transform")(rttr::metadata("Serializable", true),
													 rttr::metadata("Polymorphic", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("localPosition", &Transform::GetPositionLocal,
					  &Transform::SetPositionLocal)(rttr::metadata("Serializable", true))
			.property("localRotation", &Transform::GetRotationLocal,
					  &Transform::SetRotationLocal)(rttr::metadata("QuatToEuler", true))
			.property("localScale", &Transform::GetScaleLocal, &Transform::SetScaleLocal);


		// Camera
		registration::class_<Camera>("Camera")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("clearFlags", &Camera::GetClearFlags, &Camera::SetClearFlags)
			.property("clearColor", &Camera::GetClearColor, &Camera::SetClearColor)
			.property("projectionType", &Camera::GetProjectionType, &Camera::SetProjectionType)
			.property("nearPlane", &Camera::GetNearPlane, &Camera::SetNearPlane)
			.property("farPlane", &Camera::GetFarPlane, &Camera::SetFarPlane)
			.property("fovHorizontal", &Camera::GetFovHorizontal, &Camera::SetFovHorizontal)
			.property("viewport", &Camera::GetViewport, &Camera::SetViewport)
			.property("cameraLightDesc", &Camera::GetCameraLightDesc, &Camera::SetCameraLightDesc)
			.property("depth", &Camera::GetDepth, &Camera::SetDepth);

		// Light Base Component
		registration::class_<Light>("Light")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("lightType", &Light::GetLightType, &Light::SetLightType)
			.property("color", &Light::GetColor, &Light::SetColor)
			.property("intensityLumens", &Light::GetIntensityLumens, &Light::SetIntensityLumens)
			.property("shadowsEnabled", &Light::GetShadowsEnabled, &Light::SetShadowsEnabled)
			.property("shadowsTransparentEnabled", &Light::GetShadowsTransparentEnabled, &Light::SetShadowsTransparentEnabled)
			.property("range", &Light::GetRange, &Light::SetRange)
			.property("angle", &Light::GetAngle, &Light::SetAngle);

		// Animator
		registration::class_<Animator>("Animator").constructor<>()(rttr::policy::ctor::as_raw_ptr)
			 //.property("CurrentClipName", &Animator::GetCurrentClipName)
			.property("AnimationClipNameArr", &Animator::m_animationNameArr);

		// MeshRenderer
		registration::class_<MeshRenderer>("MeshRenderer")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("materialPath", &MeshRenderer::GetMaterialPath, &MeshRenderer::SetMaterialPath)(
				rttr::metadata("AssetPath", true),
				rttr::metadata("AssetType", PathParser::EFileType::MATERIAL));

		// SkinnedMeshRenderer
		registration::class_<SkinnedMeshRenderer>("SkinnedMeshRenderer")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr);

		// MeshFilter
		registration::class_<MeshFilter>("MeshFilter")(rttr::metadata("Serializable", true),
			rttr::metadata("Polymorphic", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("MeshPath", &MeshFilter::GetMeshPath, &MeshFilter::SetMeshPath)(
				rttr::metadata("AssetPath", true),
				rttr::metadata("AssetType", PathParser::EFileType::MODEL))
			.property("SubMeshIndex", &MeshFilter::GetSubMeshIndex, &MeshFilter::SetSubMeshIndex);

		// Rect Transform
		registration::class_<RectTransform>("RectTransform")(rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("rectPos", &RectTransform::GetPos,
					  &RectTransform::SetPos)(rttr::metadata("Serializable", true))
			.property("rectSize", &RectTransform::GetSize, &RectTransform::SetSize);

		// UICanvas
		registration::class_<UICanvas>("UICanvas")(rttr::metadata("Serializable", true))
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("resolution", &UICanvas::GetResolution, &UICanvas::SetResolution);

		registration::class_<UIText>("UIText")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("FontPath", &UIText::GetFontPath, &UIText::SetFontPath)(
				rttr::metadata("AssetPath", true),
				rttr::metadata("AssetType", PathParser::EFileType::FONT))
			.property("Text", &UIText::GetText, &UIText::SetText)
			.property("Color", &UIText::GetColor, &UIText::SetColor);

		registration::class_<UIImage>("UIImage")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("ImagePath", &UIImage::GetImagePath, &UIImage::SetImagePath)(
				rttr::metadata("AssetPath", true),
				rttr::metadata("AssetType", PathParser::EFileType::TEXTURE))
			.property("Color", &UIImage::GetColor, &UIImage::SetColor);

		// .property("light", &Light::GetLight, &Light::SetLight);

		// RigidActor
		registration::class_<RigidActor>("RigidActor").constructor<>()(rttr::policy::ctor::as_raw_ptr);

		// Collider
		registration::class_<Collider>("Collider")
			.property("physicsMaterial", &Collider::GetPhysicMaterial, &Collider::SetPhysicMaterial)
			.property("offset", &Collider::GetOffset, &Collider::SetOffset);

		// RigidDynamic
		registration::class_<RigidDynamic>("RigidDynamic")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr);

		// RigidStatic
		registration::class_<RigidStatic>("RigidStatic").constructor<>()(rttr::policy::ctor::as_raw_ptr);

		// BoxCollider
		registration::class_<BoxCollider>("BoxCollider")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("Size", &BoxCollider::GetSize, &BoxCollider::SetSize);

		// SphereCollider
		registration::class_<SphereCollider>("SphereCollider")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("Radius", &SphereCollider::GetRadius, &SphereCollider::SetRadius);

		// CapsuleCollider
		registration::class_<CapsuleCollider>("CapsuleCollider")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("Radius", &CapsuleCollider::GetRadius, &CapsuleCollider::SetRadius)
			.property("Height", &CapsuleCollider::GetHeight, &CapsuleCollider::SetHeight);


		// CharacterController
		registration::class_<CharacterController>("CharacterController")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("Radius", &CharacterController::GetRadius, &CharacterController::SetRadius)
			.property("Height", &CharacterController::GetHeight, &CharacterController::SetHeight)
			.property("physicsMaterial", &CharacterController::GetPhysicMaterial, &CharacterController::SetPhysicMaterial)
			.property("StepOffset", &CharacterController::GetStepOffset, &CharacterController::SetStepOffset)
			.property("SlopeLimit", &CharacterController::GetSlopeLimit, &CharacterController::SetSlopeLimit)
			.property("MinMoveDistance", &CharacterController::GetMinMoveDistance, &CharacterController::SetMinMoveDistance)
			.property("UpDirection", &CharacterController::GetUpDirection, &CharacterController::SetUpDirection);

		registration::class_<ThirdPersonController>("ThirdPersonController")
			.constructor<>()(rttr::policy::ctor::as_raw_ptr)
			.property("MoveLinearSpeed", &ThirdPersonController::GetMoveLinearSpeed, &ThirdPersonController::SetMoveLinearSpeed)
			.property("RotateSpeed", &ThirdPersonController::GetwRotateSpeed, &ThirdPersonController::SetRotateSpeed);

		registration::class_<ScriptComponent>("ScriptComponent")
			.property("ClassName", &ScriptComponent::GetClassName, &ScriptComponent::SetClassName)
			.constructor<>()(rttr::policy::ctor::as_raw_ptr);


		/* Framework Object Types End */
	}
}  // namespace LitchiRuntime