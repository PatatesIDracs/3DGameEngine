#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "ConfigJSON.h"
#include "Imgui\imgui.h"

#include "Glew\include\glew.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, "Camera", start_enabled)
{

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(3.0f, 3.0f, 3.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	// Set Angle	
	angle = 45;
}

ModuleCamera3D::~ModuleCamera3D()
{
	if(camera_editor != nullptr) delete camera_editor;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOGC("Setting up the camera");
	bool ret = true;
	glewInit();

	return ret;
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
	camera_editor->SetFrustumPlanes(0.5, 100);
	camera_editor->SetFrustumViewAngle();
	camera_editor->GenerateFrostumDraw();
	camera_editor->SetNewFrame(vec(Position.x, Position.y, Position.z), vec(-Z.x, -Z.y, -Z.z), vec(Y.x, Y.y, Y.z));
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{	
	update_camera = false;

	//If ImGui is using inputs don't use the camera
	if (App->input->IsImGuiUsingInput()) return UPDATE_CONTINUE;

	// Mouse motion ----------------
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		RotateCamera();
	}

	// Right Click + WASD to Move like FPS 
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		RotateCamera(false);
		MoveCamera(dt);
	}

	// Zoom in and out
	int wheelmotion = App->input->GetMouseZ();
	if (wheelmotion != 0)
	{
		if (wheelmotion > 0 && length(Position - Reference) < distance) {}
		else Position -= Z*(float)wheelmotion;

		update_camera = true;
	}

	// Recalculate matrix -------------
	if (mode_editor && update_camera) camera_editor->SetNewFrame(vec(Position.x, Position.y, Position.z), vec(-Z.x, -Z.y, -Z.z), vec(Y.x, Y.y, Y.z));

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	update_camera = true;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	update_camera = true;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	update_camera = true;
}

void ModuleCamera3D::MoveTo(const vec3 & Movement, float distance)
{
	Position -= Reference;
	Reference = Movement;

	this->distance = (float)1.2*distance;
	Position = Reference + Z*distance * 2;

	update_camera = true;
}

void ModuleCamera3D::RotateCamera(bool onpoint)
{

	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 0.25f;

	if (onpoint)
		Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	if (onpoint) Position = Reference + Z * length(Position);
	else Reference = Position - Z*length(Position - Reference);

	update_camera = true;
}

void ModuleCamera3D::MoveCamera(float dt)
{
	float dx = 0;
	float dy = 0;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) dx = -speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) dx = speed;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) dy = -speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) dy = speed;

	if (dx != 0)
	{
		Position += X*dx*dt;
		Reference += X*dx*dt;
	}
	if (dy != 0)
	{
		Position += Z*dy*dt;
		Reference += Z*dy*dt;
	}

	update_camera = true;
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	 return camera_editor->GetViewMatrix();
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