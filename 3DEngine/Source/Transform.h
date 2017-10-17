#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

class Transform : public Component
{
public:
	Transform();
	Transform(Transform &transf);
	Transform(GameObject* parent, mat4x4 transf, bool isactive = true);
	~Transform();

	const mat4x4 GetRotMat() const;
	void GetEAnglesFromMat();

	void DrawComponent();

private:

	mat4x4 transform;
	vec3 position = {0.0f,0.0f,0.0f};
	vec3 angle = {0.0f, 0.0f, 0.0f};
	vec3 scale = {1.0f,1.0f,1.0f};
};

#endif // !__TRANSFORM_H__