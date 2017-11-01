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
	bool CleanUp();

	void SetCameraEditor();

	void Look(const vec &Position, const vec &Reference, bool RotateAroundReference = false);
	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	void MoveTo(const vec &Movement, float distance);
	void RotateCamera(bool RotateAroundReference = true);
	void MoveCamera(float dt);

	float4x4 GetProjMatrix() const;
	float* GetViewMatrix() const;

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json &config);
	void SaveModuleConfig(Config_Json &config);

	void DrawConfig();

public:

	vec X, Y, Z, Position, Reference;

	bool mode_editor = true;

private:

	float distance = 0.5f;
	float speed = 10.0f;

	Camera* camera_editor = nullptr;
	bool update_camera = false;

};

#endif // !__MODULECAMERA3D_H__