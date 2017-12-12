#ifndef __JP_PHYSICS_SHAPE_H__
#define __JP_PHYSICS_SHAPE_H__

#include "PhysX/Include/PxPhysicsAPI.h"

class jpPhysicsShape
{
public:
	jpPhysicsShape();
	~jpPhysicsShape();

private:

	physx::PxGeometry* rb_collider = nullptr;
};
#endif // !__JP_PHYSICS_SHAPE_H__
