
#ifndef UNTITLED_TYPE_REGISTER_H
#define UNTITLED_TYPE_REGISTER_H
//
//#include "Runtime/AutoGen/Type/type_register_componet.h"
//#include "Runtime/AutoGen/Type/type_register_camera.h"
//#include "Runtime/AutoGen/Type/type_register_meshRenderer.h"
//#include "Runtime/AutoGen/Type/type_register_transform.h"

#include <rttr/registration.h>

#include "Runtime/Core/Meta/Reflection/object.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Function/Framework/Component/Animation/animator.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Light/Light.h"
#include "Runtime/Function/Framework/Component/Light/DirectionalLight.h"
#include "Runtime/Function/Framework/Component/Light/SpotLight.h"
#include "Runtime/Function/Framework/Component/Light/PointLight.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

#include "Runtime/Function/Framework/Component/Physcis/rigid_actor.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_dynamic.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_static.h"
#include "Runtime/Function/Framework/Component/Physcis/box_collider.h"
#include "Runtime/Function/Framework/Component/Physcis/sphere_collider.h"
#include "Runtime/Function/Framework/Component/Renderer/skinned_mesh_renderer.h"
#include "Runtime/Function/Framework/Component/UI/ui_image.h"
#include "Runtime/Function/Framework/Component/UI/ui_text.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Function/Renderer/Light/Light.h"
#include "Runtime/Function/Scene/scene_manager.h"
#include "Runtime/Resource/config_manager.h"

using namespace rttr;
namespace LitchiRuntime
{
RTTR_REGISTRATION //注册反射
{
	// 内存结构
	registration::class_<glm::vec2>("Vec2")
		.constructor()
		.property("x", &glm::vec2::x)
		.property("y", &glm::vec2::y);

	registration::class_<glm::vec3>("Vec3")
		.constructor()
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);

	registration::class_<glm::vec4>("Vec4")
		.constructor()
		.property("x", &glm::vec4::x)
		.property("y", &glm::vec4::y)
		.property("z", &glm::vec4::z)
		.property("w", &glm::vec4::w);

	registration::class_<glm::quat>("Quaternion")
		.constructor()
		.property("x", &glm::quat::x)
		.property("y", &glm::quat::y)
		.property("z", &glm::quat::z)
		.property("w", &glm::quat::w);


