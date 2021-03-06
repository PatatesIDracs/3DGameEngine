#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"
#include "MeshRenderer.h"

#define MIN_NEARP_DIST 0.5
#define MIN_FARP_DIST 25

enum CONTAINS_GOBJ_RESULT {
	CONT_INTERSECTS, 
	CONT_IN,
	CONT_OUT
};

class Camera : public Component
{
public: 
	Camera(GameObject* parent, bool isactive = true);
	~Camera();

	void Update();
	void UpdateTransform();

	void DrawComponent();

	float4x4 GetProjMatrix() const;
	float4x4 GetViewMatrix4x4() const;
	float* GetViewMatrix() const;
	void GetCameraVectors(float3& Position, float3& front, float3& up) const;

	Frustum& GetFrustum()const;
	bool GetFrustumGameObjecs(std::vector<GameObject*> &dynamic_array, std::vector<MeshRenderer*> &render_this) const;
	int ContainsAABB(const AABB &refBox) const;

	// Set Methods
	void SetFrustumPlanes();
	void SetFrustumPlanes(float nearp, float farp);
	void SetFrustumViewAngle();
	void SetFOVRatio(uint width, uint height);
	void SetNewFrame(vec& pos, vec& front, vec& up);
	void ChangeParent(GameObject* new_parent);

	void Save(const char* buffer_data, char* cursor, int& bytes_copied);
	void Load(char* cursor, int& bytes_copied);
	void GetOwnBufferSize(uint& buffer_size);

private:
	Frustum* cfrustum = nullptr;
	Plane* frustum_planes = nullptr;
	bool main_camera = false;

	float near_plane = 1.f;
	float far_plane = 50.f;
	float aspect_ratio = 0.f;
	int field_of_view = 60;
	
	bool own_transf = false;
};

#endif // !__CAMERA_H__

