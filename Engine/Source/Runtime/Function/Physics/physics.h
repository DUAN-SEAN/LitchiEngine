#pragma once

#include "PxPhysicsAPI.h"
#include "SimulationEventCallback.h"
#include "PhysicErrorCallBack.h"
#include "RaycastHit.h"
#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Core/Math/Vector2.h"
#include "Runtime/Core/Math/Vector3.h"

using namespace physx;

namespace LitchiRuntime
{

	// Physics Core API
	class Physics {
	public:
		static void Init();
		static void FixedUpdate(float fixedDeltaTime);
		
		static PxScene* CreatePxScene();
		static PxMaterial* CreateMaterial(float static_friction, float dynamic_friction, float restitution);

		/* Actor */
		static PxRigidDynamic* CreateRigidDynamic(const Vector3& position, const Quaternion& rotation, const char* name);
		static PxRigidStatic* CreateRigidStatic(const Vector3& position, const Quaternion& rotation, const char* name);
		static PxRigidDynamic* CreateRigidKinematic(const Vector3& position, const Quaternion& rotation, const char* name);
		static bool ReleaseRigidActor(PxRigidActor* rigidActor);
		static void UpdateRigidActorTransform(PxRigidActor* rigidActor, const Vector3& position, const Quaternion& rotation);
		static Vector3 GetRigidActorPosition(PxRigidActor* rigidActor);
		static Quaternion GetRigidActorRotation(PxRigidActor* rigidActor);
		static void SetMass(PxRigidBody* rigidBody, float mass);

		/* Actor Simulation */
		static void AddForce(PxRigidBody* actor, const Vector3& force, bool autoWake = true);
		static void AddLocalForce(PxRigidBody* actor, const Vector3& force, const Vector3& position, bool autoWake = true);
		// damping 0.0f-1.0f
		static void SetLinearDamping(PxRigidBody* actor, float damping);
		// damping 0.0f-1.0f  default: 0.05
		static void SetAngularDamping(PxRigidBody* actor, float damping);
		static void SetLinearVelocity(PxRigidBody* actor, const Vector3& velocity);
		static void SetAngularVelocitySet(PxRigidBody* actor, const Vector3& velocity);
		static PxVec3 GetLinearVelocity(PxRigidBody* actor);
		static PxVec3 GetAngularVelocity(PxRigidBody* actor);

		///* Cooking */
		//static PxCooking* CookingCreateDefault();
		//static void BJCookingRelease(PxCooking* cooking);

		/* Shape */
		static PxShape* CreateSphereShape(float radius, PxMaterial* material, const Vector3& position, const Quaternion& rotation);
		static PxShape* CreateBoxShape(const Vector3& size, PxMaterial* material, const Vector3& position, const Quaternion& rotation);
		static PxShape* CreateCapsuleShape(PxF32 r, PxF32 halfHeight, PxMaterial* material, const Vector3& position, const Quaternion& rotation);
		static bool ReleaseShape(PxShape* shape);
		static void SetShapeFlag(PxShape* shape, int flags);
		static bool AttachShape(PxRigidActor* rigidActor, PxShape* shape);
		static bool DetachShape(PxRigidActor* rigidActor, PxShape* shape);
		static void UpdateBoxShapeSize(PxShape* shape, const Vector3& size);
		static void UpdateSphereShapeSize(PxShape* shape, float radius);

		/* Character Controller */
		static PxControllerManager* BJControllerManagerCreate(PxScene* scene);

		static bool enable_ccd() { return enable_ccd_; }
		static void set_enable_ccd(bool enable_ccd) { enable_ccd_ = enable_ccd; }

		/* Scene Query */
		static bool RaycastSingle(Vector3& origin, Vector3& dir, float distance, RaycastHit* raycast_hit);

	private:
		static PxDefaultAllocator		px_allocator_;
		static PhysicErrorCallback	    physic_error_callback_;
		static SimulationEventCallback  simulation_event_callback_;
		//static SimulationFilterCallback  simulation_filter_callback_;

		static PxFoundation* px_foundation_;
		static PxPhysics* px_physics_;

		static PxDefaultCpuDispatcher* px_cpu_dispatcher_;
		static PxScene* px_scene_; // global scene
		static PxPvd* px_pvd_;

		static bool                     enable_ccd_;// continue collision detection
	};



}
