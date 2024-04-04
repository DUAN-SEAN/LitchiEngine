#pragma once

#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/RHI/RHI_Texture.h"

namespace LitchiRuntime
{
    /**
     * @brief UIImage Component
     * @note The Image control displays a non-interactive image to the user. 
    */
    class UIImage : public Component {
    public:
        /**
         * @brief Default Constructor
        */
        UIImage();

        /**
         * @brief Default Destructor
        */
        ~UIImage() override;

        /**
         * @brief Set Image resource path
         * @param fontPath 
        */
        void SetImagePath(std::string fontPath)
        {
            m_imagePath = fontPath;
        }

        /**
         * @brief Get Image resource path
         * @return 
        */
        std::string GetImagePath()
        {
            return m_imagePath;
        }

        /**
         * @brief Set Texture2D resource pointer
         * @param texture2D 
        */
        void SetTexture(RHI_Texture* texture2D) {
            m_texture2D = texture2D;
            m_imagePath = texture2D->GetResourceFilePathAsset();
        }

        /**
         * @brief Get Texture2D resource pointer
         * @return 
        */
        RHI_Texture* GetTexture() { return m_texture2D; }

        /**
         * @brief Set Image color
         * @param color 
        */
        void SetColor(Color color) { m_color = color; }

        /**
         * @brief Get Image color
         * @return 
        */
        Color GetColor() { return m_color; }

        /**
         * @brief Get Image Vertex Buffer
         * @return 
        */
        std::shared_ptr<RHI_VertexBuffer> GetVertexBuffer() { return m_vertexBuffer; }

        /**
         * @brief Get Image Index Buffer
         * @return 
        */
        std::shared_ptr<RHI_IndexBuffer> GetIndexBuffer() { return m_indexBuffer; }

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

    private:

        /**
         * @brief Create Vertex and Index Buffer
        */
        void CreateBuffer();

        /**
         * @brief Image resource path
        */
        std::string m_imagePath;

        /**
         * @brief texture2D resource pointer
        */
        RHI_Texture* m_texture2D;

        /**
         * @brief Image color
        */
        Color m_color;

        /**
         * @brief Image vertex buffer
        */
        std::shared_ptr<RHI_VertexBuffer> m_vertexBuffer;

        /**
         * @brief Image index Buffer
        */
        std::shared_ptr<RHI_IndexBuffer> m_indexBuffer;

        RTTR_ENABLE(Component)
    };
	
}

