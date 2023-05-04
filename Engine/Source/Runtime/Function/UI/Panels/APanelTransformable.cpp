
#include "APanelTransformable.h"

#include "Runtime/Function/UI/Internal/Converter.h"

LitchiRuntime::APanelTransformable::APanelTransformable
(
	const glm::vec2& p_defaultPosition,
	const glm::vec2& p_defaultSize,
	EHorizontalAlignment p_defaultHorizontalAlignment,
	EVerticalAlignment p_defaultVerticalAlignment,
	bool p_ignoreConfigFile
) :
	m_defaultPosition(p_defaultPosition),
	m_defaultSize(p_defaultSize),
	m_defaultHorizontalAlignment(p_defaultHorizontalAlignment),
	m_defaultVerticalAlignment(p_defaultVerticalAlignment),
	m_ignoreConfigFile(p_ignoreConfigFile)
{
}

void LitchiRuntime::APanelTransformable::SetPosition(const glm::vec2& p_position)
{
	m_position = p_position;
	m_positionChanged = true;
}

void LitchiRuntime::APanelTransformable::SetSize(const glm::vec2& p_size)
{
	m_size = p_size;
	m_sizeChanged = true;
}

void LitchiRuntime::APanelTransformable::SetAlignment(EHorizontalAlignment p_horizontalAlignment,EVerticalAlignment p_verticalAligment)
{
	m_horizontalAlignment = p_horizontalAlignment;
	m_verticalAlignment = p_verticalAligment;
	m_alignmentChanged = true;
}

const glm::vec2 & LitchiRuntime::APanelTransformable::GetPosition() const
{
	return m_position;
}

const glm::vec2 & LitchiRuntime::APanelTransformable::GetSize() const
{
	return m_size;
}

LitchiRuntime::EHorizontalAlignment LitchiRuntime::APanelTransformable::GetHorizontalAlignment() const
{
	return m_horizontalAlignment;
}

LitchiRuntime::EVerticalAlignment LitchiRuntime::APanelTransformable::GetVerticalAlignment() const
{
	return m_verticalAlignment;
}

void LitchiRuntime::APanelTransformable::UpdatePosition()
{
	if (m_defaultPosition.x != -1.f && m_defaultPosition.y != 1.f)
	{
		glm::vec2 offsettedDefaultPos = m_defaultPosition + CalculatePositionAlignmentOffset(true);
		ImGui::SetWindowPos(Converter::ToImVec2(offsettedDefaultPos), m_ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
	}

	if (m_positionChanged || m_alignmentChanged)
	{
		glm::vec2 offset = CalculatePositionAlignmentOffset(false);
		glm::vec2 offsettedPos(m_position.x + offset.x, m_position.y + offset.y);
		ImGui::SetWindowPos(Converter::ToImVec2(offsettedPos), ImGuiCond_Always);
		m_positionChanged = false;
		m_alignmentChanged = false;
	}
}

void LitchiRuntime::APanelTransformable::UpdateSize()
{
	/*
	if (m_defaultSize.x != -1.f && m_defaultSize.y != 1.f)
		ImGui::SetWindowSize(Converter::ToImVec2(m_defaultSize), m_ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
	*/
	if (m_sizeChanged)
	{
		ImGui::SetWindowSize(Converter::ToImVec2(m_size), ImGuiCond_Always);
		m_sizeChanged = false;
	}
}

void LitchiRuntime::APanelTransformable::CopyImGuiPosition()
{
	m_position = Converter::ToFVector2(ImGui::GetWindowPos());
}

void LitchiRuntime::APanelTransformable::CopyImGuiSize()
{
	m_size = Converter::ToFVector2(ImGui::GetWindowSize());
}

void LitchiRuntime::APanelTransformable::Update()
{
	if (!m_firstFrame)
	{
		if (!autoSize)
			UpdateSize();
		CopyImGuiSize();

		UpdatePosition();
		CopyImGuiPosition();
	}

	m_firstFrame = false;
}

glm::vec2 LitchiRuntime::APanelTransformable::CalculatePositionAlignmentOffset(bool p_default)
{
	glm::vec2 result(0.0f, 0.0f);

	switch (p_default ? m_defaultHorizontalAlignment : m_horizontalAlignment)
	{
	case EHorizontalAlignment::CENTER:
		result.x -= m_size.x / 2.0f;
		break;
	case EHorizontalAlignment::RIGHT:
		result.x -= m_size.x;
		break;
	}

	switch (p_default ? m_defaultVerticalAlignment : m_verticalAlignment)
	{
	case EVerticalAlignment::MIDDLE:
		result.y -= m_size.y / 2.0f;
		break;
	case EVerticalAlignment::BOTTOM:
		result.y -= m_size.y;
		break;
	}

	return result;
}
