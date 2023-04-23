
#ifndef UNTITLED_TYPE_REGISTER_H
#define UNTITLED_TYPE_REGISTER_H
//
//#include "Runtime/AutoGen/Type/type_register_componet.h"
//#include "Runtime/AutoGen/Type/type_register_camera.h"
//#include "Runtime/AutoGen/Type/type_register_meshRenderer.h"
//#include "Runtime/AutoGen/Type/type_register_transform.h"

#include <rttr/registration.h>

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Framework/Component/Camera/camera.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"

#include "Runtime/Function/Framework/Component/Physcis/rigid_actor.h"
#include "Runtime/Function/Framework/Component/Physcis/collider.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_dynamic.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_static.h"
#include "Runtime/Function/Framework/Component/Physcis/box_collider.h"
#include "Runtime/Function/Framework/Component/Physcis/sphere_collider.h"
#include "Runtime/Function/Renderer/material.h"

using namespace rttr;
namespace LitchiRuntime
{
RTTR_REGISTRATION //注册反射
{

	// 组件

	// Componet
	registration::class_<Component>("Component")
		(
		rttr::metadata("Serializable", true)
		)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// Camera
	registration::class_<Camera>("Camera")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// MeshRenderer
	registration::class_<MeshRenderer>("MeshRenderer")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// MeshFilter
	registration::class_<MeshFilter>("MeshFilter")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// Transform
	registration::class_<Transform>("Transform")
		(
		rttr::metadata("Serializable", true)
		)
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("position", &Transform::position, &Transform::set_position)
		(
			rttr::metadata("Serializable", true)
		)
		.property("rotation", &Transform::rotation, &Transform::set_rotation)
		.property("scale", &Transform::scale, &Transform::set_scale);

	// RigidActor
	registration::class_<RigidActor>("RigidActor")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	// Collider
	registration::class_<Collider>("Collider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

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

	// 内存结构
	registration::class_<TextureRes>("TextureRes")
		.constructor<>()
		.property("name",&TextureRes::texture_name_)
		.property("path",&TextureRes::texture_path_);

	registration::class_<MaterialRes>("MaterialRes")
		.constructor<>()
		.property("shader", &MaterialRes::shader_path_)
		.property("textureArr", &MaterialRes::textureRes_arr_);

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
		.property("y", &glm::vec4::z)
		.property("w", &glm::vec4::w);


	registration::class_<VertexRes>("VertexRes")
		.constructor()
		.property("pos", &VertexRes::pos_)
		.property("color", &VertexRes::color_)
		.property("uv", &VertexRes::uv_);


	}
}

#endif