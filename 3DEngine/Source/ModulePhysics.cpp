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
	delete physics_world;
}

bool ModulePhysics::Start()
{
	//Creating material
	physx::PxMaterial* mMaterial = mPhysics->createMaterial(0.5, 0.5, 0.5);		
	// Create Plane;
	physx::PxTransform planePos = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	nplane = mPhysics->createRigidStatic(planePos);
	nplane->createShape(physx::PxPlaneGeometry(), *mMaterial);
	mScene->addActor(*nplane);
	
	return true;
}

update_status ModulePhysics::Update(float dt)
{
	physx::PxVec3 pos;
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && !plane && !cube) {

		App->scene_intro->current_object = nullptr;
		plane = App->scene_intro->CreateBasicGeometry(PRIMITIVE_TYPES::PRIM_PLANE);
		plane->GetTransform()->SetScale(float3(20, 1, 20));
		
		
		App->scene_intro->current_object = nullptr;
		cube = App->scene_intro->CreateBasicGeometry(PRIMITIVE_TYPES::PRIM_CUBE);

		RigidBody* planerbdy = (RigidBody*)App->scene_intro->NewOrphanComponent(COMP_RIGIDBODY);
		planerbdy->ChangeParent(cube);
		cube->AddComponent(planerbdy);
		jpPhysicsRigidBody* rbody = new jpPhysicsRigidBody(mPhysics);
		rbody->SetBoxGeometry(1,1,1);
		rbody->ActivateShape();
		mScene->addActor(*rbody->px_body);
		planerbdy->SetRigidBody(rbody);
	}
	

	// Update Physics World
	if (dt > 0) {
		physics_world->Simulate(dt);
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
