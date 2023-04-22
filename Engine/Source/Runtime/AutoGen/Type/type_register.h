
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
#include "Runtime/Function/Framework/Component/Physcis/box_collider.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_dynamic.h"
#include "Runtime/Function/Framework/Component/Physcis/rigid_static.h"
#include "Runtime/Function/Framework/Component/Physcis/sphere_collider.h"

using namespace rttr;
RTTR_REGISTRATION //×¢²á·´Éä
{
	registration::class_<Component>("Component")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<Camera>("Camera")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<MeshRenderer>("MeshRenderer")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<MeshFilter>("MeshFilter")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<Transform>("Transform")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("position", &Transform::position, &Transform::set_position)
		.property("rotation", &Transform::rotation, &Transform::set_rotation)
		.property("scale", &Transform::scale, &Transform::set_scale);

	registration::class_<RigidActor>("RigidActor")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<Collider>("Collider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<BoxCollider>("BoxCollider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<RigidDynamic>("RigidDynamic")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<RigidStatic>("RigidStatic")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);

	registration::class_<SphereCollider>("SphereCollider")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

#endif