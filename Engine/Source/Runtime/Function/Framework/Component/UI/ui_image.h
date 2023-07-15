#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/Resources/Texture.h"

namespace LitchiRuntime
{
    class UIImage : public Component {
    public:
        UIImage();
        ~UIImage() override;

        Texture* texture2D() { return texture2D_; }
        void set_texture(Texture* texture2D) {
            texture2D_ = texture2D;
            width_ = texture2D->width;
            height_ = texture2D->height;
        }

        int width() { return width_; }
        int height() { return height_; }

    public:
        void Update() override;
        /// 渲染之前
        void OnPreRender() override;

        void OnPostRender() override;
    private:
        Texture* texture2D_;//Texture
        int width_;
        int height_;

    public:
        RTTR_ENABLE(Component)
    };
	
}

