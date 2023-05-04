/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <memory>

#include <OvMaths/FVector2.h>
#include <OvTools/Eventing/Event.h>

#include "core/type.hpp"

#include "Runtime/Function/UI/Settings/Alignment.h"
#include "Runtime/Function/UI/Settings/PanelWindowSettings.h"
#include "APanel.h"

namespace LitchiRuntime
{
	/**
	* APanelTransformable is a panel that is localized in the canvas
	*/
	class APanelTransformable : public LitchiRuntime::APanel
	{
	public:
		/**
		* Create a APanelTransformable
		* @param p_defaultPosition
		* @param p_defaultSize
		* @param p_defaultHorizontalAlignment
		* @param p_defaultVerticalAlignment
		* @param p_ignoreConfigFile
		*/
		APanelTransformable
		(
			const OvMaths::FVector2& p_defaultPosition = OvMaths::FVector2(-1.f, -1.f),
			const OvMaths::FVector2& p_defaultSize = OvMaths::FVector2(-1.f, -1.f),
			Settings::EHorizontalAlignment p_defaultHorizontalAlignment = Settings::EHorizontalAlignment::LEFT,
			Settings::EVerticalAlignment p_defaultVerticalAlignment = Settings::EVerticalAlignment::TOP,
			bool p_ignoreConfigFile = false
		);

		/**
		* Defines the position of the panel
		* @param p_position
		*/
		void SetPosition(const OvMaths::FVector2& p_position);

		/**
		* Defines the size of the panel
		* @param p_size
		*/
		void SetSize(const OvMaths::FVector2& p_size);

		/**
		* Defines the alignment of the panel
		* @param p_horizontalAlignment
		* @param p_verticalAligment
		*/
		void SetAlignment(Settings::EHorizontalAlignment p_horizontalAlignment, Settings::EVerticalAlignment p_verticalAligment);

		/**
		* Returns the current position of the panel
		*/
		const OvMaths::FVector2& GetPosition() const;

		/**
		* Returns the current size of the panel
		*/
		const OvMaths::FVector2& GetSize() const;

		/**
		* Returns the current horizontal alignment of the panel
		*/
		Settings::EHorizontalAlignment GetHorizontalAlignment() const;

		/**
		* Returns the current vertical alignment of the panel
		*/
		Settings::EVerticalAlignment GetVerticalAlignment() const;

	protected:
		void Update();
		virtual void _Draw_Impl() = 0;

	private:
		glm::vec2 CalculatePositionAlignmentOffset(bool p_default = false);

		void UpdatePosition();
		void UpdateSize();
		void CopyImGuiPosition();
		void CopyImGuiSize();

	public:
		bool autoSize = true;

	protected:
		glm::vec2 m_defaultPosition;
		glm::vec2 m_defaultSize;
		EHorizontalAlignment m_defaultHorizontalAlignment;
		EVerticalAlignment m_defaultVerticalAlignment;
		bool m_ignoreConfigFile;

		glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
		glm::vec2 m_size = glm::vec2(0.0f, 0.0f);

		bool m_positionChanged = false;
		bool m_sizeChanged = false;

		EHorizontalAlignment m_horizontalAlignment = EHorizontalAlignment::LEFT;
		EVerticalAlignment m_verticalAlignment = EVerticalAlignment::TOP;

		bool m_alignmentChanged = false;
		bool m_firstFrame = true;
	};
}