//
//
//#include "Transform.h"
//
//#include <functional>
//
//LitchiRuntime::Transform::Transform(glm::vec3 p_localPosition, glm::quat p_localRotation, glm::vec3 p_localScale) :
//	//m_notificationHandlerID(-1),
//	m_parent(nullptr)
//{
//	GenerateMatrices(p_localPosition, p_localRotation, p_localScale);
//}
//
//LitchiRuntime::Transform::~Transform()
//{
//	// Notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED);
//}
////
////void LitchiRuntime::Transform::NotificationHandler(Internal::TransformNotifier::ENotification p_notification)
////{
////	switch (p_notification)
////	{
////	case Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED:
////		UpdateWorldMatrix();
////		break;
////
////	case Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED:
////		/* 
////		* RemoveParent() is not called here because it is unsafe to remove a notification handler
////		* while the parent is iterating on his notification handlers (Segfault otherwise)
////		*/
////		GenerateMatrices(m_worldPosition, m_worldRotation, m_worldScale);
////		m_parent = nullptr;
////		UpdateWorldMatrix();
////		break;
////	}
////}
//
//void LitchiRuntime::Transform::SetParent(Transform& p_parent)
//{
//	m_parent = &p_parent;
//
//	// m_notificationHandlerID = m_parent->Notifier.AddNotificationHandler(std::bind(&Transform::NotificationHandler, this, std::placeholders::_1));
//
//	UpdateWorldMatrix();
//}
//
//bool LitchiRuntime::Transform::RemoveParent()
//{
//	if (m_parent != nullptr)
//	{
//		m_parent->Notifier.RemoveNotificationHandler(m_notificationHandlerID);
//		m_parent = nullptr;
//		UpdateWorldMatrix();
//
//		return true;
//	}
//
//	return false;
//}
//
//bool LitchiRuntime::Transform::HasParent() const
//{
//	return m_parent != nullptr;
//}
//
//void LitchiRuntime::Transform::GenerateMatrices(glm::vec3 p_position, glm::quat p_rotation, glm::vec3 p_scale)
//{
//	m_localMatrix = glm::mat4::Translation(p_position) * glm::quat::ToMatrix4(glm::quat::Normalize(p_rotation)) * glm::mat4::Scaling(p_scale);
//	m_localPosition = p_position;
//	m_localRotation = p_rotation;
//	m_localScale = p_scale;
//
//	UpdateWorldMatrix();
//}
//
//void LitchiRuntime::Transform::UpdateWorldMatrix()
//{
//	m_worldMatrix = HasParent() ? m_parent->m_worldMatrix * m_localMatrix : m_localMatrix;
//	PreDecomposeWorldMatrix();
//
//	// Notifier.NotifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED);
//}
//
//void LitchiRuntime::Transform::SetLocalPosition(glm::vec3 p_newPosition)
//{
//	GenerateMatrices(p_newPosition, m_localRotation, m_localScale);
//}
//
//void LitchiRuntime::Transform::SetLocalRotation(glm::quat p_newRotation)
//{
//	GenerateMatrices(m_localPosition, p_newRotation, m_localScale);
//}
//
//void LitchiRuntime::Transform::SetLocalScale(glm::vec3 p_newScale)
//{
//	GenerateMatrices(m_localPosition, m_localRotation, p_newScale);
//}
//
//void LitchiRuntime::Transform::SetWorldPosition(glm::vec3 p_newPosition)
//{
//	GenerateMatrices(p_newPosition, m_worldRotation, m_worldScale);
//}
//
//void LitchiRuntime::Transform::SetWorldRotation(glm::quat p_newRotation)
//{
//	GenerateMatrices(m_worldPosition, p_newRotation, m_worldScale);
//}
//
//void LitchiRuntime::Transform::SetWorldScale(glm::vec3 p_newScale)
//{
//	GenerateMatrices(m_worldPosition, m_worldRotation, p_newScale);
//}
//
//void LitchiRuntime::Transform::TranslateLocal(const glm::vec3& p_translation)
//{
//	SetLocalPosition(m_localPosition + p_translation);
//}
//
//void LitchiRuntime::Transform::RotateLocal(const glm::quat& p_rotation)
//{
//	SetLocalRotation(m_localRotation * p_rotation);
//}
//
//void LitchiRuntime::Transform::ScaleLocal(const glm::vec3& p_scale)
//{
//	SetLocalScale(glm::vec3
//	(
//		m_localScale.x * p_scale.x,
//		m_localScale.y * p_scale.y,
//		m_localScale.z * p_scale.z
//	));
//}
//
//const glm::vec3& LitchiRuntime::Transform::GetLocalPosition() const
//{
//	return m_localPosition;
//}
//
//const glm::quat& LitchiRuntime::Transform::GetLocalRotation() const
//{
//	return m_localRotation;
//}
//
//const glm::vec3& LitchiRuntime::Transform::GetLocalScale() const
//{
//	return m_localScale;
//}
//
//const glm::vec3& LitchiRuntime::Transform::GetWorldPosition() const
//{
//	return m_worldPosition;
//}
//
//const glm::quat& LitchiRuntime::Transform::GetWorldRotation() const
//{
//	return m_worldRotation;
//}
//
//const glm::vec3& LitchiRuntime::Transform::GetWorldScale() const
//{
//	return m_worldScale;
//}
//
//const glm::mat4& LitchiRuntime::Transform::GetLocalMatrix() const
//{
//	return m_localMatrix;
//}
//
//const glm::mat4& LitchiRuntime::Transform::GetWorldMatrix() const
//{
//	return m_worldMatrix;
//}
//
//glm::vec3 LitchiRuntime::Transform::GetWorldForward() const
//{
//	return m_worldRotation * glm::vec3::Forward;
//}
//
//glm::vec3 LitchiRuntime::Transform::GetWorldUp() const
//{
//	return m_worldRotation * glm::vec3::Up;
//}
//
//glm::vec3 LitchiRuntime::Transform::GetWorldRight() const
//{
//	return m_worldRotation * glm::vec3::Right;
//}
//
//glm::vec3 LitchiRuntime::Transform::GetLocalForward() const
//{
//	return m_localRotation * glm::vec3::Forward;
//}
//
//glm::vec3 LitchiRuntime::Transform::GetLocalUp() const
//{
//	return m_localRotation * glm::vec3::Up;
//}
//
//glm::vec3 LitchiRuntime::Transform::GetLocalRight() const
//{
//	return m_localRotation * glm::vec3::Right;
//}
//
//void LitchiRuntime::Transform::PreDecomposeWorldMatrix()
//{
//	m_worldPosition.x = m_worldMatrix(0, 3);
//	m_worldPosition.y = m_worldMatrix(1, 3);
//	m_worldPosition.z = m_worldMatrix(2, 3);
//
//	glm::vec3 columns[3] = 
//	{
//		{ m_worldMatrix(0, 0), m_worldMatrix(1, 0), m_worldMatrix(2, 0)},
//		{ m_worldMatrix(0, 1), m_worldMatrix(1, 1), m_worldMatrix(2, 1)},
//		{ m_worldMatrix(0, 2), m_worldMatrix(1, 2), m_worldMatrix(2, 2)},
//	};
//
//	m_worldScale.x = glm::vec3::Length(columns[0]);
//	m_worldScale.y = glm::vec3::Length(columns[1]);
//	m_worldScale.z = glm::vec3::Length(columns[2]);
//
//	if (m_worldScale.x)
//	{
//		columns[0] /= m_worldScale.x;
//	}
//	if (m_worldScale.y)
//	{
//		columns[1] /= m_worldScale.y;
//	}
//	if (m_worldScale.z)
//	{
//		columns[2] /= m_worldScale.z;
//	}
//
//	glm::mat3 rotationMatrix
//	(
//		columns[0].x, columns[1].x, columns[2].x,
//		columns[0].y, columns[1].y, columns[2].y,
//		columns[0].z, columns[1].z, columns[2].z
//	);
//
//	m_worldRotation = glm::quat(rotationMatrix);
//}
//
//void LitchiRuntime::Transform::PreDecomposeLocalMatrix()
//{
//	m_localPosition.x = m_localMatrix(0, 3);
//	m_localPosition.y = m_localMatrix(1, 3);
//	m_localPosition.z = m_localMatrix(2, 3);
//
//	glm::vec3 columns[3] =
//	{
//		{ m_localMatrix(0, 0), m_localMatrix(1, 0), m_localMatrix(2, 0)},
//		{ m_localMatrix(0, 1), m_localMatrix(1, 1), m_localMatrix(2, 1)},
//		{ m_localMatrix(0, 2), m_localMatrix(1, 2), m_localMatrix(2, 2)},
//	};
//
//	m_localScale.x = glm::vec3::Length(columns[0]);
//	m_localScale.y = glm::vec3::Length(columns[1]);
//	m_localScale.z = glm::vec3::Length(columns[2]);
//
//	if (m_localScale.x)
//	{
//		columns[0] /= m_localScale.x;
//	}
//	if (m_localScale.y)
//	{
//		columns[1] /= m_localScale.y;
//	}
//	if (m_localScale.z)
//	{
//		columns[2] /= m_localScale.z;
//	}
//
//	glm::mat3 rotationMatrix
//	(
//		columns[0].x, columns[1].x, columns[2].x,
//		columns[0].y, columns[1].y, columns[2].y,
//		columns[0].z, columns[1].z, columns[2].z
//	);
//
//	m_localRotation = glm::quat(rotationMatrix);
//}