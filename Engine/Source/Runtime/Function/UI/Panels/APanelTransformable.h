
#pragma once

#include "Runtime/Function/UI/Settings/Alignment.h"
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
			const Vector2& p_defaultPosition = Vector2(-1.f, -1.f),
			const Vector2& p_defaultSize = Vector2(-1.f, -1.f),
			EHorizontalAlignment p_defaultHorizontalAlignment = EHorizontalAlignment::LEFT,
			EVerticalAlignment p_defaultVerticalAlignment = EVerticalAlignment::TOP,
			bool p_ignoreConfigFile = false
		);

		/**
		* Defines the position of the panel
		* @param p_position
		*/
		void SetPosition(const Vector2& p_position);

		/**
		* Defines the size of the panel
		* @param p_size
		*/
		void SetSize(const Vector2& p_size);

		/**
		* Defines the alignment of the panel
		* @param p_horizontalAlignment
		* @param p_verticalAligment
		*/
		void SetAlignment(EHorizontalAlignment p_horizontalAlignment,EVerticalAlignment p_verticalAligment);

		/**
		* Returns the current position of the panel
		*/
		const Vector2& GetPosition() const;

		/**
		* Returns the current size of the panel
		*/
		const Vector2& GetSize() const;

		/**
		* Returns the current horizontal alignment of the panel
		*/
		EHorizontalAlignment GetHorizontalAlignment() const;

		/**
		* Returns the current vertical alignment of the panel
		*/
		EVerticalAlignment GetVerticalAlignment() const;

	protected:
		virtual void OnDraw();
		virtual void _Draw_Impl() = 0;

	private:
		Vector2 CalculatePositionAlignmentOffset(bool p_default = false);

		void UpdatePosition();
		void UpdateSize();
		void CopyImGuiPosition();
		void CopyImGuiSize();

	public:
		bool autoSize = true;

	protected:
		Vector2 m_defaultPosition;
		Vector2 m_defaultSize;
		EHorizontalAlignment m_defaultHorizontalAlignment;
		EVerticalAlignment m_defaultVerticalAlignment;
		bool m_ignoreConfigFile;

		Vector2 m_position = Vector2(0.0f, 0.0f);
		Vector2 m_size = Vector2(0.0f, 0.0f);

		bool m_positionChanged = false;
		bool m_sizeChanged = false;

		EHorizontalAlignment m_horizontalAlignment = EHorizontalAlignment::LEFT;
		EVerticalAlignment m_verticalAlignment = EVerticalAlignment::TOP;

		bool m_alignmentChanged = false;
		bool m_firstFrame = true;
	};
}