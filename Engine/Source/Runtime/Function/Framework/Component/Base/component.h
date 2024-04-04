#pragma once

#include "rttr/registration"
#include "Runtime/Function/Scripting/ScriptObject.h"

using namespace rttr;
namespace LitchiRuntime
{
    class GameObject;

    /**
     * @brief Component Base Class, All Component must Derived this
    */
    class Component :public ScriptObject{
    public:

        /**
         * @brief Default Constructor
        */
        Component();

        /**
         * @brief Default Destructor
        */
        ~Component() override;

        /**
         * @brief Set this Component belong GameObject
         * @param game_object 
        */
        void SetGameObject(GameObject* game_object) { m_gameObject = game_object; }

        /**
         * @brief Get the game object to which this Component belongs 
         * @return Belong GameObject pointer
        */
        GameObject* GetGameObject() const  { return m_gameObject; }

        /**
         * @brief Call before object resource change
        */
        void PostResourceModify() override;

        /**
         * @brief Call before object resource loaded
         * when instantiate prefab, add component, resource loaded etc
         * after call resource load completed
        */
        void PostResourceLoaded() override;
    
    public:

        /**
         * @brief Called when the scene start right before OnStart
         * It allows you to apply prioritized game logic on scene start
        */
        virtual void OnAwake();

        /**
         * @brief Called when the components gets enabled (owner SetActive set to true) and after OnAwake() on scene starts
        */
        virtual void OnEnable();

        /**
         * @brief Called when the scene start right after OnAwake
         * It allows you to apply prioritized game logic on scene start
        */
        virtual void OnStart();

        /**
         * @brief Called every frame, only editor mode
        */
        virtual void OnEditorUpdate(){}

        /**
         * @brief Called every frame
        */
        virtual void OnUpdate();

        /**
         * @brief Called every physics frame
        */
        virtual void OnFixedUpdate();

        /**
         * @brief Called every frame after OnUpdate
        */
        virtual void OnLateUpdate() {}

        /**
         * @brief Called when the component gets disabled (owner SetActive set to false) and before OnDestroy() when the component get destroyed
        */
        virtual void OnDisable();

        /**
         * @brief Called when the components gets destroyed
        */
        virtual void OnDestroy() {}

        /**
         * @brief Called when render before
        */
        virtual void OnPreRender();

        /**
         * @brief Called when render after
        */
        virtual void OnPostRender();

        /**
         * @brief Called when trigger enter other GameObject
         * @param game_object 
        */
        virtual void OnTriggerEnter(GameObject* game_object);

        /**
         * @brief Called when trigger exit other GameObject
         * @param game_object 
        */
        virtual void OnTriggerExit(GameObject* game_object);

        /**
         * @brief Called when trigger stay other GameObject
         * @param game_object
        */
        virtual void OnTriggerStay(GameObject* game_object);

    private:

        /**
         * @brief Belong GameObject
        */
        GameObject* m_gameObject;

        RTTR_ENABLE(ScriptObject)
    };
}
