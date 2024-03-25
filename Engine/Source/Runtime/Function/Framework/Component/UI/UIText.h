#pragma once

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
		void SetFontPath(std::string fontPath)
		{
			m_font_path = fontPath;
		}
		void SetFont(Font* font) { m_font = font; }
		Font* GetFont() { return m_font; }

		void SetText(std::string text);
		std::string GetText() { return m_text; }

		void SetColor(Color color) { m_color = color; }
		Color GetColor() { return m_color; }

		std::shared_ptr<RHI_VertexBuffer> GetVertexBuffer(){ return m_vertex_buffer; }
		std::shared_ptr<RHI_IndexBuffer> GetIndexBuffer() { return m_index_buffer; }

	public:
		void OnAwake() override;
		void OnUpdate() override;
		void OnEditorUpdate() override;
		/// 渲染之前
		void OnPreRender() override;
		void OnPostRender() override;

		void PostResourceLoaded() override;
		void PostResourceModify() override;
		
	private:

		void CreateTextBuffer();
		void UpdateTextContent();

	private:
		std::string m_font_path;
		Font* m_font{};// font
		std::string m_text;// text
		bool m_dirty{};// is need generate new text
		Color m_color;// text color

		std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
		std::shared_ptr<RHI_IndexBuffer> m_index_buffer;

		RTTR_ENABLE(Component)

	};


}
