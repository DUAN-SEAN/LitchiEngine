
#pragma once


namespace LitchiRuntime
{
    class Screen {
    public:
        static int GetWidth() { return m_width; }
        static int GetHeight() { return m_height; }

        /**
         * @brief get screen height/width
         * @return 
        */
        static float GetAspectRatio() { return m_aspectRatio; }

        /**
         * @brief Set Width
         * @param width 
         */
        static void SetWidth(int width) { m_width = width; CalculateAspectRatio(); }

        /**
         * @brief Set Height
         * @param height 
         */
        static void SetHeight(int height) { m_height = height; CalculateAspectRatio(); }

        /**
         * @brief Set Width and Height
         * @param width 
         * @param height 
         */
        static void SetWidthHeight(int width, int height) { m_width = width; m_height = height; CalculateAspectRatio(); }

    private:
        static void CalculateAspectRatio() { m_aspectRatio = static_cast<float>(m_width) * 1.0f / static_cast<float>(m_height); }
    private:
        static int m_width;
        static int m_height;
        static float m_aspectRatio;//宽高比
    };
}
