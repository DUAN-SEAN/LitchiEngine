//
// 使用std::weak_ptr无疑是最好的选择。然而用std::weak_ptr来打破std::shared_ptr引起的循环并不那么常见，所以这个方法也不值一提。
// 严格来讲层级数据结构，比如tree，孩子结点一般都只被父节点拥有，当父节点被销毁后，所有的孩子结点也都应该被销毁。
// 这样，从父节点到子节点的链接可以用std::unique_ptr来表示，而反过来从子节点到父节点的指针可以用原始指针来实现，因为子节点的生命周期不会比父节点的更长，所以不会出现子节点去解引用一个父节点的悬浮指针的情况。
//
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

        GameObject* GetGameObject() { return m_gameObject; }
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
