
#include "Color.h"

const LitchiRuntime::Color LitchiRuntime::Color::Red		= { 1.f, 0.f, 0.f };
const LitchiRuntime::Color LitchiRuntime::Color::Green		= { 0.f, 1.f, 0.f };
const LitchiRuntime::Color LitchiRuntime::Color::Blue		= { 0.f, 0.f, 1.f };
const LitchiRuntime::Color LitchiRuntime::Color::White		= { 1.f, 1.f, 1.f };
const LitchiRuntime::Color LitchiRuntime::Color::Black		= { 0.f, 0.f, 0.f };
const LitchiRuntime::Color LitchiRuntime::Color::Grey		= { 0.5f, 0.5f, 0.5f };
const LitchiRuntime::Color LitchiRuntime::Color::Yellow		= { 1.f, 1.f, 0.f };
const LitchiRuntime::Color LitchiRuntime::Color::Cyan		= { 0.f, 1.f, 1.f };
const LitchiRuntime::Color LitchiRuntime::Color::Magenta	= { 1.f, 0.f, 1.f };

LitchiRuntime::Color::Color(float p_r, float p_g, float p_b, float p_a) : r(p_r), g(p_g), b(p_b), a(p_a)
{
}

bool LitchiRuntime::Color::operator==(const Color & p_other)
{
	return this->r == p_other.r && this->g == p_other.g && this->b == p_other.b && this->a == p_other.a;
}

bool LitchiRuntime::Color::operator!=(const Color & p_other)
{
	return !operator==(p_other);
}
