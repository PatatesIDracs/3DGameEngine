#ifndef __MODULECAMERA3D_H__
#define __MODULECAMERA3D_H__

#include "Module.h" 
#include "Camera.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void SetCameraEditor();
	void SetMainCamera(Camera* comp_camera, bool active);
	void ChangeCamera(bool mode_editor);
	void UpdateFov(int width, int height);
	Camera* GetMainCamera() const;

	void Look(const vec &Position, const vec &Reference, bool RotateAroundReference = false);
	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	void MoveTo(const vec &Movement, float distance);
	void RotateCamera(bool RotateAroundReference = true);
	void MoveCamera();

	float4x4 GetProjMatrix() const;
	float4x4 GetViewMatrix4x4() const;
	float* GetViewMatrix() const;

	void CameraRayCast();
	float3 CameraShotBall();

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json &config);
	void SaveModuleConfig(Config_Json &config);

	void DrawConfig();

public:

	vec X, Y, Z, Position, Reference;

	bool mode_editor = true;
	bool update_camera = false;

public:

	float distance = 0.5f;
	float speed = 10.0f;
	float last_dt = 0.0f;

	Camera* camera_editor = nullptr;
	Camera* main_camera = nullptr;

};

#endif // !__MODULECAMERA3D_H__