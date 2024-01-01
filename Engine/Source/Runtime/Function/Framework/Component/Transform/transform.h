
#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Core/Math/Vector3.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Quaternion.h"

namespace LitchiRuntime
{
    class Transform : public Component {
    public:
        Transform();
        ~Transform();

         //= ICOMPONENT ===============================
        //void OnInitialize() override;
        //void OnTick() override;
        //============================================

        //= POSITION ======================================================================
        Vector3 GetPosition()             const { return m_matrix.GetTranslation(); }
        const Vector3& GetPositionLocal() const { return m_position_local; }
        void SetPosition(const Vector3& position);
        void SetPositionLocal(const Vector3& position);
        //=================================================================================

        //= ROTATION ======================================================================
        Quaternion GetRotation()             const { return m_matrix.GetRotation(); }
        const Quaternion& GetRotationLocal() const { return m_rotation_local; }
        void SetRotation(const Quaternion& rotation);
        void SetRotationLocal(const Quaternion& rotation);
        //=================================================================================

        //= SCALE ================================================================
        Vector3 GetScale()             const { return m_matrix.GetScale(); }
        const Vector3& GetScaleLocal() const { return m_scale_local; }
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
        bool HasPositionChangedThisFrame() const { return m_position_changed_this_frame; }
        bool HasRotationChangedThisFrame() const { return m_rotation_changed_this_frame; }
        bool HasScaleChangedThisFrame()    const { return m_scale_changed_this_frame; }
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
        // std::weak_ptr<GameObject> GetDescendantPtrWeakByName(const std::string& name);
        bool IsRoot()                          const { return m_parent == nullptr; }
        bool HasParent()                       const { return m_parent != nullptr; }
        bool HasChildren()                     const { return GetChildrenCount() > 0 ? true : false; }
        uint32_t GetChildrenCount()            const { return static_cast<uint32_t>(m_children.size()); }
        Transform* GetRoot() { return HasParent() ? GetParent()->GetRoot() : this; }
        Transform* GetParent()                 const { return m_parent; }
        std::vector<Transform*>& GetChildren() { return m_children; }
        void MakeDirty() { m_is_dirty = true; }
        //==================================================================================================

        const Matrix& GetMatrix()                    const { return m_matrix; }
        const Matrix& GetLocalMatrix()               const { return m_matrix_local; }
        const Matrix& GetMatrixPrevious()            const { return m_matrix_previous; }
        void SetMatrixPrevious(const Matrix& matrix) { m_matrix_previous = matrix; }

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
        Vector3 m_position_local;
        Quaternion m_rotation_local;
        Vector3 m_scale_local;

        Matrix m_matrix;
        Matrix m_matrix_local;

        Transform* m_parent; // the parent of this transform
        std::vector<Transform*> m_children; // the children of this transform

        Matrix m_matrix_previous;

        bool m_position_changed_this_frame = false;
        bool m_rotation_changed_this_frame = false;
        bool m_scale_changed_this_frame = false;

        // thread safety
        std::mutex m_child_add_remove_mutex;


    private:

        // FTransform m_transform;

        RTTR_ENABLE(Component)
    };
}
