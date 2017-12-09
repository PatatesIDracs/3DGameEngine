#ifndef __JP_PHYSICS_RIGIDBODY_H__
#define __JP_PHYSICS_RIGIDBODY_H__

#include "PhysX/Include/PxPhysicsAPI.h"

//Simple abstraction of the physX rigiddynamic class for an easier implementation with component system
class jpPhysicsRigidBody
{
public:
	jpPhysicsRigidBody(physx::PxPhysics* px_physics);
	~jpPhysicsRigidBody();

	void SetGeometry();

private:

	physx::PxRigidDynamic* px_body = nullptr;

	physx::PxShape* body_shape = nullptr;
};


#endif // !__JP_PHYSICS_RIGIDBODY_H__

