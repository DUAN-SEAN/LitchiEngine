#pragma once

#include <iostream>
#include <string>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Font/font.h"

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
			if (m_font != nullptr)
			{
				return m_font->path;
			}
			return "";
		}
		void SetFont(Font* font) { m_font = font; }
		Font* GetFont() { return m_font; }

		void SetText(std::string text);
		std::string GetText() { return m_text; }

		void SetColor(glm::vec4 color) { m_color = color; }
		glm::vec4 GetColor() { return m_color; }
	public:
		void Update() override;
		/// 渲染之前
		void OnPreRender() override;

		void OnPostRender() override;

	private:
		Font* m_font{};
		std::string m_text;
		bool m_dirty{};//是否变化需要重新生成Mesh
		glm::vec4 m_color;//字体颜色

	public:
		RTTR_ENABLE(Component)
	};


}
