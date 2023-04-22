
#include "transform.h"

Transform::Transform():Component(),
	position_(0.f),
	rotation_(glm::quat(1,0,0,0)),
	scale_(1.f)
{
}

Transform::~Transform() {
}
