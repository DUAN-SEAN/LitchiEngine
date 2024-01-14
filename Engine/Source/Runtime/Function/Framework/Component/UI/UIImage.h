#pragma once

#include "Runtime/Core/Math/Vector4.h"
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"

namespace LitchiRuntime
{
    class UIImage : public Component {
    public:
        UIImage();
        ~UIImage() override;

        std::string GetImagePath()
        {
            return m_image_path;
        }
        void SetImagePath(std::string fontPath)
        {
            m_image_path = fontPath;
        }

        RHI_Texture* GetTexture() { return m_texture2D; }
        void SetTexture(RHI_Texture* texture2D) {
            m_texture2D = texture2D;
            m_image_path = texture2D->GetResourceFilePathAsset();
        }

        void SetColor(Vector4 color) { m_color = color; }
        Vector4 GetColor() { return m_color; }

        std::shared_ptr<RHI_VertexBuffer> GetVertexBuffer() { return m_vertex_buffer; }
        std::shared_ptr<RHI_IndexBuffer> GetIndexBuffer() { return m_index_buffer; };
    public:
        void OnCreate() override;
        void Awake() override;
        void Update() override;

        void PostResourceLoaded() override;
        void PostResourceModify() override;
    private:

        void CreateBuffer();

        std::string m_image_path;

        RHI_Texture* m_texture2D;//Texture
        Vector4 m_color;// text color

        std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer;
        std::shared_ptr<RHI_IndexBuffer> m_index_buffer;

    public:
        RTTR_ENABLE(Component)
    };
	
}

