#pragma once

#include "Runtime/Core/Log/debug.h"
#include "rttr/registration"
#include "Runtime/Function/Scripting/ScriptObject.h"

using namespace rttr;
namespace LitchiRuntime
{
    class GameObject;
    class Component :public ScriptObject{
    public:
        Component();
        virtual ~Component();

        GameObject* GetGameObject() const  { return m_gameObject; }
        void SetGameObject(GameObject* game_object) { m_gameObject = game_object; }

    private:

    public:

        /**
        * Called when the scene start right before OnStart
        * It allows you to apply prioritized game logic on scene start
        */
        virtual void OnAwake();

        /**
        * Called when the components gets enabled (owner SetActive set to true) and after OnAwake() on scene starts
        */
        virtual void OnEnable();

        /**
        * Called when the scene start right after OnAwake
        * It allows you to apply prioritized game logic on scene start
        */
        virtual void OnStart();

        /**
        * Called every frame
        */
        virtual void OnUpdate();

        /**
        * Called every physics frame
        */
        virtual void OnFixedUpdate();

        /**
        * Called every frame after OnUpdate
        */
        virtual void OnLateUpdate() {}

        /**
        * Called when the component gets disabled (owner SetActive set to false) and before OnDestroy() when the component get destroyed
        */
        virtual void OnDisable();

        /**
        * Called when the components gets destroyed
        */
        virtual void OnDestroy() {}

        /// 渲染之前
        virtual void OnPreRender();

        /// 渲染之后
        virtual void OnPostRender();

        virtual void OnTriggerEnter(GameObject* game_object);

        virtual void OnTriggerExit(GameObject* game_object);

        virtual void OnTriggerStay(GameObject* game_object);

        // when 1. scene load on editor or game 、2. AddComponent will be call
        void PostResourceLoaded() override;

        /**
         * \brief 当资源加载完成后调用
         */
        virtual void PostResourceModify() override;

    private:
        GameObject* m_gameObject;

        RTTR_ENABLE(ScriptObject)
    };
}
