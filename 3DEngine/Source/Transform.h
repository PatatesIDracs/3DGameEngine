#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

class Transform : public Component
{
public:
	Transform(GameObject* parent);
	~Transform();

	const float4x4 GetLocalTransform() const;
	const float4x4 GetGlobalTransform()const;
	const float* GetGlobalTransposed()const;
	const Quat GetRotQuat() const;
	float3 GetPosition() const;
	float3 GetScale() const;
	
	void Update();
	//void UpdateTransform();

	void SetTransform(float4x4 &transf);

	void DrawComponent();

private:

	float4x4 transform, global_transform;
	Quat rotation = Quat::identity;

	bool update_transform = false;

	float3 position = {0.0f,0.0f,0.0f};
	float3 angle = {0.0f, 0.0f, 0.0f};
	float3 scale = {1.0f,1.0f,1.0f};
};

#endif // !__TRANSFORM_H__