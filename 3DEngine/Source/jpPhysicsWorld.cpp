#include "jpPhysicsWorld.h"
//#include "Globals.h"
#include "Math.h"

#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3ExtensionsDEBUG.lib")
#pragma comment (lib, "PhysX/lib/vc14win32/PhysX3CommonDEBUG_x86.lib")

jpPhysicsWorld::jpPhysicsWorld()
{	
}

jpPhysicsWorld::~jpPhysicsWorld()
{
	if (jpWorld) {
		jpWorld->getFoundation().release();
		jpWorld->release();
	}
}

bool jpPhysicsWorld::CreateNewPhysicsWorld()
{
	if (!jpWorld) {
		physx::PxFoundation* jpFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
			gDefaultErrorCallback);

		if (jpFoundation)
			jpWorld = PxCreatePhysics(PX_PHYSICS_VERSION, *jpFoundation, physx::PxTolerancesScale(), false, nullptr);
		
		if (jpWorld)
			return true;
	}
	else return false;
}

bool jpPhysicsWorld::Simulate(float dt,unsigned int scene_index)
{
	if (jpWorld) {
		if (scene_index < jpWorld->getNbScenes())
		{
			physx::PxScene* scene[1];
			jpWorld->getScenes(scene, 1, scene_index);
			scene[0]->simulate(dt);
			scene[0]->fetchResults();
			return true;
		}
		else return false;
	}
	return false;
}

// Create Default Scene
physx::PxScene * jpPhysicsWorld::CreateNewScene()
{
	physx::PxSceneDesc sceneDesc(jpWorld->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	return jpWorld->createScene(sceneDesc);
}

physx::PxPhysics * jpPhysicsWorld::GetPhysicsWorld()
{
	return jpWorld;
}
