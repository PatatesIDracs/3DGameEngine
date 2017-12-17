#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"
#include "Imgui\ImGuizmo.h"

class Transform : public Component
{
public:
	Transform(GameObject* parent);
	~Transform();

	const float4x4 GetLocalTransform() const;
	const float4x4 GetGlobalTransform()const;
	const float* GetGlobalTransposed()const;
	const Quat GetRotQuat() const;
	const Quat CurrRotQuat() const;
	const Quat GetGlobalQuat() const;
	const Quat GetParentQuat() const;
	float3 GetPosition() const;
	float3 GetGlobalPos() const;
	float3 GetParentPos() const;
	float3 GetScale() const;
	float3 GetParentScale() const;
	float3 GetGlobalScale() const;
	
	void Update();
	void UpdateGlobalTransform();
	void EnableUpdateTransform();
	void OnGuizmo();

	void SetTransform(float4x4 &transf);
	void SetTransform(float3 pos, Quat rot);
	void SetPosition(float3 new_pos);
	void SetScale(float3 new_scale);

	void DrawComponent();

	void Save(const char * buffer_data, char * cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:

	void NormalizeRotationAngle();


private:

	float4x4 transform, global_transform;
	Quat rotation = Quat::identity;

	Quat global_rotation = Quat::identity;
	float3 global_scale = float3(1, 1, 1);

	bool update_transform = false;

	float3 position = {0.0f,0.0f,0.0f};
	float3 angle = {0.0f, 0.0f, 0.0f};
	float3 scale = {1.0f,1.0f,1.0f};
};

#endif // !__TRANSFORM_H__