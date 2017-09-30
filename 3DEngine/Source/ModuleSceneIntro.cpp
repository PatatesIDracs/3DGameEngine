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
	col_test_BodyA->SetPos(-5,1,0);

	origin = { -5,2,0 };
	col_test_BodyB = new Sphere(origin, 1.5f);
	origin = { -7,2,0 };
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
	GLfloat vertices[] = { -1.f, 2.f, 1.f,		-1.f, 0.f, 1.f,		1.f, 0.f, 1.f,
							-1.f, 2.f, 1.f,		1.f, 0.f, 1.f,		1.f, 2.f, 1.f,
							1.f, 2.f, 1.f,		1.f, 0.f, 1.f,		1.f, 0.f, -1.f,
							1.f, 2.f, 1.f,		1.f, 0.f, -1.f,		1.f, 2.f, -1.f,
							1.f, 2.f, -1.f,		1.f, 0.f, -1.f,		-1.f, 0.f, -1.f,
							1.f, 2.f, -1.f,		-1.f, 0.f, -1.f,	-1.f, 2.f, -1.f,
							-1.f, 2.f, -1.f,	-1.f, 0.f, -1.f,	-1.f, 0.f, 1.f,
							-1.f, 2.f, -1.f,	-1.f, 0.f, 1.f,		-1.f, 2.f, 1.f,
							-1.f, 2.f, 1.f,		1.f, 2.f, 1.f,		1.f, 2.f, -1.f,
							-1.f, 2.f, 1.f,		1.f, 2.f, -1.f,		-1.f, 2.f, -1.f,
							-1.f, 0.f, 1.f,		-1.f, 0.f, -1.f,	1.f, 0.f, -1.f,
							-1.f, 0.f, 1.f,		1.f, 0.f, -1.f,		1.f, 0.f, 1.f };

	GLfloat uniquevertices[] = {5,0,0,
								5,1,0,
								6,0,0,
								6,1,0,
								6,1,-1,
								6,0,-1,
								5,1,-1,
								5,0,-1};

	GLubyte indices[] = {0,2,1,		1,2,3,		//Front
						3,2,5,		3,5,4,		//Right	
						1,3,4,		1,4,6,		//Top
						4,5,7,		4,7,6,		//Back
						7,0,1,		7,1,6,		//Left	
						7,2,0,		7,5,2};		//Bottom


	
	glGenBuffers(1, (GLuint*)&cube_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, &vertices[0], GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, cube_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, uniquevertices);

	// draw a cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);


	/*
	glGenBuffers(1, (GLuint*)&indices_id);
	glBindBuffer(GL_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*12 , indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, indices_id);
	glDrawElements(GL_TRIANGLES, indices_id, GL_UNSIGNED_INT, NULL);
	*/

	glDeleteBuffers(1, &cube_id);
	glDeleteBuffers(1, &indices_id);

	col_test_BodyA->Render();
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOGC("Hit!");
}

