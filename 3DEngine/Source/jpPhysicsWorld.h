#ifndef __JP_PHYSICS_WORLD_H__
#define __JP_PHYSICS_WORLD_H__

#include "PhysX/Include/PxPhysicsAPI.h"

class jpPhysicsWorld
{
public:
	jpPhysicsWorld();
	~jpPhysicsWorld();

	bool CreateNewPhysicsWorld();
	bool Simulate(float dt, unsigned int scene_index = 0);

	physx::PxScene* CreateNewScene();

	// Temporal functions
	physx::PxPhysics* GetPhysicsWorld();

private:
	// World, only one can be created
	physx::PxPhysics* jpWorld = nullptr;

	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
};
#endif // !__JP_PHYSICS_WORLD_H__
