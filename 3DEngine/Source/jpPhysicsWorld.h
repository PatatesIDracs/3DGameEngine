#ifndef __JP_PHYSICS_WORLD_H__
#define __JP_PHYSICS_WORLD_H__

#include "PhysX/Include/PxPhysicsAPI.h"
#include "jpPhysicsRigidBody.h"

class jpPhysicsWorld
{
public:
	jpPhysicsWorld();
	~jpPhysicsWorld();

	bool CreateNewPhysicsWorld();
	bool Simulate(float dt);

	physx::PxScene* CreateNewScene();

	// Temporal functions
	physx::PxPhysics* GetPhysicsWorld();
	physx::PxCooking* GetCooking();

	jpPhysicsRigidBody* CreateRigidBody(physx::PxScene* curr_scene);

private:
	// World, only one can be created
	physx::PxPhysics* jpWorld = nullptr;

	physx::PxCooking* jpCooking = nullptr;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
};
#endif // !__JP_PHYSICS_WORLD_H__
