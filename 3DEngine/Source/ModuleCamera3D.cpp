#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ConfigJSON.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, "Camera", start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(3.0f, 3.0f, 3.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	// Set Angle	
	angle = 45;
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOGC("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOGC("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
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
		else Position -= Z*wheelmotion;
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

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

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

void ModuleCamera3D::MoveTo(const vec3 & Movement, float distance)
{
	Position -= Reference;
	Reference = Movement;

	this->distance = 1.2*distance;
	Position = Reference + Z*distance*2;

	CalculateViewMatrix();
}

void ModuleCamera3D::RotateCamera(bool onpoint)
{

	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 0.25f;

	if(onpoint) 
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

	if(onpoint) Position = Reference + Z * length(Position);
	else Reference = Position - Z*length(Position - Reference);
}

void ModuleCamera3D::MoveCamera(float dt)
{
	int speed = 8;
	int dx = 0;
	int dy = 0;
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
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// ----------------------------------------------
void ModuleCamera3D::LoadModuleConfig(Config_Json & config)
{
}

void ModuleCamera3D::SaveModuleConfig(Config_Json & config)
{
	Config_Json camera_config = config.AddJsonObject(this->GetName());
	camera_config.SetBool("Is Active", true);
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}