	registration::class_<Object>("Object")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);


	/*registration::class_<TextureRes>("TextureRes")
		.constructor<>()
		.property("name",&TextureRes::texture_name_)
		.property("path",&TextureRes::texture_path_);*/

	/*registration::class_<MaterialRes>("MaterialRes")
		.constructor<>()
		.property("shader", &MaterialRes::shader_path_)
		.property("textureArr", &MaterialRes::textureRes_arr_);*/

	registration::class_<PhysicMaterialRes>("PhysicMaterialRes")
		.constructor<>()
		.property("dynamicFriction",  &PhysicMaterialRes::GetDynamicFriction, &PhysicMaterialRes::SetDynamicFriction)
		.property("restitution", &PhysicMaterialRes::GetRestitution,&PhysicMaterialRes::SetRestitution)
		.property("staticFriction", &PhysicMaterialRes::GetStaticFriction,&PhysicMaterialRes::SetStaticFriction);

	registration::class_<ConfigRes>("ConfigRes")
		.constructor<>()
		.property("assetFolder", &ConfigRes::asset_folder_)
		.property("rootFolder", &ConfigRes::root_folder_)
		.property("defaultScenePath", &ConfigRes::default_scene_path_);


	/* Material Resource  */
	// UniformInfoBase
	registration::class_<Resource::UniformInfoBase>("UniformInfoBase")
		(
			rttr::metadata("Serializable", true),
			rttr::metadata("Polymorphic", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("name", &Resource::UniformInfoBase::name);

	registration::class_<Resource::UniformInfoBool>("UniformInfoBool")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("value", &Resource::UniformInfoBool::value);

	registration::class_<Resource::UniformInfoInt>("UniformInfoInt")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("value", &Resource::UniformInfoInt::value);

	registration::class_<Resource::UniformInfoFloat>("UniformInfoFloat")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("value", &Resource::UniformInfoFloat::value);

	registration::class_<Resource::UniformInfoVector2>("UniformInfoVector2")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("vector", &Resource::UniformInfoVector2::vector);

	registration::class_<Resource::UniformInfoVector3>("UniformInfoVector3")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("vector", &Resource::UniformInfoVector3::vector);

	registration::class_<Resource::UniformInfoVector4>("UniformInfoVector4")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("vector", &Resource::UniformInfoVector4::vector);

	registration::class_<Resource::UniformInfoPath>("UniformInfoPath")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("path", &Resource::UniformInfoPath::path);

	registration::class_<Resource::MaterialResSetting>("MaterialResSetting")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()
		.property("blendable", &Resource::MaterialResSetting::blendable)
		.property("backfaceCulling", &Resource::MaterialResSetting::backfaceCulling)
		.property("depthTest", &Resource::MaterialResSetting::depthTest)
		.property("gpuInstances", &Resource::MaterialResSetting::gpuInstances);

	registration::class_<Resource::MaterialRes>("MaterialRes")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()
		.property("shaderPath", &Resource::MaterialRes::shaderPath)
		.property("settings", &Resource::MaterialRes::settings)
		.property("uniformInfoList", &Resource::MaterialRes::uniformInfoList);

	// 内存中的material句柄, 需要在Asset中显示, 需要被反射
	registration::class_<Resource::Material>("Material")
		(
			rttr::metadata("Serializable", true),
			rttr::metadata("Polymorphic", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("materialPath", &Resource::Material::path)
		.property("materialRes", &Resource::Material::materialRes);

	// 内存中的material句柄, 需要在Asset中显示, 需要被反射
	registration::class_<LitchiRuntime::Light>("Light")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("lightType", &Light::type)
		.property("color", &Light::color)
		.property("intensity", &Light::intensity)
		.property("constant", &Light::constant)
		.property("linear", &Light::linear)
		.property("quadratic", &Light::quadratic)
		.property("cutoff", &Light::cutoff)
		.property("outerCutoff", &Light::outerCutoff);


	// 场景管理
	// GO
	registration::class_<GameObject>("GameObject")
		(
			rttr::metadata("Polymorphic", true)
		)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("id", &GameObject::id)
		.property("parentId", &GameObject::parentId)
		.property("name", &GameObject::name)
		.property("layer", &GameObject::GetLayer,&GameObject::SetLayer)
		.property("componentList", &GameObject::componentList);

	registration::class_<Scene>("Scene")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("availableID", &Scene::availableID)
		.property("gameObjects", &Scene::game_object_vec_);

	// 组件
	// Componet
	registration::class_<Component>("Component")
		(
			rttr::metadata("Serializable", true),
			rttr::metadata("Polymorphic", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// Camera
	registration::class_<Camera>("Camera")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// Camera
	registration::class_<Animator>("Animator")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// MeshRenderer
	registration::class_<MeshRenderer>("MeshRenderer")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("materialPath",&MeshRenderer::materialPath)
			(rttr::metadata("AssetPath",true), rttr::metadata("AssetType", PathParser::EFileType::MATERIAL));

	// SkinnedMeshRenderer
	registration::class_<SkinnedMeshRenderer>("SkinnedMeshRenderer")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// MeshFilter
	registration::class_<MeshFilter>("MeshFilter")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("modelPath",&MeshFilter::modelPath)
			(rttr::metadata("AssetPath", true), rttr::metadata("AssetType", PathParser::EFileType::MODEL))
		.property("meshIndex",&MeshFilter::meshIndex);

	// Transform
	registration::class_<Transform>("Transform")
		(
			rttr::metadata("Serializable", true)
			)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("localPosition", &Transform::GetLocalPosition, &Transform::SetLocalPosition)
		(
			rttr::metadata("Serializable", true)
			)
		.property("localRotation", &Transform::GetLocalRotation, &Transform::SetLocalRotation)
		(
			rttr::metadata("QuatToEuler", true)
			)
		.property("localScale", &Transform::GetLocalScale, &Transform::SetLocalScale);

	// Light Base Component
	registration::class_<LightComponent>("LightComponent")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("light", &LightComponent::GetLight, &LightComponent::SetLight);

	// DirectionalLight
	registration::class_<DirectionalLight>("DirectionalLight")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// PointLight
	registration::class_<PointLight>("PointLight")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// SpotLight
	registration::class_<SpotLight>("SpotLight")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// RigidActor
	registration::class_<RigidActor>("RigidActor")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// Collider
	registration::class_<Collider>("Collider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("physicsMaterial",&Collider::GetPhysicMaterial,&Collider::SetPhysicMaterial);

	// RigidDynamic
	registration::class_<RigidDynamic>("RigidDynamic")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// RigidStatic
	registration::class_<RigidStatic>("RigidStatic")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// BoxCollider
	registration::class_<BoxCollider>("BoxCollider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// SphereCollider
	registration::class_<SphereCollider>("SphereCollider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<UIText>("UIText")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);


	registration::class_<UIImage>("UIImage")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	}
}

#endif