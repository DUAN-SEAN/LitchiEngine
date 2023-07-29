#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Resources/Texture.h"

namespace LitchiRuntime
{
    class UIImage : public Component {
    public:
        UIImage();
        ~UIImage() override;

        Texture* GetTexture() { return m_texture2D; }
        void SetTexture(Texture* texture2D) {
            m_texture2D = texture2D;
            m_width = texture2D->width;
            m_height = texture2D->height;
        }

        int GetWidth() { return m_width; }
        int GetHeight() { return m_height; }

    public:
        void Update() override;
        /// 渲染之前
        void OnPreRender() override;

        void OnPostRender() override;
    private:
        Texture* m_texture2D;//Texture
        int m_width;
        int m_height;

    public:
        RTTR_ENABLE(Component)
    };
	
}

