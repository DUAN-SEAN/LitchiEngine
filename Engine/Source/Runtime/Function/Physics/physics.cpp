
#include "physics.h"
#include "Runtime/Core/Log/debug.h"
#include "Runtime/Core/Math/Vector3.h"

namespace LitchiRuntime
{
    namespace PhysXMathConvertHelper
    {
        PxVec3 ConvertToPxVec3(const Vector3& vector) { return PxVec3(vector.x, vector.y, vector.z); }
        PxVec2 ConvertToPxVec2(const Vector2& vector) { return PxVec2(vector.x, vector.y); }
        PxQuat ConvertToPxQuat(const Quaternion& quaternion) { return PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w); }
        Vector3 ConvertToVector3(const PxVec3& vector) { return Vector3(vector.x, vector.y, vector.z); }
        Vector2 ConvertToVector2(const PxVec2& vector) { return Vector2(vector.x, vector.y); }
        Quaternion ConvertToQuaternion(const PxQuat& quaternion) { return Quaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w); }
        PxTransform ConvertToPxTransform(const Vector3& position, const Quaternion& rotation) { return PxTransform(ConvertToPxVec3(position), ConvertToPxQuat(rotation)); }
    }

    PxDefaultAllocator		Physics::px_allocator_;
    PhysicErrorCallback	    Physics::physic_error_callback_;
    SimulationEventCallback Physics::simulation_event_callback_;
    //SimulationFilterCallback Physics::simulation_filter_callback_;
    PxFoundation* Physics::px_foundation_;
    PxPhysics* Physics::px_physics_;
    PxDefaultCpuDispatcher* Physics::px_cpu_dispatcher_;
    PxScene* Physics::px_scene_;
    PxPvd* Physics::px_pvd_;
    bool                    Physics::enable_ccd_ = true;

    //~zh 设置在碰撞发生时，Physx需要做的事情
    //~en Set the actions when collision occurs,Physx needs to do.
    static	PxFilterFlags SimulationFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {

        //~zh eNOTIFY_TOUCH_FOUND:当碰撞发生时处理回调。 eNOTIFY_TOUCH_LOST:当碰撞结束时处理回调。
        //~en eNOTIFY_TOUCH_FOUND:When collision occurs,process callback. eNOTIFY_TOUCH_LOST:When collision ends,process callback.
        pairFlags = PxPairFlag::eSOLVE_CONTACT
            |PxPairFlag::eDETECT_DISCRETE_CONTACT
            |PxPairFlag::eNOTIFY_TOUCH_FOUND
            |PxPairFlag::eNOTIFY_TOUCH_LOST
            | PxPairFlag::eNOTIFY_TOUCH_PERSISTS // 会持续性的报告接触
    		| PxPairFlag::eNOTIFY_CONTACT_POINTS // 提供对接触点的报告;
    		| PxPairFlag::eMODIFY_CONTACTS; // 提供对接触点的报告;

        //~zh Trigger的意思就是不处理物理碰撞，只是触发一个回调函数。
        //~en Trigger means that the physical collision is not processed,only a callback function is triggered.
        bool isTrigger = filterData0.word0 == 1 || filterData1.word0 == 1;
        if (isTrigger) {
            pairFlags = pairFlags ^ PxPairFlag::eSOLVE_CONTACT;
        }

        if (Physics::enable_ccd()) {
            //~zh 场景启用CCD后，还需要指定碰撞时使用CCD，并且处理回调。
            //~en When the scene is enabled CCD, you need to specify the collision to use CCD and handle the callback.
            pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eNOTIFY_TOUCH_CCD;
        }
        return PxFilterFlag::eNOTIFY;
    }


    void Physics::Init() {
        //~en Creates an instance of the foundation class,The foundation class is needed to initialize higher level SDKs.only one instance per process.
        //~zh 创建Foundation实例。
        px_foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator_, physic_error_callback_);

        //~en Connect to pvd(PhysX Visual Debugger).
        //~zh 连接PVD
        px_pvd_ = PxCreatePvd(*px_foundation_);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        px_pvd_->connect(*transport, PxPvdInstrumentationFlag::eALL);

        //~en Creates an instance of the physics SDK.
        //~zh 创建Physx SDK实例
        px_physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation_, PxTolerancesScale(), true, px_pvd_);

        px_scene_ = CreatePxScene();
    }

    void Physics::FixedUpdate(float fixedDeltaTime) {
        if (px_scene_ == nullptr) {
            DEBUG_LOG_ERROR("px_scene_==nullptr,please call Physics::CreatePxScene() first");
            return;
        }
        px_scene_->simulate(fixedDeltaTime);
        px_scene_->fetchResults(true);
    }

    PxScene* Physics::CreatePxScene() {
        if (px_physics_ == nullptr) {
            DEBUG_LOG_ERROR("px_physics_==nullptr,please call Physics::Init() first");
            return nullptr;
        }
        //~zh 创建Physx Scene
        PxSceneDesc sceneDesc(px_physics_->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        px_cpu_dispatcher_ = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = px_cpu_dispatcher_;
        //~en set physx event callback,such as trigger,collision,etc.
        //~zh 设置事件回调，用于接收物理事件，如Awake/Trigger等
        sceneDesc.simulationEventCallback = &simulation_event_callback_;
        //~zh 设置在碰撞发生时，Physx需要做的事情
        //~en Set the actions when collision occurs,Physx needs to do.
        // sceneDesc.filterShader = SimulationFilterShader;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        //sceneDesc.filterCallback = &simulation_filter_callback_;
        if (enable_ccd_) {
            //~zh 启用CCD
            //~en Enable CCD
            sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
        }
        PxScene* px_scene = px_physics_->createScene(sceneDesc);
        //~zh 设置PVD
        PxPvdSceneClient* pvd_client = px_scene->getScenePvdClient();
        if (pvd_client)
        {
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }

        return px_scene;
    }

    PxRigidDynamic* Physics::CreateRigidDynamic(const Vector3& position, const Quaternion& rotation, const char* name) {
        PxRigidDynamic* body = px_physics_->createRigidDynamic(PhysXMathConvertHelper::ConvertToPxTransform(position, rotation));
        body->setName(name);
        px_scene_->addActor(*body);
        return body;
    }

    PxRigidStatic* Physics::CreateRigidStatic(const Vector3& position, const Quaternion& rotation, const char* name) {
        PxRigidStatic* body = px_physics_->createRigidStatic(PhysXMathConvertHelper::ConvertToPxTransform(position, rotation));
        body->setName(name);
        px_scene_->addActor(*body);
        return body;
    }

    PxRigidDynamic* Physics::CreateRigidKinematic(const Vector3& position, const Quaternion& rotation, const char* name)
    {
        PxRigidDynamic* rigidActor = px_physics_->createRigidDynamic(PhysXMathConvertHelper::ConvertToPxTransform(position, rotation));
        rigidActor->setName(name);
        // 设置RigidActor为Kinematic
        rigidActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        return rigidActor;
    }

    bool Physics::ReleaseRigidActor(PxRigidActor* rigidActor)
    {
        if (rigidActor != nullptr) {

            // 内部会唤醒所有与此Actor有联系的Actor
            // NpRigidActorTemplate::releaseActorT内部实现
            // 会清理actor上的shape，会使得shape上的引用计数减一，当引用计数减一时会被清理
            // 若actor存在于场景中,则会从场景中移除,内部会调用Scene->removeActor
            rigidActor->release();
            return true;
        }
        return false;
    }

    void Physics::UpdateRigidActorTransform(PxRigidActor* rigidActor, const Vector3& position, const Quaternion& rotation)
    {
        if (rigidActor != nullptr) {
            // 设置全局Pose，一般针对于
            rigidActor->setGlobalPose(PhysXMathConvertHelper::ConvertToPxTransform(position, rotation));
        }
    }

    Vector3 Physics::GetRigidActorPosition(PxRigidActor* rigidActor)
    {
        if (rigidActor != nullptr) {
            PxTransform tran = rigidActor->getGlobalPose();
            return PhysXMathConvertHelper::ConvertToVector3(tran.p);
        }
        return Vector3::Zero;
    }

    Quaternion Physics::GetRigidActorRotation(PxRigidActor* rigidActor)
    {
        if (rigidActor != nullptr) {
            PxTransform tran = rigidActor->getGlobalPose();
            return PhysXMathConvertHelper::ConvertToQuaternion(tran.q);
        }
        return Quaternion::Identity;
    }

    void Physics::SetMass(PxRigidBody* rigidBody, float mass)
    {
        if (rigidBody)
        {
            rigidBody->setMass(mass);
        }
    }

    void Physics::AddForce(PxRigidBody* actor, const Vector3& force, bool autoWake)
    {
        if (actor != nullptr) 
        {
            actor->addForce(PhysXMathConvertHelper::ConvertToPxVec3(force), PxForceMode::eFORCE, autoWake);
        }
    }

    void Physics::AddLocalForce(PxRigidBody* actor, const Vector3& force, const Vector3& position, bool autoWake)
    {
        if (actor != nullptr) 
        {
        	PxRigidBodyExt::addLocalForceAtLocalPos(*actor, PhysXMathConvertHelper::ConvertToPxVec3(force), PhysXMathConvertHelper::ConvertToPxVec3(position), PxForceMode::eFORCE, autoWake);
        }
    }

    void Physics::SetLinearDamping(PxRigidBody* actor, float damping)
    {
        if (actor != nullptr)
        {
            actor->setLinearDamping(damping);
        }
    }

    void Physics::SetAngularDamping(PxRigidBody* actor, float damping)
    {
        if (actor != nullptr)
        {
            actor->setAngularDamping(damping);
        }
    }

    void Physics::SetLinearVelocity(PxRigidBody* actor, const Vector3& velocity)
    {
        if (actor != nullptr)
        {
            //actor->setLinearVelocity(PhysXMathConvertHelper::ConvertToPxVec3(velocity));
        }
    }

    void Physics::SetAngularVelocitySet(PxRigidBody* actor, const Vector3& velocity)
    {
        if (actor != nullptr)
        {
            //actor->setAngularVelocity(PhysXMathConvertHelper::ConvertToPxVec3(velocity));
        }
    }

    PxVec3 Physics::GetLinearVelocity(PxRigidBody* actor)
    {
        return actor->getLinearVelocity();
    }

    PxVec3 Physics::GetAngularVelocity(PxRigidBody* actor)
    {
        return actor->getAngularVelocity();
    }

  /*  PxCooking* Physics::CookingCreateDefault()
    {
        PxCooking* mCooking = PxCreateCooking(PX_PHYSICS_VERSION, PxGetFoundation(), PxCookingParams(px_physics_->getTolerancesScale()));
        return mCooking;
    }

    void Physics::BJCookingRelease(PxCooking* cooking)
    {
        if (cooking != nullptr)
        {
            cooking->release();
        }
    }*/

    PxMaterial* Physics::CreateMaterial(float static_friction, float dynamic_friction, float restitution) {
        PxMaterial* material = px_physics_->createMaterial(static_friction, dynamic_friction, restitution);
        return material;
    }

    PxShape* Physics::CreateSphereShape(float radius, PxMaterial* material, const Vector3& position, const Quaternion& rotation) {
        PxShape* shape = px_physics_->createShape(PxSphereGeometry(radius), *material,false);
        shape->setLocalPose(PhysXMathConvertHelper::ConvertToPxTransform(position,rotation));
        PxFilterData data(1, 0, 0, 0);
        shape->setQueryFilterData(data);
        return shape;
    }

    PxShape* Physics::CreateBoxShape(const Vector3& size, PxMaterial* material, const Vector3& position, const Quaternion& rotation) {
        PxShape* shape = px_physics_->createShape(PxBoxGeometry(size.x / 2, size.y / 2, size.z / 2), *material,false);
        shape->setLocalPose(PhysXMathConvertHelper::ConvertToPxTransform(position, rotation));
        PxFilterData data(1, 0, 0, 0);
        shape->setQueryFilterData(data);
        return shape;
    }

    PxShape* Physics::CreateCapsuleShape(PxF32 r, PxF32 halfHeight, PxMaterial* material, const Vector3& position, const Quaternion& rotation)
    {
    	// 默认胶囊体是 X-AXIS,半径位胶囊体球半径,半长指胶囊体中间圆柱的半长
        PxCapsuleGeometry capsule(r, halfHeight);
        PxShape* shape = px_physics_->createShape(capsule, *material, false);
        // 左手坐标系,绕y轴正方向旋转90度
        PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
        // 下面的方法将改变shape的局部姿势,不会唤醒物体,也不会重新计算其惯性,仅仅就是改变了数值
        shape->setLocalPose(PhysXMathConvertHelper::ConvertToPxTransform(position, rotation) * relativePose);
        PxFilterData data(1, 0, 0, 0);
        shape->setQueryFilterData(data);
        return shape;
    }

    bool Physics::ReleaseShape(PxShape* shape)
    {
        if (shape == nullptr) {
            return false;
        }
        // 这里会使得shape的引用计数减一，当计数为0时它会删掉它自己
        // 所以可以在将shape添加到一个actor上时，就release它，这样这个shape就只属于actor，当actor移除shape或者release时，shape就会被释放
        shape->release();
        return true;
    }

    void Physics::SetShapeFlag(PxShape* shape, int flags)
    {
        PxShapeFlags shapeFlags(flags);
        shape->setFlags(shapeFlags);
    }

    bool Physics::AttachShape(PxRigidActor* rigidActor, PxShape* shape)
    {
        if (rigidActor == nullptr || shape == nullptr) {
            return false;
        }
        // 这个操作将导致shape的引用计数加一
        // 同时会更新pruner
        rigidActor->attachShape(*shape);
        return true;
    }

    bool Physics::DetachShape(PxRigidActor* rigidActor, PxShape* shape)
    {
        if (rigidActor == nullptr || shape == nullptr) {
            return false;
        }
        // 这个操作将导致shape引用计数减一
        // 同时会更新pruner
        rigidActor->detachShape(*shape);
        return true;
    }

    void Physics::UpdateBoxShapeSize(PxShape* shape,const Vector3& size)
    {
        shape->setGeometry(PxBoxGeometry(size.x / 2, size.y / 2, size.z / 2));
    }

    void Physics::UpdateSphereShapeSize(PxShape* shape,float radius)
    {
        shape->setGeometry(PxSphereGeometry(radius));
    }

    bool Physics::RaycastSingle(Vector3& origin, Vector3& dir, float distance, RaycastHit* raycast_hit) {
        PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
        PxRaycastHit px_raycast_hit;
        if (PxSceneQueryExt::raycastSingle(*px_scene_, PxVec3(origin.x, origin.y, origin.z), PxVec3(dir.x, dir.y, dir.z), distance, hitFlags, px_raycast_hit)) {
            raycast_hit->set_position(px_raycast_hit.position.x, px_raycast_hit.position.y, px_raycast_hit.position.z);

            GameObject* game_object = static_cast<GameObject*>(px_raycast_hit.shape->userData);
            raycast_hit->set_game_object(game_object);
            return true;
        }
        return false;
    }
}

