#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ConfigJSON.h"

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "RbCollider.h"
#include "RigidBody.h"

#include "jpPhysicsWorld.h"
#include "PhysX/Include/PxPhysicsAPI.h"
#include "Glew/include/glew.h"
/*
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3ExtensionsDEBUG.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3CommonDEBUG_x86.lib")*/

ModulePhysics::ModulePhysics(Application * app, bool start_enabled) : Module(app, "Physics", start_enabled)
{
	physics_world = new jpPhysicsWorld();

	physics_world->CreateNewPhysicsWorld();
	mScene = physics_world->CreateNewScene();
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);

	mPhysics = physics_world->GetPhysicsWorld();
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{	
	shot_balls.reserve(20);

	return true;
}

update_status ModulePhysics::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN && App->scene_intro->current_object) {
		RigidBody* rbody =(RigidBody*) App->scene_intro->current_object->FindFirstComponent(COMP_RIGIDBODY);
		if (rbody) {
			rbody->GetPhysicsBody()->ApplyImpulse(physx::PxVec3(2, 0, 0));
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN && App->scene_intro->current_object) {
		RigidBody* rbody = (RigidBody*)App->scene_intro->current_object->FindFirstComponent(COMP_RIGIDBODY);
		if (rbody) {
			rbody->GetPhysicsBody()->ApplyForce(physx::PxVec3(2, 0, 0));
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN && App->scene_intro->current_object) {
		RigidBody* rbody = (RigidBody*)App->scene_intro->current_object->FindFirstComponent(COMP_RIGIDBODY);
		if (rbody) {
			rbody->GetPhysicsBody()->ApplyTorqueForce(physx::PxVec3(0, 10, 0));
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN && App->scene_intro->current_object) {
		RigidBody* rbody = (RigidBody*)App->scene_intro->current_object->FindFirstComponent(COMP_RIGIDBODY);
		if (rbody) {
			rbody->GetPhysicsBody()->ApplyTorqueImpulse(physx::PxVec3(0, -2, 0));
		}
	}

	if (App->clock.state == APP_PLAY && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		ShotBalls();
	}
	if (App->clock.state != APP_PLAY && shot_balls.size() > 0) {
		shot_balls.clear();
		curr_ball = 0;
	}


	// Update Physics World
	if (dt > 0) {
		physics_world->Simulate(dt);
	}



	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	if (physics_world)
		delete physics_world;
	return true;
}

jpPhysicsRigidBody * ModulePhysics::GetNewRigidBody(int scene_to_load_it)
{
	if (scene_to_load_it == -1)
		return physics_world->CreateRigidBody();
	else
		return physics_world->CreateRigidBody(physics_world->GetScene(scene_to_load_it));
}

void ModulePhysics::SetDebugCullingLimits(AABB & box)
{
	if (box.IsFinite()) {
		mScene->setVisualizationCullingBox(physx::PxBounds3(physx::PxVec3(box.MinX(), box.MinY(), box.MinZ()), physx::PxVec3(box.MaxX(), box.MaxY(), box.MaxZ())));
	}
}

void ModulePhysics::LoadModuleConfig(Config_Json & config)
{
	render_physics = config.GetBool("Render physics", render_physics);
	render_on_play = config.GetBool("Render on play", render_on_play);
}

void ModulePhysics::SaveModuleConfig(Config_Json & config)
{
	Config_Json physics_config = config.AddJsonObject(this->GetName());
	physics_config.SetBool("Render physics", render_physics);
	physics_config.SetBool("Render on play", render_on_play);
}

void ModulePhysics::DrawConfig()
{
	if (ImGui::Checkbox("Render physics", &render_physics))
	{
		if (render_physics)
		{
			mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
		}
		else
		{
			render_on_play = false;
			mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 0.0f);
		}
	}
	ImGui::Checkbox("Render on play", &render_on_play);
}

void ModulePhysics::ShotBalls()
{
	App->scene_intro->current_object = nullptr;

	// Create Ball and set position to camera pos
	GameObject* new_ball = App->scene_intro->CreateBasicGeometry(PRIMITIVE_TYPES::PRIM_SPHERE);
	
	GameObject* camera = App->camera->GetMainCamera()->GetParent();
	if (camera != nullptr) {
		new_ball->SetTransform((float4x4)camera->GetTransform()->GetGlobalTransform().Transposed());
	}
	// Get Shot Direction from camera raycast
	float3 dir = App->camera->CameraShotBall();

	// Set the ball out of camera collider
	new_ball->GetTransform()->SetPosition(new_ball->GetTransform()->GetPosition() + 2 * dir);
	new_ball->GetTransform()->SetScale(float3(0.5f, 0.5f, 0.5));

	// Create Physics Components, default values will be enough
	RbCollider* collider = new RbCollider(new_ball);
	RigidBody* rigid_body = new RigidBody(new_ball);

	// Apply Force to the ball
	dir *= 1000;
	rigid_body->GetPhysicsBody()->ApplyForce(physx::PxVec3(dir.x, dir.y, dir.z));


	// Add Last Shot to Vector
	if (curr_ball < 20) {
		shot_balls.push_back(new_ball);
	}
	else {
		int place = curr_ball%shot_balls.size();
		GameObject* remove_this = shot_balls[place];
		// might crush;
		App->scene_intro->DeleteGameObject(remove_this);
		shot_balls[place] = new_ball;
	}
	curr_ball++;
}

void ModulePhysics::DrawPhysics()
{
	if ((!render_on_play && App->clock.state == APP_PLAY) || !render_physics) return;

	if (App->clock.state == APP_STOP)
	{
		mScene->simulate((physx::PxReal)VERY_LITTLE_NUMBER);
		mScene->fetchResults(true);
	}
	const physx::PxRenderBuffer& rb = mScene->getRenderBuffer();

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (uint i = 0; i < rb.getNbLines(); i++)
	{
		const physx::PxDebugLine& line = rb.getLines()[i];

		glVertex3f(line.pos0.x, line.pos0.y, line.pos0.z);
		glVertex3f(line.pos1.x, line.pos1.y, line.pos1.z);
	}
	glEnd();
	glLineWidth(1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}
