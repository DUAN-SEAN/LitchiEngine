
#pragma once

//= INCLUDES =============
#include "../Math/Plane.h"
#include "Matrix.h"
#include "Vector3.h"
//========================

namespace LitchiRuntime
{
    /**
     * @brief Frustum Struct 
    */
    class Frustum
    {
    public:

        /**
         * @brief Construct
        */
        Frustum() = default;

        /**
         * @brief Construct From view and  project matrix
         * @param mView view matrix
         * @param mProjection project matrix
         * @param screenDepth screen depth
        */
        Frustum(const Matrix& mView, const Matrix& mProjection, float screenDepth);

        /**
         * @brief default DisConstruct
        */
        ~Frustum() = default;

        /**
         * @brief Check Is Visible
         * @param center checked target center location
         * @param extent checked target size
         * @param ignore_near_plane is ignore check near plane
         * @return 
        */
        bool IsVisible(const Vector3& center, const Vector3& extent, bool ignore_near_plane = false) const;

    private:

        /**
         * @brief Check cube is in frustum
         * @param center 
         * @param extent 
         * @return 
        */
        Intersection CheckCube(const Vector3& center, const Vector3& extent) const;

        /**
         * @brief Check sphere is in frustum
         * @param center 
         * @param radius 
         * @return 
        */
        Intersection CheckSphere(const Vector3& center, float radius) const;

        /**
         * @brief Frustum six plane
        */
        Plane m_planes[6];
    };
}
