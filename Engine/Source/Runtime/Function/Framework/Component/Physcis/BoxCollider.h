
#pragma once

#include "rttr/registration"
#include "collider.h"
#include "Runtime/Core/Math/Vector3.h"

using namespace rttr;
namespace LitchiRuntime
{
    /**
     * @brief BoxCollider Component
    */
    class BoxCollider : public Collider {
    public:

        /**
         * @brief Default Constructor
        */
        BoxCollider();

        /**
         * @brief Default Destructor
        */
        ~BoxCollider() override;

        /**
         * @brief Set Box Collider Size
         * @param size
        */
        void SetSize(Vector3 size)
        {
            m_size = size;
        }

        /**
         * @brief Get Box Collider Size
         * @return 
        */
        Vector3 GetSize()
        {
            return m_size;
        }

        /**
         * @brief Call before object resource change
        */
        void PostResourceModify() override;

    protected:

        /**
         * @brief Create PhysX Shape
        */
        void CreateShape() override;

    private:

        /**
         * @brief Update Collider shape box size
         * @param size Current collider box size
        */
        void UpdateShapeSize(const Vector3& size);

        /**
         * @brief Box Collider Size
        */
        Vector3 m_size;

        RTTR_ENABLE(Collider);
    };
}
