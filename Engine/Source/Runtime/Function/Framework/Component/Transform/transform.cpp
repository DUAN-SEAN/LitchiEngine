
#include "transform.h"
#include "Runtime/Function/Framework/GameObject/GameObject.h"
#include "Runtime/Function/Scene/SceneManager.h"

using namespace std;

namespace LitchiRuntime
{
	Transform::Transform() :Component()
		// , m_transform{}
	{
        m_position_local = Vector3::Zero;
        m_rotation_local = Quaternion(0, 0, 0, 1);
        m_scale_local = Vector3::One;
        m_matrix = Matrix::Identity;
        m_matrix_local = Matrix::Identity;
        m_matrix_previous = Matrix::Identity;
        m_parent = nullptr;
        m_is_dirty = true;
	}

	Transform::~Transform() {
	}

    void Transform::UpdateTransform()
    {
        // Compute local transform
        m_matrix_local = Matrix(m_position_local, m_rotation_local, m_scale_local);

        // Compute world transform
        if (m_parent)
        {
            m_matrix = m_matrix_local * m_parent->GetMatrix();
        }
        else
        {
            m_matrix = m_matrix_local;
        }

        // Update children
        for (Transform* child : m_children)
        {
            child->UpdateTransform();
        }
    }

    void Transform::SetPosition(const Vector3& position)
    {
        if (GetPosition() == position)
            return;

        SetPositionLocal(!HasParent() ? position : position * GetParent()->GetMatrix().Inverted());
    }

    void Transform::SetPositionLocal(const Vector3& position)
    {
        if (m_position_local == position)
            return;

        m_position_local = position;
        UpdateTransform();

        m_position_changed_this_frame = true;
    }

    void Transform::SetRotation(const Quaternion& rotation)
    {
        if (GetRotation() == rotation)
            return;

        SetRotationLocal(!HasParent() ? rotation : rotation * GetParent()->GetRotation().Inverse());
    }

    void Transform::SetRotationLocal(const Quaternion& rotation)
    {
        if (m_rotation_local == rotation)
            return;

        m_rotation_local = rotation;
        UpdateTransform();

        m_rotation_changed_this_frame = true;
    }

    void Transform::SetScale(const Vector3& scale)
    {
        if (GetScale() == scale)
            return;

        SetScaleLocal(!HasParent() ? scale : scale / GetParent()->GetScale());
    }

    void Transform::SetScaleLocal(const Vector3& scale)
    {
        if (m_scale_local == scale)
            return;

        m_scale_local = scale;

        // A scale of 0 will cause a division by zero when decomposing the world transform matrix.
        m_scale_local.x = (m_scale_local.x == 0.0f) ? Math::Helper::EPSILON : m_scale_local.x;
        m_scale_local.y = (m_scale_local.y == 0.0f) ? Math::Helper::EPSILON : m_scale_local.y;
        m_scale_local.z = (m_scale_local.z == 0.0f) ? Math::Helper::EPSILON : m_scale_local.z;

        UpdateTransform();

        m_scale_changed_this_frame = true;
    }

    void Transform::Translate(const Vector3& delta)
    {
        if (!HasParent())
        {
            SetPositionLocal(m_position_local + delta);
        }
        else
        {
            SetPositionLocal(m_position_local + GetParent()->GetMatrix().Inverted() * delta);
        }
    }

    void Transform::Rotate(const Quaternion& delta)
    {
        if (!HasParent())
        {
            SetRotationLocal((m_rotation_local * delta).Normalized());
        }
        else
        {
            SetRotationLocal(m_rotation_local * GetRotation().Inverse() * delta * GetRotation());
        }
    }

    Vector3 Transform::GetUp() const
    {
        return GetRotation() * Vector3::Up;
    }

    Vector3 Transform::GetDown() const
    {
        return GetRotation() * Vector3::Down;
    }

    Vector3 Transform::GetForward() const
    {
        return GetRotation() * Vector3::Forward;
    }

    Vector3 Transform::GetBackward() const
    {
        return GetRotation() * Vector3::Backward;
    }

    Vector3 Transform::GetRight() const
    {
        return GetRotation() * Vector3::Right;
    }

    Vector3 Transform::GetLeft() const
    {
        return GetRotation() * Vector3::Left;
    }
    
    Transform* Transform::GetChildByIndex(const uint32_t index)
    {
        if (!HasChildren())
        {
            DEBUG_LOG_WARN("%s has no children.");
            return nullptr;
        }

        // prevent an out of vector bounds error
        if (index >= GetChildrenCount())
        {
            DEBUG_LOG_WARN("There is no child with an index of \"%d\".", index);
            return nullptr;
        }

        return m_children[index];
    }

    Transform* Transform::GetChildByName(const std::string& name)
    {
        for (Transform* child : m_children)
        {
            if (child->GetGameObject()->GetObjectName() == name)
                return child;
        }

        return nullptr;
    }

