#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Glew\include\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ConfigJSON.h"
#include "Imgui\imgui.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, "Renderer", start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOGC("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOGC("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOGC("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOGC("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOGC("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOGC("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);


		lights[0].Active(true);
	}

	return ret;
}

bool ModuleRenderer3D::Start()
{
	// Projection matrix for
	OnResize(App->window->width, App->window->height);

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetViewMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	switch (render_mode)
	{
	case FILL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case VERTEX:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	default:
		break;
	}
	
	SetLight();
	App->scene_intro->Draw();
	glDisable(GL_LIGHTING);

	App->physics->DrawPhysics();
	App->editor->Draw();

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOGC("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	App->window->height = height;
	App->window->width = width;
	App->camera->UpdateFov(width,height);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = App->camera->GetProjMatrix().Inverted();
	glLoadMatrixf(App->camera->GetViewMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// ----------------------------------------------
void ModuleRenderer3D::LoadModuleConfig(Config_Json & config)
{
	render_mode = (RENDER_MODE)config.GetInt("Render mode", RENDER_MODE::FILL);
	show_grid = config.GetBool("Show Grid", true);
	depth_test = config.GetBool("Depth Test", true);
	cull_face = config.GetBool("Cull face", true);
	cull_face_mode = config.GetInt("Cull face mode", 0);
	smooth = config.GetBool("Smooth", 1);
	lighting = config.GetBool("Lighting", true);
	color_material = config.GetBool("Color material", true);
	texture_2d = config.GetBool("Texture 2D", true);
	LightModelAmbient[0] = config.GetFloat("Red", 0.0f);
	LightModelAmbient[1] = config.GetFloat("Green", 0.0f);
	LightModelAmbient[2] = config.GetFloat("Blue", 0.0f);
	LightModelAmbient[3] = config.GetFloat("Alpha", 1.0f);

}

void ModuleRenderer3D::SaveModuleConfig(Config_Json & config)
{
	Config_Json render_config = config.AddJsonObject(this->GetName());
	render_config.SetBool("Is Active", true);
	render_config.SetInt("Render mode", render_mode);
	render_config.SetBool("Show Grid", show_grid);
	render_config.SetBool("Depth Test", depth_test);
	render_config.SetBool("Cull face", cull_face);
	render_config.SetInt("Cull face mode", cull_face_mode);
	render_config.SetBool("Smooth", smooth);
	render_config.SetBool("Lighting", lighting);
	render_config.SetBool("Color material", color_material);
	render_config.SetBool("Texture 2D", texture_2d);
	render_config.SetFloat("Red", LightModelAmbient[0]);
	render_config.SetFloat("Green", LightModelAmbient[1]);
	render_config.SetFloat("Blue", LightModelAmbient[2]);
	render_config.SetFloat("Alpha", LightModelAmbient[3]);

}

void ModuleRenderer3D::DrawConfig()
{
	ImGui::Text("Render mode: ");
	ImGui::RadioButton("Fill", (int*)&render_mode, (int)RENDER_MODE::FILL); ImGui::SameLine();
	ImGui::RadioButton("Wireframe", (int*)&render_mode, (int)RENDER_MODE::WIREFRAME); ImGui::SameLine();
	ImGui::RadioButton("Vertex", (int*)&render_mode, (int)RENDER_MODE::VERTEX); ImGui::Separator();

	ImGui::Checkbox("Show Grid", &show_grid);
	ImGui::Checkbox("Draw Octree", &show_octree);

	if (ImGui::Checkbox("Depth test", &depth_test)) SetDepthTest();

	if (ImGui::Checkbox("Face culling", &cull_face)) SetFaceCulling();
	if (ImGui::RadioButton("GL_CCW", &cull_face_mode, 0)) SetFaceCulling(); ImGui::SameLine();
	if (ImGui::RadioButton("GL_CW", &cull_face_mode, 1)) SetFaceCulling();


	if (ImGui::Checkbox("Smooth", &smooth)) SetSmooth();
	if (ImGui::Checkbox("Lighting", &lighting)) SetLight();

	if (ImGui::ColorEdit4("Light color", LightModelAmbient)) SetLightColor();

	if (ImGui::Checkbox("Color material", &color_material)) SetColorMaterial();
	if (ImGui::Checkbox("2D texture", &texture_2d)) Set2DTexture();

}

//Check all variables form config and set it right
//Should only be called once at the start of the program
void ModuleRenderer3D::CheckConfig()
{
	SetDepthTest();
	SetFaceCulling();
	SetSmooth();
	//SetLight();
	SetLightColor();
	SetColorMaterial();
	Set2DTexture();
}

void ModuleRenderer3D::SetDepthTest()
{
	if (depth_test)glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void ModuleRenderer3D::SetFaceCulling()
{
	if (cull_face)glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	if (cull_face_mode)glFrontFace(GL_CW);
	else glFrontFace(GL_CCW);
}

void ModuleRenderer3D::SetSmooth()
{
	if (smooth)glShadeModel(GL_SMOOTH);
	else glShadeModel(GL_FLAT);
}

void ModuleRenderer3D::SetLight()
{
	if (lighting)glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
}

void ModuleRenderer3D::SetLightColor()
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
}

void ModuleRenderer3D::SetColorMaterial()
{
	if (color_material)glEnable(GL_COLOR_MATERIAL);
	else glDisable(GL_COLOR_MATERIAL);
}

void ModuleRenderer3D::Set2DTexture()
{
	if (texture_2d) glEnable(GL_TEXTURE_2D);
	else glDisable(GL_TEXTURE_2D);
}