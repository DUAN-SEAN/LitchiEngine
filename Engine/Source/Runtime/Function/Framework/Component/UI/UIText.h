#pragma once

#include <iostream>
#include <string>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Rendering/Font/font.h"

namespace LitchiRuntime
{
	class Font;
	class MeshRenderer;

	/**
	 * @brief UIText Component
	 * @note The Text control displays a non-interactive piece of text to the user.
	*/
	class UIText : public LitchiRuntime::Component {
	public:

		/**
		 * @brief Default Constructor
		*/
		UIText();

		/**
		 * @brief Default Destructor
		*/
		~UIText() override;

		void SetFontPath(std::string fontPath) { m_fontPath = fontPath; }
		std::string GetFontPath() { return m_fontPath; }

		void SetFont(Font* font) { m_font = font; }
		Font* GetFont() { return m_font; }

		void SetText(std::string text);
		std::string GetText() { return m_text; }

		void SetColor(Color color) { m_color = color; }
		Color GetColor() { return m_color; }

		std::shared_ptr<RHI_VertexBuffer> GetVertexBuffer() { return m_vertex_buffer; }
		std::shared_ptr<RHI_IndexBuffer> GetIndexBuffer() { return m_index_buffer; }

		/**
		 * @brief Call before object resource change
		*/
		void PostResourceModify() override;

		/**
		 * @brief Call before object resource loaded
		 * when instantiate prefab, add component, resource loaded etc
		 * after call resource load completed
		*/
		void PostResourceLoaded() override;

	public:

		/**
		 * @brief Called when the scene start right before OnStart
		 * It allows you to apply prioritized game logic on scene start
		*/
		void OnAwake() override;

		/**
		 * @brief Called every frame
		*/
		void OnUpdate() override;

		/**
		 * @brief Called every frame, only editor mode
		*/
		void OnEditorUpdate() override;

		/**
		 * @brief Called when render before
		*/
		void OnPreRender() override;

		/**
		 * @brief Called when render after
		*/
		void OnPostRender() override;

	private:

		/**
		 * @brief Create Text Vertex and Index Buffer
		*/
		void CreateTextBuffer();

		/**
		 * @brief Update Text content
		*/
		void UpdateTextContent();

	private:


		std::string m_fontPath;

		/**
		 * @brief Font resource pointer
		*/
		Font* m_font{};

		/**
		 * @brief Text content
		*/
		std::string m_text;

		/**
		 * @brief Is need generate new text
		*/
		bool m_dirty{ false };

		/**
		 * @brief Text color
		*/
		Color m_color;

		/**
		 * @brief Text Vertex Buffer
		*/
		std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;

		/**
		 * @brief Text Index Buffer
		*/
		std::shared_ptr<RHI_IndexBuffer> m_index_buffer;

		RTTR_ENABLE(Component)

	};


}
