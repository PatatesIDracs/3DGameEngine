#ifndef __JP_PHYSICS_WORLD_H__
#define __JP_PHYSICS_WORLD_H__

#include "PhysX/Include/PxPhysicsAPI.h"

class jpPhysicsWorld
{
public:
	jpPhysicsWorld();
	~jpPhysicsWorld();

	physx::PxPhysics* CreateNewPhysicsWorld();

private:

};
#endif // !__JP_PHYSICS_WORLD_H__
