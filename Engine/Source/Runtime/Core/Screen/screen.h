
#pragma once


namespace LitchiRuntime
{
    class Screen {
    public:
        static int GetWidth() { return m_width; }
        static int GetHeight() { return m_height; }

        /**
         * @brief get screen height/witdh
         * @return 
        */
        static float GetAspectRatio() { return m_aspectRatio; }

        static void SetWidth(int width) { m_width = width; CalculateAspectRatio(); }
        static void SetHeight(int height) { m_height = height; CalculateAspectRatio(); }
        static void SetWidthHeight(int width, int height) { m_width = width; m_height = height; CalculateAspectRatio(); }

    private:
        static void CalculateAspectRatio() { m_aspectRatio = static_cast<float>(m_width) * 1.0f / static_cast<float>(m_height); }
    private:
        static int m_width;
        static int m_height;
        static float m_aspectRatio;//宽高比
    };
}
