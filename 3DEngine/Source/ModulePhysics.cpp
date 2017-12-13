#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "RigidBody.h"

#include "jpPhysicsWorld.h"
#include "PhysX/Include/PxPhysicsAPI.h"

#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3ExtensionsDEBUG.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3CommonDEBUG_x86.lib")

ModulePhysics::ModulePhysics(Application * app, bool start_enabled) : Module(app, "Physics", start_enabled)
{
	physics_world = new jpPhysicsWorld();

	physics_world->CreateNewPhysicsWorld();
	mScene = physics_world->CreateNewScene();

	mPhysics = physics_world->GetPhysicsWorld();
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{	
	return true;
}

update_status ModulePhysics::Update(float dt)
{
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

void ModulePhysics::LoadModuleConfig(Config_Json & config)
{
}

void ModulePhysics::SaveModuleConfig(Config_Json & config)
{
}

void ModulePhysics::DrawConfig()
{
}
