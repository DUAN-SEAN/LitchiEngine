
#pragma once

//= INCLUDES =======
#include <memory>
#include "Vector2.h"
//==================

namespace LitchiRuntime
{
    /*namespace Math
    {*/
        class Rectangle
        {
        public:
            Rectangle()
            {
                left   = 0.0f;
                top    = 0.0f;
                right  = 0.0f;
                bottom = 0.0f;
            }

            Rectangle(const float left, const float top, const float right, const float bottom)
            {
                this->left   = left;
                this->top    = top;
                this->right  = right;
                this->bottom = bottom;
            }

            Rectangle(const Rectangle& rectangle)
            {
                left   = rectangle.left;
                top    = rectangle.top;
                right  = rectangle.right;
                bottom = rectangle.bottom;
            }

            ~Rectangle() = default;

            bool operator==(const Rectangle& rhs) const
            {
                return
                    left   == rhs.left  &&
                    top    == rhs.top   &&
                    right  == rhs.right &&
                    bottom == rhs.bottom;
            }

            bool operator!=(const Rectangle& rhs) const
            {
                return
                    left   != rhs.left  ||
                    top    != rhs.top   ||
                    right  != rhs.right ||
                    bottom != rhs.bottom;
            }

            bool IsDefined() const
            {
                return  left   != 0.0f ||
                        top    != 0.0f ||
                        right  != 0.0f ||
                        bottom != 0.0f;
            }

            float Width()  const { return right - left; }
            float Height() const { return bottom - top; }

            // Merge a point.
            void Merge(const Vector2& point)
            {
                left   = Math::Helper::Min(left,   point.x);
                top    = Math::Helper::Min(top,    point.y);
                right  = Math::Helper::Max(right,  point.x);
                bottom = Math::Helper::Max(bottom, point.y);
            }

            float left   = 0.0f;
            float top    = 0.0f;
            float right  = 0.0f;
            float bottom = 0.0f;

            static const Rectangle Zero;
        };
   // }
}
