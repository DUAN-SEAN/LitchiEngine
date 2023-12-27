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
        virtual void OnEnable();
        virtual void Awake();
        virtual void Update();
        virtual void FixedUpdate();

        /// 渲染之前
        virtual void OnPreRender();
        /// 渲染之后
        virtual void OnPostRender();

        virtual void OnDisable();

        virtual void OnTriggerEnter(GameObject* game_object);

        virtual void OnTriggerExit(GameObject* game_object);

        virtual void OnTriggerStay(GameObject* game_object);

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
