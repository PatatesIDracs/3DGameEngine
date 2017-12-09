#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "GameObject.h"
#include "Transform.h"

#include "jpPhysicsWorld.h"
#include "PhysX/Include/PxPhysicsAPI.h"

ModulePhysics::ModulePhysics(Application * app, bool start_enabled) : Module(app, "Physics", start_enabled)
{
	physics_world = new jpPhysicsWorld();

	physics_world->CreateNewPhysicsWorld();
	physics_world->CreateNewScene();

	mPhysics = physics_world->GetPhysicsWorld();
}

ModulePhysics::~ModulePhysics()
{
	delete physics_world;
}

bool ModulePhysics::Start()
{
	/*
	//Creating material
	physx::PxMaterial* mMaterial = mPhysics->createMaterial(0.5, 0.5, 0.5);	
	
	// Create Plane;
	physx::PxTransform planePos = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	nplane = mPhysics->createRigidStatic(planePos);
	nplane->createShape(physx::PxPlaneGeometry(), *mMaterial);
	mScene->addActor(*nplane);

	// Create cube
	physx::PxTransform boxPos(physx::PxVec3(0.0f, 10.0f, 0.0f));
	physx::PxBoxGeometry boxGeometry(physx::PxVec3(0.5f, 0.5f, 0.5f));
	box = physx::PxCreateDynamic(*mPhysics, boxPos, boxGeometry, *mMaterial,
		1.0f);
	mScene->addActor(*box);
	*/
	return true;
}

update_status ModulePhysics::Update(float dt)
{
	/*
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && !plane && !cube) {

		App->scene_intro->current_object = nullptr;
		plane = App->scene_intro->CreateBasicGeometry(PRIMITIVE_TYPES::PRIM_PLANE);
		plane->GetTransform()->SetScale(float3(20, 1, 20));
		
		App->scene_intro->current_object = nullptr;
		cube = App->scene_intro->CreateBasicGeometry(PRIMITIVE_TYPES::PRIM_CUBE);
		pos = box->getGlobalPose().p;
		float3 new_pos = float3(pos.x, pos.y, pos.z);
		cube->GetTransform()->SetPosition(new_pos);
		start_simulation = true;

	}
	*/

	if (dt != 0) {
		physics_world->Simulate(dt, 0);
		// Do things
	}
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	return true;
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
