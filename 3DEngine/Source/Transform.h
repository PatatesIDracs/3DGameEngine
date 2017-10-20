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
	const Quat GetRotQuat();
	
	void SetPosition();
	void SetRotation();
	void SetScale();

	void GetEAnglesFromMat();

	void DrawComponent();

private:

	mat4x4 transform;
	Quat rotation = Quat::identity;

	vec3 position = {0.0f,0.0f,0.0f};
	vec3 angle = {0.0f, 0.0f, 0.0f};
	vec3 scale = {1.0f,1.0f,1.0f};

	vec3 rot_angle = { 0.f,0.f,0.f };
};

#endif // !__TRANSFORM_H__