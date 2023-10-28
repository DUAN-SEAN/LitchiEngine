//
//#pragma once
//
//#include <array>
//
//
//#include "gtc/quaternion.hpp"
//#include "Runtime/Function/Renderer/Geometry/BoundingSphere.h"
//
//namespace LitchiRuntime
//{
//	/**
//	* Mathematic representation of a 3D frustum
//	*/
//	class Frustum
//	{
//	public:
//		/**
//		* Update frustum values
//		* @param p_viewProjection
//		*/ 
//		void CalculateFrustum(const Matrix& p_viewProjection);
//
//		/**
//		* Returns true if the given point is in frustum
//		* @param p_x
//		* @param p_y
//		* @param p_z
//		*/
//		bool PointInFrustum(float p_x, float p_y, float p_z) const;
//
//		/**
//		* Returns true if the given sphere is in frustum
//		* @param p_x
//		* @param p_y
//		* @param p_z
//		* @param p_radius
//		*/
//		bool SphereInFrustum(float p_x, float p_y, float p_z, float p_radius) const;
//
//		/**
//		* Returns true if the given cube is in frustum
//		* @param p_x
//		* @param p_y
//		* @param p_z
//		* @param p_size
//		*/
//		bool CubeInFrustum(float p_x, float p_y, float p_z, float p_size) const;
//
//		/**
//		* Returns true if the given bouding sphere is in frustum
//		* @param p_boundingSphere
//		* @param p_transform
//		*/
//		bool BoundingSphereInFrustum(const LitchiRuntime::BoundingSphere& p_boundingSphere, const Vector3 worldPos,const Quaternion worldRotation,const Vector3 worldScale) const;
//		// bool BoundingSphereInFrustum(const LitchiRuntime::BoundingSphere& p_boundingSphere, const OvMaths::FTransform& p_transform) const;
//
//		/**
//		* Returns the near plane
//		*/
//		std::array<float, 4> GetNearPlane() const;
//
//		/**
//		* Returns the far plane
//		*/
//		std::array<float, 4> GetFarPlane() const;
//
//	private:
//		float m_frustum[6][4];
//	};
//}
