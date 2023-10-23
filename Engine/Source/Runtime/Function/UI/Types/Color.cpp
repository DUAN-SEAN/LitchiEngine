//
//#include "Color.h"
//
//namespace LitchiRuntime
//{
//
//    // Most of the color/temperature values are derived from: https://physicallybased.info/ 
//    // Might get inaccurate over 40000 K (which is really the limit that you should be using)
//    static void temperature_to_color(const float temperature_kelvin, float& r, float& g, float& b)
//    {
//        // Constants for color temperature to RGB conversion
//        const float A_R = 329.698727446f;
//        const float B_R = -0.1332047592f;
//        const float A_G = 288.1221695283f;
//        const float B_G = -0.0755148492f;
//
//        // Ensure temperature is above absolute zero
//        if (temperature_kelvin < 0) {
//            // Handle error
//        }
//
//        float temp = temperature_kelvin / 100.0f;
//
//        r = 0.0f;
//        g = 0.0f;
//        b = 0.0f;
//
//        if (temp <= 66)
//        {
//            r = 255;
//            g = temp;
//            g = 99.4708025861f * Math::Helper::Log(g) - 161.1195681661f;
//
//            if (temp <= 19)
//            {
//                b = 0;
//            }
//            else
//            {
//                b = temp - 10.0f;
//                b = 138.5177312231f * Math::Helper::Log(b) - 305.0447927307f;
//            }
//        }
//        else
//        {
//            r = temp - 60.0f;
//            r = A_R * Math::Helper::Pow(r, B_R);
//            g = temp - 60.0f;
//            g = A_G * Math::Helper::Pow(g, B_G);
//            b = 255;
//        }
//
//        // Clamp RGB values to [0, 1]
//        r = Math::Helper::Clamp(r / 255.0f, 0.0f, 1.0f);
//        g = Math::Helper::Clamp(g / 255.0f, 0.0f, 1.0f);
//        b = Math::Helper::Clamp(b / 255.0f, 0.0f, 1.0f);
//    }
//
//
//	const LitchiRuntime::Color LitchiRuntime::Color::Red = { 1.f, 0.f, 0.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Green = { 0.f, 1.f, 0.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Blue = { 0.f, 0.f, 1.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::White = { 1.f, 1.f, 1.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Black = { 0.f, 0.f, 0.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Grey = { 0.5f, 0.5f, 0.5f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Yellow = { 1.f, 1.f, 0.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Cyan = { 0.f, 1.f, 1.f };
//	const LitchiRuntime::Color LitchiRuntime::Color::Magenta = { 1.f, 0.f, 1.f };
//
//	LitchiRuntime::Color::Color(float p_r, float p_g, float p_b, float p_a) : r(p_r), g(p_g), b(p_b), a(p_a)
//	{
//	}
//
//	bool LitchiRuntime::Color::operator==(const Color& p_other)
//	{
//		return this->r == p_other.r && this->g == p_other.g && this->b == p_other.b && this->a == p_other.a;
//	}
//
//	bool LitchiRuntime::Color::operator!=(const Color& p_other)
//	{
//		return !operator==(p_other);
//	}
//}
//
