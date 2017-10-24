#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

class Transform : public Component
{
public:
	Transform(GameObject* parent);
	~Transform();

	const float4x4 GetRotMat() const;
	const Quat GetRotQuat();
	float3 GetPosition() const;
	
	void Update();
	void UpdateTransform();

	void SetTransform(float4x4 &transf);


	void DrawComponent();

private:

	float4x4 transform;
	Quat rotation = Quat::identity;

	bool update_transform = false;

	float3 position = {0.0f,0.0f,0.0f};
	float3 angle = {0.0f, 0.0f, 0.0f};
	float3 scale = {1.0f,1.0f,1.0f};
};

#endif // !__TRANSFORM_H__