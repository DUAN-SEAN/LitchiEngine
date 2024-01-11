﻿#pragma once

#include <iostream>
#include <string>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Font/font.h"

namespace LitchiRuntime
{
	class Font;
	class MeshRenderer;
	class UIText : public LitchiRuntime::Component {
	public:
		UIText();
		~UIText();

		std::string GetFontPath()
		{
			return m_font_path;
		}
		void SetFont(Font* font) { m_font = font; }
		Font* GetFont() { return m_font; }

		void SetText(std::string text);
		std::string GetText() { return m_text; }

		void SetColor(Vector4 color) { m_color = color; }
		Vector4 GetColor() { return m_color; }
	public:
		void Awake() override;
		void Update() override;
		/// 渲染之前
		void OnPreRender() override;

		void OnPostRender() override;
		
		std::string m_font_path;
	private:

		void CreateTextBuffer();

	private:
		Font* m_font{};// font
		std::string m_text;// text
		bool m_dirty{};// is need generate new text
		Vector4 m_color;// text color

		std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
		std::shared_ptr<RHI_IndexBuffer> m_index_buffer;

		RTTR_ENABLE(Component)

	};


}