    void Transform::SetParent(Transform* new_parent)
    {
        // Early exit if the parent is this transform (which is invalid).
        if (new_parent)
        {
            if (GetObjectId() == new_parent->GetObjectId())
                return;
        }

        // Early exit if the parent is already set.
        if (m_parent && new_parent)
        {
            if (m_parent->GetObjectId() == new_parent->GetObjectId())
                return;
        }

        // If the new parent is a descendant of this transform (e.g. dragging and dropping an entity onto one of it's children).
        if (new_parent && new_parent->IsDescendantOf(this))
        {
            // Assign the parent of this transform to the children.
            for (Transform* child : m_children)
            {
                child->SetParent_Internal(m_parent);
            }

            // Remove any children
            m_children.clear();
        }

        // Remove this child from it's previous parent
        if (m_parent)
        {
            m_parent->RemoveChild_Internal(this);
        }

        // Add this child to the new parent
        if (new_parent)
        {
            new_parent->AddChild_Internal(this);
            new_parent->MakeDirty();
        }

        // Assign the new parent.
        m_parent = new_parent ? new_parent : nullptr;
        m_is_dirty = true;

        // update transform
        if(m_parent)
        {
            UpdateTransform();
        }
    }

    void Transform::AddChild(Transform* child)
    {
        SP_ASSERT(child != nullptr);

        // Ensure that the child is not this transform.
        if (child->GetObjectId() == GetObjectId())
            return;

        child->SetParent(GetGameObject()->GetComponent<Transform>());
    }

    void Transform::RemoveChild(Transform* child)
    {
        SP_ASSERT(child != nullptr);

        // Ensure the transform is not itself
        if (child->GetObjectId() == GetObjectId())
            return;

        // Remove the child.
        m_children.erase(remove_if(m_children.begin(), m_children.end(), [child](Transform* vec_transform) { return vec_transform->GetObjectId() == child->GetObjectId(); }), m_children.end());

        // Remove the child's parent.
        child->SetParent(nullptr);
    }

    void Transform::SetParent_Internal(Transform* new_parent)
    {
        // Ensure that parent is not this transform.
        if (new_parent)
        {
            if (GetObjectId() == new_parent->GetObjectId())
                return;
        }

        // Mark as dirty if the parent is about to really change.
        if ((m_parent && !new_parent) || (!m_parent && new_parent))
        {
            m_is_dirty = true;
        }

        // Assign the new parent.
        m_parent = new_parent;
    }

    void Transform::AddChild_Internal(Transform* child)
    {
        SP_ASSERT(child != nullptr);

        // Ensure that the child is not this transform.
        if (child->GetObjectId() == GetObjectId())
            return;

        lock_guard lock(m_child_add_remove_mutex);

        // If this is not already a child, add it.
        if (!(find(m_children.begin(), m_children.end(), child) != m_children.end()))
        {
            m_children.emplace_back(child);
        }
    }

    void Transform::RemoveChild_Internal(Transform* child)
    {
        SP_ASSERT(child != nullptr);

        // Ensure the transform is not itself
        if (child->GetObjectId() == GetObjectId())
            return;

        lock_guard lock(m_child_add_remove_mutex);

        // Remove the child
        m_children.erase(remove_if(m_children.begin(), m_children.end(), [child](Transform* vec_transform) { return vec_transform->GetObjectId() == child->GetObjectId(); }), m_children.end());
    }

    // Searches the entire hierarchy, finds any children and saves them in m_children.
    // This is a recursive function, the children will also find their own children and so on...
    void Transform::AcquireChildren()
    {
        m_children.clear();
        m_children.shrink_to_fit();

        auto entities = GetGameObject()->GetScene()->GetAllGameObjectList();
        for (const auto& entity : entities)
        {
            if (!entity)
                continue;

            // get the possible child
            auto possible_child = entity->GetComponent<Transform>();

            // if it doesn't have a parent, forget about it.
            if (!possible_child->HasParent())
                continue;

            // if it's parent matches this transform
            if (possible_child->GetParent()->GetObjectId() == GetObjectId())
            {
                // welcome home son
                m_children.emplace_back(possible_child);

                // make the child do the same thing all over, essentially resolving the entire hierarchy.
                possible_child->AcquireChildren();
            }
        }
    }

    bool Transform::IsDescendantOf(Transform* transform) const
    {
        SP_ASSERT(transform != nullptr);

        if (!m_parent)
            return false;

        if (m_parent->GetObjectId() == transform->GetObjectId())
            return true;

        for (Transform* child : transform->GetChildren())
        {
            if (IsDescendantOf(child))
                return true;
        }

        return false;
    }

    void Transform::GetDescendants(vector<Transform*>* descendants)
    {
        for (Transform* child : m_children)
        {
            descendants->emplace_back(child);

            if (child->HasChildren())
            {
                child->GetDescendants(descendants);
            }
        }
    }

    GameObject* Transform::GetDescendantPtrByName(const std::string& name)
    {
        vector<Transform*> descendants;
        GetDescendants(&descendants);

        for (Transform* transform : descendants)
        {
            if (transform->GetGameObject()->GetObjectName() == name)
                return transform->GetGameObject();
        }

        return nullptr;
    }

   /* weak_ptr<GameObject> Transform::GetDescendantPtrWeakByName(const std::string& name)
    {
        vector<Transform*> descendants;
        GetDescendants(&descendants);

        for (Transform* transform : descendants)
        {
            if (transform->GetGameObject()->GetObjectName() == name)
                return transform->GetEntityPtrWeak();
        }

        static weak_ptr<Entity> empty;
        return empty;
    }*/

    Matrix Transform::GetParentTransformMatrix() const
    {
        return HasParent() ? GetParent()->GetMatrix() : Matrix::Identity;
    }
}
