
#ifndef UNTITLED_CAMERA_H
#define UNTITLED_CAMERA_H

#include <iostream>
#include <vector>
#include <functional>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"

namespace LitchiRuntime
{
    class RenderCamera;
    class RenderTexture;
    class Camera : public Component {
    public:
        Camera();
        ~Camera();


      /*  /// 设置清屏颜色
        /// \param r
        /// \param g
        /// \param b
        /// \param a
        void set_clear_color(float r, float g, float b, float a);

        /// 设置刷帧清屏内容种类
        /// \param clear_flag
        void set_clear_flag(unsigned int clear_flag);
        */
        unsigned char GetDepth() { return m_depth; }

        /// 设置 depth，触发相机排序
        /// \param depth 相机深度，多相机按从后往前排序，按depth从小往大遍历，depth越大的越上层。
        void SetDepth(unsigned char depth);

        unsigned char GetCullingMask() { return m_culling_mask; }
        void SetCullingMask(unsigned char culling_mask) { m_culling_mask = culling_mask; }
    public:
        void Update() override;

    private:

        unsigned char m_depth;//排序深度

        unsigned char m_culling_mask;//控制渲染哪些Layer的物体
    };
}


#endif //UNTITLED_CAMERA_H
