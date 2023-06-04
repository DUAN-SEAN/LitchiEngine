
#include <functional>

#include <gtx/Transform2.hpp>
#include <gtx/quaternion.hpp>

#include "FTransform.h"

const glm::vec3 One(1.0f, 1.0f, 1.0f);
const glm::vec3 Zero(0.0f, 0.0f, 0.0f);
const glm::vec3 Forward(0.0f, 0.0f, -1.0f);
const glm::vec3 Right(1.0f, 0.0f, 0.0f);
const glm::vec3 Up(0.0f, 1.0f, 0.0f);

LitchiRuntime::FTransform::FTransform(glm::vec3 p_localPosition, glm::quat p_localRotation, glm::vec3 p_localScale) :
	m_notificationHandlerID(-1),
	m_parent(nullptr)
{
	GenerateMatrices(p_localPosition, p_localRotation, p_localScale);
}

LitchiRuntime::FTransform::~FTransform()
{
	Notifier.NotifyChildren(TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void LitchiRuntime::FTransform::NotificationHandler(TransformNotifier::ENotification p_notification)
{
	switch (p_notification)
	{
	case TransformNotifier::ENotification::TRANSFORM_CHANGED:
		UpdateWorldMatrix();
		break;

	case TransformNotifier::ENotification::TRANSFORM_DESTROYED:
		/* 
		* RemoveParent() is not called here because it is unsafe to remove a notification handler
		* while the parent is iterating on his notification handlers (Segfault otherwise)
		*/
		GenerateMatrices(m_worldPosition, m_worldRotation, m_worldScale);
		m_parent = nullptr;
		UpdateWorldMatrix();
		break;
	}
}

void LitchiRuntime::FTransform::SetParent(FTransform& p_parent)
{
	m_parent = &p_parent;

	m_notificationHandlerID = m_parent->Notifier.AddNotificationHandler(std::bind(&FTransform::NotificationHandler, this, std::placeholders::_1));

	UpdateWorldMatrix();
}

bool LitchiRuntime::FTransform::RemoveParent()
{
	if (m_parent != nullptr)
	{
		m_parent->Notifier.RemoveNotificationHandler(m_notificationHandlerID);
		m_parent = nullptr;
		UpdateWorldMatrix();

		return true;
	}

	return false;
}

bool LitchiRuntime::FTransform::HasParent() const
{
	return m_parent != nullptr;
}

void LitchiRuntime::FTransform::GenerateMatrices(glm::vec3 p_position, glm::quat p_rotation, glm::vec3 p_scale)
{
	m_localMatrix = glm::translate(p_position) * glm::toMat4(glm::normalize(p_rotation)) * glm::scale(p_scale);
	m_localPosition = p_position;
	m_localRotation = p_rotation;
	m_localScale = p_scale;

	UpdateWorldMatrix();
}

void LitchiRuntime::FTransform::UpdateWorldMatrix()
{
	m_worldMatrix = HasParent() ? m_parent->m_worldMatrix * m_localMatrix : m_localMatrix;
	PreDecomposeWorldMatrix();

	Notifier.NotifyChildren(TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void LitchiRuntime::FTransform::SetLocalPosition(glm::vec3 p_newPosition)
{
	GenerateMatrices(p_newPosition, m_localRotation, m_localScale);
}

void LitchiRuntime::FTransform::SetLocalRotation(glm::quat p_newRotation)
{
	GenerateMatrices(m_localPosition, p_newRotation, m_localScale);
}

void LitchiRuntime::FTransform::SetLocalScale(glm::vec3 p_newScale)
{
	GenerateMatrices(m_localPosition, m_localRotation, p_newScale);
}

void LitchiRuntime::FTransform::SetWorldPosition(glm::vec3 p_newPosition)
{
	GenerateMatrices(p_newPosition, m_worldRotation, m_worldScale);
}

void LitchiRuntime::FTransform::SetWorldRotation(glm::quat p_newRotation)
{
	GenerateMatrices(m_worldPosition, p_newRotation, m_worldScale);
}

void LitchiRuntime::FTransform::SetWorldScale(glm::vec3 p_newScale)
{
	GenerateMatrices(m_worldPosition, m_worldRotation, p_newScale);
}

void LitchiRuntime::FTransform::TranslateLocal(const glm::vec3& p_translation)
{
	SetLocalPosition(m_localPosition + p_translation);
}

void LitchiRuntime::FTransform::RotateLocal(const glm::quat& p_rotation)
{
	SetLocalRotation(m_localRotation * p_rotation);
}

void LitchiRuntime::FTransform::ScaleLocal(const glm::vec3& p_scale)
{
	SetLocalScale(glm::vec3
	(
		m_localScale.x * p_scale.x,
		m_localScale.y * p_scale.y,
		m_localScale.z * p_scale.z
	));
}

const glm::vec3& LitchiRuntime::FTransform::GetLocalPosition() const
{
	return m_localPosition;
}

const glm::quat& LitchiRuntime::FTransform::GetLocalRotation() const
{
	return m_localRotation;
}

const glm::vec3& LitchiRuntime::FTransform::GetLocalScale() const
{
	return m_localScale;
}

const glm::vec3& LitchiRuntime::FTransform::GetWorldPosition() const
{
	return m_worldPosition;
}

const glm::quat& LitchiRuntime::FTransform::GetWorldRotation() const
{
	return m_worldRotation;
}

const glm::vec3& LitchiRuntime::FTransform::GetWorldScale() const
{
	return m_worldScale;
}

const glm::mat4& LitchiRuntime::FTransform::GetLocalMatrix() const
{
	return m_localMatrix;
}

const glm::mat4& LitchiRuntime::FTransform::GetWorldMatrix() const
{
	return m_worldMatrix;
}

glm::vec3 LitchiRuntime::FTransform::GetWorldForward() const
{
	return m_worldRotation * Forward;
}

glm::vec3 LitchiRuntime::FTransform::GetWorldUp() const
{
	return m_worldRotation * Up;
}

glm::vec3 LitchiRuntime::FTransform::GetWorldRight() const
{
	return m_worldRotation * Right;
}

glm::vec3 LitchiRuntime::FTransform::GetLocalForward() const
{
	return m_localRotation * Forward;
}

glm::vec3 LitchiRuntime::FTransform::GetLocalUp() const
{
	return m_localRotation * Up;
}

glm::vec3 LitchiRuntime::FTransform::GetLocalRight() const
{
	return m_localRotation * Right;
}

void LitchiRuntime::FTransform::PreDecomposeWorldMatrix()
{
	m_worldPosition.x = m_worldMatrix[3][0];
	m_worldPosition.y = m_worldMatrix[3][1];
	m_worldPosition.z = m_worldMatrix[3][2];

	glm::vec3 rows[3] = 
	{
		{ m_worldMatrix[0][0], m_worldMatrix[0][1], m_worldMatrix[0][2]},
		{ m_worldMatrix[1][0], m_worldMatrix[1][1], m_worldMatrix[1][2]},
		{ m_worldMatrix[2][0], m_worldMatrix[2][1], m_worldMatrix[2][2]},
	};

	m_worldScale.x = glm::length(rows[0]);
	m_worldScale.y = glm::length(rows[1]);
	m_worldScale.z = glm::length(rows[2]);

	if (m_worldScale.x)
	{
		rows[0] /= m_worldScale.x;
	}
	if (m_worldScale.y)
	{
		rows[1] /= m_worldScale.y;
	}
	if (m_worldScale.z)
	{
		rows[2] /= m_worldScale.z;
	}

	glm::mat3 rotationMatrix
	(
		rows[0].x, rows[0].y, rows[0].z,
		rows[1].x, rows[1].y, rows[1].z,
		rows[2].x, rows[2].y, rows[2].z
	);

	m_worldRotation = glm::quat(rotationMatrix);
}

void LitchiRuntime::FTransform::PreDecomposeLocalMatrix()
{
	m_worldPosition.x = m_worldMatrix[3][0];
	m_worldPosition.y = m_worldMatrix[3][1];
	m_worldPosition.z = m_worldMatrix[3][2];

	glm::vec3 rows[3] =
	{
		{ m_worldMatrix[0][0], m_worldMatrix[0][1], m_worldMatrix[0][2]},
		{ m_worldMatrix[1][0], m_worldMatrix[1][1], m_worldMatrix[1][2]},
		{ m_worldMatrix[2][0], m_worldMatrix[2][1], m_worldMatrix[2][2]},
	};

	m_localScale.x = glm::length(rows[0]);
	m_localScale.y = glm::length(rows[1]);
	m_localScale.z = glm::length(rows[2]);

	if (m_localScale.x)
	{
		rows[0] /= m_localScale.x;
	}
	if (m_localScale.y)
	{
		rows[1] /= m_localScale.y;
	}
	if (m_localScale.z)
	{
		rows[2] /= m_localScale.z;
	}

	glm::mat3 rotationMatrix
	(
		rows[0].x, rows[0].y, rows[0].z,
		rows[1].x, rows[1].y, rows[1].z,
		rows[2].x, rows[2].y, rows[2].z
	);

	m_localRotation = glm::quat(rotationMatrix);
}