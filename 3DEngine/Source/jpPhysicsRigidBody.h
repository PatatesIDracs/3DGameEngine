#ifndef __JP_PHYSICS_RIGIDBODY_H__
#define __JP_PHYSICS_RIGIDBODY_H__

#include "PhysX/Include/PxPhysicsAPI.h"

//Simple abstraction of the physX rigiddynamic class for an easier implementation with component system
class jpPhysicsRigidBody
{
public:
	jpPhysicsRigidBody(physx::PxPhysics* px_physics);
	~jpPhysicsRigidBody();

	void ActivateShape();

	void SetDynamic(bool is_dynamic);
	void SetTransform(float* trans_mat);
	void SetGeometry(physx::PxGeometry new_geometry); //DO NOT USE, does nothing, will be later on defined to use with convexmesh
	void SetBoxGeometry(float x_scale, float y_scale, float z_scale);	//Regular scale, the method will turn it into halfExtents
	void SetMaterial(float static_friction, float dynamic_friction, float restitution);

	void SetShape(physx::PxShape* new_shape);

	void GetTransform(physx::PxVec3& pos, physx::PxQuat& quat);

public:
	physx::PxMaterial* body_material = nullptr;
	physx::PxRigidDynamic* px_body = nullptr;
	physx::PxShape* body_shape = nullptr;
};


#endif // !__JP_PHYSICS_RIGIDBODY_H__

