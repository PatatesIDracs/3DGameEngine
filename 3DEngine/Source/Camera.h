#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"

#define MIN_NEARP_DIST 0.5
#define MIN_FARP_DIST 25

class Camera : public Component
{
public: 
	Camera(GameObject* parent, bool isactive);
	~Camera();

	void Update();
	void UpdateTransform();

	void DrawComponent();

	float* GetViewMatrix();

	void GenerateFrostumDraw();

	// Set Methods
	void SetFrustumPlanes();
	void SetFrustumPlanes(float nearp, float farp);
	void SetFrustumViewAngle();
	void SetFOVRatio(uint width, uint height);
	void SetNewFrame(vec& pos, vec& front, vec& up);

private:
	Frustum* cfrustum = nullptr;
	
	float near_plane = 1.f;
	float far_plane = 50.f;
	int field_of_view = 60;
	float aspect_ratio = 0.f;

	uint findices_id = 0;
	uint fvertices_id = 0;
	
};

#endif // !__CAMERA_H__

