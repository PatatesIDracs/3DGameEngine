#ifndef __MODULESCENEINTRO_H__
#define __MODULESCENEINTRO_H__


#include "Module.h"
#include "Primitive.h"
#include "Math.h"
#include "SDL\include\SDL.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class Mesh_data;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Draw();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:


	bool	showtestwindow = false;
	bool	random_test = false;
	bool	collision_test = false;

	float	float_rand_test = 0.0f;
	int		int_rand_test = 0;

	oldSphere* col_test_BodyA = nullptr;
	Sphere* col_test_BodyB = nullptr;
	Sphere* col_test_BodyC = nullptr;
	
	uint cube_id = 0;
	uint indices_id = 0;
	uint unique_cube_id = 0;

	std::vector<Mesh_data> meshes;
};

#endif // !__MODULESCENEINTRO_H__