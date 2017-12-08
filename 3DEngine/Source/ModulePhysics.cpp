#include "ModulePhysics.h"
#include "Application.h"

#include "PhysX/Include/PxPhysicsAPI.h"

#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3ExtensionsDEBUG.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3CommonDEBUG_x86.lib")

ModulePhysics::ModulePhysics(Application * app, bool start_enabled) : Module(app, "Physics", start_enabled)
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;
	
	physx::PxFoundation* mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	
	if (!mFoundation)
		LOGC("PxCreateFoundation failed!");
	
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), false, nullptr);
	
	if (!mPhysics)
		LOGC("PxCreatePhysics failed!");
}

ModulePhysics::~ModulePhysics()
{
	if (mPhysics != nullptr) {
		mPhysics->getFoundation().release();
		mPhysics->release();
	}
}

bool ModulePhysics::Start()
{
	return true;
}

update_status ModulePhysics::Update(float dt)
{
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
