﻿#pragma once

#include "PxPhysicsAPI.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Core/Log/debug.h"

using namespace physx;
namespace LitchiRuntime
{
    //~en SimulationEventCallback is a PxSimulationEventCallback that is used to receive events from the PhysX SDK.
    //~zh SimulationEventCallback 是一个用于从 PhysX SDK 接收事件的 PxSimulationEventCallback。
    class SimulationEventCallback : public PxSimulationEventCallback {
    public:
        void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {
            DEBUG_LOG_INFO("onConstraintBreak");
        }

        void onWake(PxActor** actors, PxU32 count) override {
            DEBUG_LOG_INFO("onWake");
        }

        void onSleep(PxActor** actors, PxU32 count) override {
            DEBUG_LOG_INFO("onSleep");
        }

        void onTrigger(PxTriggerPair* pairs, PxU32 count) override {
            DEBUG_LOG_INFO("onTrigger: {} trigger pairs", count);
            while (count--)
            {
                const PxTriggerPair& current = *pairs++;
                if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                    DEBUG_LOG_INFO("Shape is entering trigger volume");
                }
                if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
                    DEBUG_LOG_INFO("Shape is leaving trigger volume");
                }
            }
        }

        void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {
            DEBUG_LOG_INFO("onAdvance");
        }

        void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count) override {
            //		DEBUG_LOG_INFO("onContact: {} pairs", count);
            while (count--) {
                const PxContactPair& current = *pairs++;

                for (int i = 0; i < 2; ++i) {
                    PxShape* shape = current.shapes[i];
                    PxShape* another_shape = current.shapes[i ^ 1];
                    //~zh 判断Shape附加数据为1表示Trigger。
                    //~en If the shape's user data is 1, it is a trigger.
                    bool is_trigger = shape->getSimulationFilterData().word0 & 0x1;
                    if (!is_trigger) {
                        continue;
                    }
                    GameObject* gameObject = static_cast<GameObject*>(shape->userData);
                    //                DEBUG_LOG_INFO("Shape is a trigger,gameObject:{}", gameObject->name());
                    GameObject* another_game_object = static_cast<GameObject*>(another_shape->userData);
                    if (another_game_object->GetActive() == false) {
                        continue;
                    }

                    if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD)) {
                        //                    DEBUG_LOG_INFO("onContact Shape is entering volume");
                                            //通知相交的另外一个物体进入
                        another_game_object->ForeachComponent([gameObject](Component* component) {
                            component->OnTriggerEnter(gameObject);
                            });
                    }

                    if (current.events & (PxPairFlag::eNOTIFY_TOUCH_LOST)) {
                        //                    DEBUG_LOG_INFO("onContact Shape is leaving volume");
                                            //通知相交的另外一个物体离开
                        another_game_object->ForeachComponent([gameObject](Component* component) {
                            component->OnTriggerExit(gameObject);
                            });
                    }
                }
            }
        }
    };


    // 细检测前的过滤类型
    class SimulationFilterCallback :public PxSimulationFilterCallback
    {
    public:

        PxFilterFlags pairFound(PxU64 pairID, 
            PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0, 
            PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1, PxPairFlags& pairFlags) override
        {
            // 如果是Trigger 且 shape层级相同时就可以不走后续检测

            //printf("pairFound\n");
            return PxFilterFlag::eNOTIFY;
        }

        void pairLost(PxU64 pairID, 
            PxFilterObjectAttributes attributes0, PxFilterData filterData0,
            PxFilterObjectAttributes attributes1, PxFilterData filterData1,
            bool objectRemoved) override
        {
	        
        }

        bool statusChange(PxU64& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags) override
        {
            //printf("statusChange\n");
            return false;
        }

    };
}
