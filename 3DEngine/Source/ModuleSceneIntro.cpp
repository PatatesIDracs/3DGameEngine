#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#include "3DModel.h"

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
	
	scene_bound_box = new AABB();
	return ret;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	oldPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	// Get Camera Focus
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		//Set Focus To GameObject or Origin if there's non
		if (objects_3d.size() > 0)
		{
		
			App->camera->MoveTo(bound_box_center, dist);
		}
		else App->camera->MoveTo(vec3(0.0f, 0.0f, 0.0f), 2);
	}

	return UPDATE_CONTINUE;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	for (uint count = 0; count < objects_3d.size(); count++)
	{
		delete objects_3d[count];
	}
	objects_3d.clear();
	
	delete scene_bound_box;
	return true;
}

void ModuleSceneIntro::Draw()
{
	DrawBody3D();
}

void ModuleSceneIntro::AddBody3D(Body3D * gameobject)
{
	// Fit new mesh inside the Scene Boundary Box 
	// We don't use GameObjects yet but Scene_boundary_Box will be the "Parent Boundary Box"
	// which enclose all it's childrens
	const BodyMesh* mesh = gameobject->GetMesh();
	for (uint i = 0; i < mesh->num_vertices * 3; i += 3)
	{
		scene_bound_box->Enclose(vec(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]));
		if (i == 0) scene_bound_box->minPoint = scene_bound_box->maxPoint;
	}

	// Calculate Center of the Boundary Box and distance from center to vertex;
	vec center = (scene_bound_box->minPoint + scene_bound_box->maxPoint)*0.5;
	bound_box_center = vec3(center.x, center.y, center.z);
	vec ret = scene_bound_box->HalfSize();
	dist = sqrt(ret.x*ret.x + ret.y*ret.y + ret.z*ret.z);

	// Focus Camera on GameObject;
	App->camera->MoveTo(bound_box_center, dist);

	

	objects_3d.push_back(gameobject);
}

void ModuleSceneIntro::ClearBody3DArray()
{
	for (uint count = 0; count < objects_3d.size(); count++)
	{
		delete objects_3d[count];
	}
	objects_3d.clear();

	// Reset BoundaryBox
	delete scene_bound_box;
	scene_bound_box = new AABB();
}

void ModuleSceneIntro::DrawBody3D() const
{
	for (uint i = 0; i < objects_3d.size(); i++)
	{
		objects_3d[i]->Render();
	}
}

// This methods are here for the assignment, will be included inside Game Object Components;
//-------------------------------------------------

// Transform Data ---------------------------------
float3 ModuleSceneIntro::GetAngles() const
{
	float3 ret = {0.0f,0.0f,0.0f};
	if (objects_3d.size() > 0)
	{
		ret = objects_3d[0]->GetTransfAngles();
	}
	return ret;
}

vec3 ModuleSceneIntro::GetScale() const
{
	vec3 ret = { 0.0f,0.0f,0.0f };
	if (objects_3d.size() > 0)
	{
		ret = objects_3d[0]->GetTransfScale();
	}
	return ret;
}

vec3 ModuleSceneIntro::GetPosition() const
{
	vec3 ret = { 0.0f,0.0f,0.0f };
	if (objects_3d.size() > 0)
	{
		ret = objects_3d[0]->GetTransfPosition();
	}
	return ret;
}

// Geometry Data -------------------------------------
uint ModuleSceneIntro::GetVertex() const
{
	uint ret = 0;
	for (uint i = 0; i < objects_3d.size(); i++)
	{
		ret += (objects_3d[i]->GetMesh().num_vertices);
	}
	return ret;
}

uint ModuleSceneIntro::GetFaces() const
{
	uint ret = 0;
	for (uint i = 0; i < objects_3d.size(); i++)
	{
		ret += (objects_3d[i]->GetMesh().num_indices / 3);
	}
	return ret;
}

// Texture Data ---------------------------------------
uint ModuleSceneIntro::GetWidth() const
{
	return texture_width;
}

uint ModuleSceneIntro::GetHeight() const
{
	return texture_height;
}

void ModuleSceneIntro::SetTexSize(uint width, uint height)
{
	texture_width = width;
	texture_height = height;
}

void ModuleSceneIntro::ChangeTexture(uint new_texture_id)
{
	for (uint i = 0; i < objects_3d.size(); i++)
	{
		objects_3d[i]->SetTexture(new_texture_id);
	}
	if (objects_3d.size() == 0)
		glDeleteTextures(1, &new_texture_id);
}
