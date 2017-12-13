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
	physx::PxScene* GetScene(int scene_index) const;

	// Temporal functions
	physx::PxPhysics* GetPhysicsWorld();
	physx::PxCooking* GetCooking();

	//No Scene passed creates a sceneless rigidBody
	jpPhysicsRigidBody* CreateRigidBody(physx::PxScene* curr_scene = nullptr);

private:
	// World, only one can be created
	physx::PxPhysics* jpWorld = nullptr;
	physx::PxFoundation* jpFoundation = nullptr;

	//physx::PxCooking* jpCooking = nullptr;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
};
#endif // !__JP_PHYSICS_WORLD_H__
