#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

//#include "Math.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app,"Scene", start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOGC("Loading Intro assets");
	bool ret = true;


	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	vec origin = { 0, 1, 0 };
	col_test_BodyA = new oldSphere(1.0f);
	col_test_BodyA->SetPos(0,1,0);

	origin = { 0,2,0 };
	col_test_BodyB = new Sphere(origin, 1.5f);
	origin = { 2,2,0 };
	col_test_BodyC = new Sphere(origin, 1.0f);

	random_test = false;
	collision_test = false;


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	delete col_test_BodyA;
	delete col_test_BodyB;
	delete col_test_BodyC;

	//ImGui_ImplSdlGL3_Shutdown();

	return true;
}


// Update
update_status ModuleSceneIntro::Update(float dt)
{

	oldPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Draw()
{
	glBegin(GL_TRIANGLES);
	glVertex3f(-1.f, 2.f, 1.f);
	glVertex3f(-1.f, 0.f, 1.f);
	glVertex3f(1.f, 0.f, 1.f);

	glVertex3f(-1.f, 2.f, 1.f);
	glVertex3f(1.f, 0.f, 1.f);
	glVertex3f(1.f, 2.f, 1.f);

	glVertex3f(1.f, 2.f, 1.f);
	glVertex3f(1.f, 0.f, 1.f);
	glVertex3f(1.f, 0.f, -1.f);

	glVertex3f(1.f, 2.f, 1.f);
	glVertex3f(1.f, 0.f, -1.f);
	glVertex3f(1.f, 2.f, -1.f);

	glVertex3f(1.f, 2.f, -1.f);
	glVertex3f(1.f, 0.f, -1.f);
	glVertex3f(-1.f, 0.f, -1.f);

	glVertex3f(1.f, 2.f, -1.f);
	glVertex3f(-1.f, 0.f, -1.f);
	glVertex3f(-1.f, 2.f, -1.f);

	glVertex3f(-1.f, 2.f, -1.f);
	glVertex3f(-1.f, 0.f, -1.f);
	glVertex3f(-1.f, 0.f, 1.f);

	glVertex3f(-1.f, 2.f, -1.f);
	glVertex3f(-1.f, 0.f, 1.f);
	glVertex3f(-1.f, 2.f, 1.f);

	glVertex3f(-1.f, 2.f, 1.f);
	glVertex3f(1.f, 2.f, 1.f);
	glVertex3f(1.f, 2.f, -1.f);

	glVertex3f(-1.f, 2.f, 1.f);
	glVertex3f(1.f, 2.f, -1.f);
	glVertex3f(-1.f, 2.f, -1.f);

	glVertex3f(-1.f, 0.f, 1.f);
	glVertex3f(-1.f, 0.f, -1.f);
	glVertex3f(1.f, 0.f, -1.f);

	glVertex3f(-1.f, 0.f, 1.f);
	glVertex3f(1.f, 0.f, -1.f);
	glVertex3f(1.f, 0.f, 1.f);
	glEnd();
	
	//col_test_BodyA->Render();
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOGC("Hit!");
}

