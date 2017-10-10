#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

#include "glmath.h"

class Transform : public Component
{
public:
	Transform();
	Transform(GameObject* parent, mat4x4 transf, vec3 pos, vec3 eu_angles, vec3 scale, bool isactive = true);
	~Transform();

private:

	mat4x4 transform;
	vec3 position = {0.0f,0.0f,0.0f};
	vec3 angle = {0.0f, 0.0f, 0.0f};
	vec3 scale = {1.0f,1.0f,1.0f};
};

#endif // !__TRANSFORM_H__