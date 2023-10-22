//
//#pragma once
//
//#include "rttr/registration"
//
//#include "Runtime/Core/Math/FTransform.h"
//
//namespace LitchiRuntime
//{
//	/**
//	* Data structure that can represent any type of light
//	*/
//	struct Light
//	{
//		/**
//		* Light types
//		*/
//		enum class Type { POINT, DIRECTIONAL, SPOT, AMBIENT_BOX, AMBIENT_SPHERE };
//
//		Light(){}
//		~Light(){}
//
//		/**
//		* Create the light with the given transform and type
//		* @param p_transform
//		* @param p_type
//		*/
//		Light(Type p_type);
//
//		/**
//		* Generate the light matrix, ready to send to the GPU
//		*/
//		glm::mat4 GenerateMatrix(FTransform& p_transform) const;
//
//		/**
//		* Calculate the light effect range from the quadratic falloff equation
//		*/
//		float GetEffectRange(FTransform& p_transform) const;
//		
//		glm::vec3	color		= { 1.f, 1.f, 1.f };
//		float				intensity	= 1.0f; // 光照强度
//		float				constant	= 0.0f; // point spot
//		float				linear		= 0.0f; // point spot 
//		float				quadratic	= 1.0f; // point spot
//		float				cutoff		= 12.f; // spot
//		float				outerCutoff = 15.f; // spot
//		float				type		= 0.0f; // 光照类型
//
//		RTTR_ENABLE()
//	};
//}
