#include "Transform.h"

Transform::Transform() : Component(nullptr, COMP_TRANSFORM, true)
{
	transform = mat4x4();
}

Transform::Transform(GameObject* parent, mat4x4 transf, vec3 pos, vec3 eu_angles, vec3 scale, bool isactive) : Component(parent, COMP_TRANSFORM, isactive),
transform(transf), position(pos), angle(eu_angles), scale(scale)
{}

Transform::~Transform()
{
}
