
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Quaternion.h"

namespace LitchiRuntime
{
    /**
     * @brief Transform Component
     * @note The Transform class provides you with a variety of ways to work with a GameObject’s position, rotation and scale via script,
     *       as well as its hierarchical relationship to parent and child GameObjects.
    */
    class Transform : public Component {
    public:

        /**
         * @brief Default Constructor
        */
        Transform();

        /**
         * @brief Default Destructor
        */
        ~Transform() override;

        //= POSITION ======================================================================
        Vector3 GetPosition()             const { return m_matrix.GetTranslation(); }
        const Vector3& GetPositionLocal() const { return m_localPosition; }
        void SetPosition(const Vector3& position);
        void SetPositionLocal(const Vector3& position);
        //=================================================================================

        //= ROTATION ======================================================================
        Quaternion GetRotation()             const { return m_matrix.GetRotation(); }
        const Quaternion& GetRotationLocal() const { return m_localRotation; }
        void SetRotation(const Quaternion& rotation);
        void SetRotationLocal(const Quaternion& rotation);
        //=================================================================================

        //= SCALE ================================================================
        Vector3 GetScale()             const { return m_matrix.GetScale(); }
        const Vector3& GetScaleLocal() const { return m_localScale; }
        void SetScale(const Vector3& scale);
        void SetScaleLocal(const Vector3& scale);
        //========================================================================

        //= TRANSLATION/ROTATION ==================
        void Translate(const Vector3& delta);
        void Rotate(const Quaternion& delta);
        //=========================================

        //= DIRECTIONS ===================
        Vector3 GetUp()       const;
        Vector3 GetDown()     const;
        Vector3 GetForward()  const;
        Vector3 GetBackward() const;
        Vector3 GetRight()    const;
        Vector3 GetLeft()     const;
        //================================

        //= DIRTY CHECKS =================================================================
        bool HasPositionChangedThisFrame() const { return m_isPositionChangedThisFrame; }
        bool HasRotationChangedThisFrame() const { return m_isRotationChangedThisFrame; }
        bool HasScaleChangedThisFrame()    const { return m_isScaleChangedThisFrame; }
        //================================================================================

        //= HIERARCHY ======================================================================================
        void SetParent(Transform* new_parent);
        Transform* GetChildByIndex(uint32_t index);
        Transform* GetChildByName(const std::string& name);
    	void AcquireChildren();
        void RemoveChild(Transform* child);
        void AddChild(Transform* child);
        bool IsDescendantOf(Transform* transform) const;
        void GetDescendants(std::vector<Transform*>* descendants);
        GameObject* GetDescendantPtrByName(const std::string& name);
        bool IsRoot()                          const { return m_parent == nullptr; }
        bool HasParent()                       const { return m_parent != nullptr; }
        bool HasChildren()                     const { return GetChildrenCount() > 0 ? true : false; }
        uint32_t GetChildrenCount()            const { return static_cast<uint32_t>(m_children.size()); }
        Transform* GetRoot() { return HasParent() ? GetParent()->GetRoot() : this; }
        Transform* GetParent()                 const { return m_parent; }
        std::vector<Transform*>& GetChildren() { return m_children; }
        void MakeDirty() { m_is_dirty = true; }
        //==================================================================================================

        //= MATRIX ======================================================================================
        const Matrix& GetMatrix()                    const { return m_matrix; }
        const Matrix& GetLocalMatrix()               const { return m_localMatrix; }
        const Matrix& GetMatrixPrevious()            const { return m_previousMatrix; }
        void SetMatrixPrevious(const Matrix& matrix) { m_previousMatrix = matrix; }

    private:

        // Internal functions don't propagate changes throughout the hierarchy.
        // They just make enough changes so that the hierarchy can be resolved later (in one go).
        void SetParent_Internal(Transform* parent);
        void AddChild_Internal(Transform* child);
        void RemoveChild_Internal(Transform* child);

        void UpdateTransform();
        Matrix GetParentTransformMatrix() const;
        bool m_is_dirty = false;

        // local
        Vector3 m_localPosition;
        Quaternion m_localRotation;
        Vector3 m_localScale;

        Matrix m_matrix;
        Matrix m_localMatrix;

        Transform* m_parent = nullptr; // the parent of this transform
        std::vector<Transform*> m_children; // the children of this transform

        Matrix m_previousMatrix;

        bool m_isPositionChangedThisFrame = false;
        bool m_isRotationChangedThisFrame = false;
        bool m_isScaleChangedThisFrame = false;

        // thread safety
        std::mutex m_childAddRemoveMutex;

        RTTR_ENABLE(Component)
    };
}
