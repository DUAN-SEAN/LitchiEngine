
#ifndef UNTITLED_CAMERA_H
#define UNTITLED_CAMERA_H

#include <iostream>
#include <vector>
#include <functional>
#include <glm.hpp>
#include "Runtime/Function/Framework/Component/Base/component.h"
#include "Runtime/Function/Renderer/render_camera.h"

namespace LitchiRuntime
{
    class RenderCamera;
    class RenderTexture;
    class Camera : public Component {
    public:
        Camera();
        ~Camera();

        /// 设置相机位置 朝向
        /// \param cameraFowrad 相机朝前方向
        /// \param cameraUp 相机朝上方向
        void SetView(const glm::vec3& centerPos, const glm::vec3& cameraUp);
        /// 设置相机视野
        /// \param fovDegrees   相机视野 可视角度
        /// \param aspectRatio  宽高比
        /// \param nearClip 近裁剪面
        /// \param farClip  远裁剪面
        void SetProjection(float fovDegrees, float aspectRatio, float nearClip, float farClip);

        /// 设置清屏颜色
        /// \param r
        /// \param g
        /// \param b
        /// \param a
        void set_clear_color(float r, float g, float b, float a);

        /// 设置刷帧清屏内容种类
        /// \param clear_flag
        void set_clear_flag(unsigned int clear_flag);
        
        unsigned char depth() { return depth_; }

        /// 设置 depth，触发相机排序
        /// \param depth 相机深度，多相机按从后往前排序，按depth从小往大遍历，depth越大的越上层。
        void set_depth(unsigned char depth);

        unsigned char culling_mask() { return culling_mask_; }
        void set_culling_mask(unsigned char culling_mask) { culling_mask_ = culling_mask; }
        
    private:

        unsigned char depth_;//排序深度

        unsigned char culling_mask_;//控制渲染哪些Layer的物体
    };
}


#endif //UNTITLED_CAMERA_H
