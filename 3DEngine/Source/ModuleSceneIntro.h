#ifndef __MODULESCENEINTRO_H__
#define __MODULESCENEINTRO_H__


#include "Module.h"
#include "Primitive.h"
#include "Math.h"
#include "SDL\include\SDL.h"

#define MAX_SNAKE 2

class Body3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void Draw();

	// Add Mesh to Render
	void AddBody3D(Body3D* new_mesh);
	void ClearBody3DArray();
	void DrawBody3D() const;

public:

	std::vector<Body3D*> objects_3d;
	AABB* scene_bound_box = nullptr;
	vec3 bound_box_center;
	float dist = 0;

};

#endif // !__MODULESCENEINTRO_H__