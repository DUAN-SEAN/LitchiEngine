
#pragma once

//= INCLUDES ===================
#include <memory>
#include "../Definitions.h"
//==============================

namespace LitchiRuntime
{
    class GameObject;

   /* namespace Math
    {*/
        class LC_CLASS RayHit
        {
        public:
            RayHit(GameObject* entity, const Vector3& position, float distance, bool is_inside)
                :m_entity{entity}
                , m_position{position}
            {
                m_distance = distance;
                m_inside   = is_inside;
            };

            GameObject* m_entity;
            Vector3 m_position;
            float m_distance;
            bool m_inside;
        };
    // }
}
