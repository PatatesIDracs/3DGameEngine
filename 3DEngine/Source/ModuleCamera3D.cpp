#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "ConfigJSON.h"
#include "Imgui\imgui.h"
#include "Imgui\ImGuizmo.h"

#include "Glew\include\glew.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, "Camera", start_enabled)
{
	Position = vec(1.0f, 1.0f, 1.0f);
	Reference = vec(0.0f, 0.0f, 0.0f);

	LookAt(float3(0, 0, 0));
}

ModuleCamera3D::~ModuleCamera3D()
{
	if(camera_editor != nullptr) delete camera_editor;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOGC("Cleaning camera");

	return true;
}

void ModuleCamera3D::SetCameraEditor()
{
	camera_editor = new Camera(nullptr, true);
	camera_editor->SetFrustumPlanes(0.5, 512);
	camera_editor->SetFrustumViewAngle();
	camera_editor->SetNewFrame(Position, Z, Y);
}

void ModuleCamera3D::SetMainCamera(Camera* comp_camera, bool active)
{
	if (active) {
		if (main_camera != nullptr && main_camera != comp_camera) main_camera->Disable();
		main_camera = comp_camera;
		main_camera->SetFOVRatio(App->window->width, App->window->height);
	}
	else if (main_camera == comp_camera) main_camera = nullptr;
}

void ModuleCamera3D::ChangeCamera(bool mode_editor)
{
	this->mode_editor = mode_editor;
	if (mode_editor) {
		camera_editor->GetCameraVectors(Position, Z, Y);
	}
	else {
		GetMainCamera()->GetCameraVectors(Position, Z, Y);
	}
	Reference = Position;
	X = Y.Cross(Z);

	update_camera = true;
}

void ModuleCamera3D::UpdateFov(int width, int height)
{

	if (main_camera != nullptr) 
		main_camera->SetFOVRatio(width, height);
	if (camera_editor != nullptr)
		camera_editor->SetFOVRatio(width, height);
}

Camera * ModuleCamera3D::GetMainCamera() const
{
	if (main_camera != nullptr) return main_camera;
	else return camera_editor;
}

bool ModuleCamera3D::Start()
{
	SetCameraEditor();
	return true;
}

update_status ModuleCamera3D::PreUpdate(float dt)
{
	// Recalculate matrix -------------
	if (update_camera) {
		if (mode_editor) {
			camera_editor->SetNewFrame(Position, Z, Y);
		}
		else GetMainCamera()->SetNewFrame(Position, Z, Y);

		update_camera = false;
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{	
	//If ImGui is using inputs don't use the camera
	if (App->input->IsImGuiUsingInput() || (ImGui::GetIO().WantCaptureMouse && ImGuizmo::IsOver())) return UPDATE_CONTINUE;

	last_dt = App->clock.real_delta_time;
	
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		App->scene_intro->LookAtScene();
	}

	// Mouse motion ----------------
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		RotateCamera();
	}	

	// Right Click + WASD to Move like FPS 
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		RotateCamera(false);
		MoveCamera();
	}

	// Zoom in and out
	int wheelmotion = App->input->GetMouseZ();
	if (wheelmotion != 0)
	{
		Position += Z*(float)wheelmotion;
		update_camera = true;
	}

	if (!update_camera && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->clock.state != APPSTATE::APP_PLAY)
	{
		CameraRayCast();
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec &Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = vec(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	update_camera = true;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec &Spot)
{
	Reference = Spot;

	Z = -(Position - Reference).Normalized();
	X = vec(0.0f,1.0f,0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	update_camera = true;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec &Movement)
{
	Position += Movement;
	Reference += Movement;

	update_camera = true;
}

void ModuleCamera3D::MoveTo(const vec &Movement, float distance)
{
	Position -= Reference;
	Reference = Movement;

	this->distance = (float)1.2*distance;
	Position = Reference + Z*distance * 2;

	update_camera = true;
}

void ModuleCamera3D::RotateCamera(bool RotateAroundReference)
{

	int dx = -App->input->GetMouseXMotion();
	int dy = App->input->GetMouseYMotion();

	float Sensitivity = 0.25f;

	if (RotateAroundReference)
		Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity*DEGTORAD;

		float3x3 rotate = float3x3::RotateAxisAngle(vec(0.0f, 1.0f, 0.0f), DeltaX);
		X = rotate * X;
		Y = rotate * Y;
		Z = rotate * Z;		
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity*DEGTORAD;

		float3x3 rotate = float3x3::RotateAxisAngle(X, DeltaY);

		Y = rotate * Y;
		Z = rotate * Z;

		if (Y.y < 0.0f)
		{
			Z = vec(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = Z.Cross(X);
		}
	}

	if (RotateAroundReference) Position = Reference + Z *Position.Length();
	else Reference = Position - Z*(Position - Reference).Length();

	update_camera = true;
}

void ModuleCamera3D::MoveCamera()
{
	float dx = 0;
	float dy = 0;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) dx = speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) dx = -speed;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) dy = speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) dy = -speed;

	if (dx != 0)
	{
		Position += X*dx*last_dt;
		Reference += X*dx*last_dt;
	}
	if (dy != 0)
	{
		Position += Z*dy*last_dt;
		Reference += Z*dy*last_dt;
	}

	update_camera = true;
}

float4x4 ModuleCamera3D::GetProjMatrix() const
{
	if (mode_editor) return camera_editor->GetProjMatrix();
	else return GetMainCamera()->GetProjMatrix();
}

float4x4 ModuleCamera3D::GetViewMatrix4x4() const
{
	return camera_editor->GetViewMatrix4x4();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix() const
{
	if (mode_editor) return camera_editor->GetViewMatrix();
	else return GetMainCamera()->GetViewMatrix();
}

void ModuleCamera3D::CameraRayCast()
{
	float y = (float)(-App->input->GetMouseY() + App->window->height*0.5f) / (App->window->height*0.5f);
	float x = (float)(App->input->GetMouseX() - App->window->width*0.5f) / (App->window->width*0.5f);

	// Camera Ray Cast
	App->scene_intro->CheckRayCastCollision(camera_editor->GetFrustum().UnProjectLineSegment(x, y));
}

float3 ModuleCamera3D::CameraShotBall()
{
	float y = (float)(-App->input->GetMouseY() + App->window->height*0.5f) / (App->window->height*0.5f);
	float x = (float)(App->input->GetMouseX() - App->window->width*0.5f) / (App->window->width*0.5f);

	// Camera Ray Cast
	LineSegment dir = GetMainCamera()->GetFrustum().UnProjectLineSegment(x, y);
	if(dir.IsFinite())
		return dir.Dir();
	else return float3(0.f, 0.f, 1.f);
}

// ----------------------------------------------
void ModuleCamera3D::LoadModuleConfig(Config_Json & config)
{
	speed = config.GetFloat("Speed", 10.0f);
}

void ModuleCamera3D::SaveModuleConfig(Config_Json & config)
{
	Config_Json camera_config = config.AddJsonObject(this->GetName());
	camera_config.SetBool("Is Active", true);
	camera_config.SetInt("Speed", (int)speed);
}

void ModuleCamera3D::DrawConfig()
{
	ImGui::SliderFloat("Speed", &speed, 8.0f, 30.0f);
}