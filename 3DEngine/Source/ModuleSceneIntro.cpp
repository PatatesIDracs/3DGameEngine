#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_sdl_gl3.h"

#include "Glew\include\glew.h"

#include "Math.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOGC("Loading Intro assets");
	bool ret = true;

	glewInit();
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGuiIO& io = ImGui::GetIO();

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOGC("Unloading Intro scene");

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	
	//Temp Imgui stuff-----------------

	ImGui_ImplSdlGL3_NewFrame(App->window->window);

	//Open a Gui window
	ImGui::Begin("Amazing menu to close the app"); 

	//Buttons to close the app and show the test window
	if (ImGui::Button("show test window")) showtestwindow = !showtestwindow;
	if(ImGui::Button("close app")) return UPDATE_STOP;

	if (showtestwindow) ImGui::ShowTestWindow();

	ImGui::End();

	//Rendre the UI
	ImGui::Render();

	//-----------------------------------

	oldPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOGC("Hit!");
}

