#ifndef __MODULECAMERA3D_H__
#define __MODULECAMERA3D_H__

#include "Module.h"
#include "glmath.h"

#include "Camera.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void MoveTo(const vec3 &Movement, float distance);
	void RotateCamera(bool onpoint = true);
	void MoveCamera(float dt);

	float* GetViewMatrix();

	// JSON Save/Load Configuration
	void LoadModuleConfig(Config_Json& config);
	void SaveModuleConfig(Config_Json& config);

	void DrawConfig();

private:

	void CalculateViewMatrix();

public:

	Camera* camera_editor = nullptr;
	
	bool mode_editor = true;

	vec3 X, Y, Z, Position, Reference;

	float speed = 10.0f;
	float distance = 0.5f;

	float angle = 0.0f;

private:
	//mat3x3 R;
	mat4x4 ViewMatrix, ViewMatrixInverse;
};

#endif // !__MODULECAMERA3D_H__