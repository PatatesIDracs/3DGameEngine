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

	////true == dynamic, fale == Kinematic(Static)
	void SetDynamic(bool is_dynamic);
	void SetTransform(float* trans_mat);
	void SetGeometry(physx::PxGeometry new_geometry); //DO NOT USE, does nothing, will be later on defined to use with convexmesh
	void SetBoxGeometry(float x_scale, float y_scale, float z_scale);	//Regular scale, the method will turn it into halfExtents
	void SetSphereGeometry(float radius);
	void SetMaterial(float static_friction, float dynamic_friction, float restitution);

	void SetShape(physx::PxShape* new_shape);

	void GetTransform(physx::PxVec3& pos, physx::PxQuat& quat);

public:
	physx::PxRigidDynamic* px_body = nullptr;
	physx::PxShape* body_shape = nullptr;

	//Store a default material since we can't create a new one without the PxPhysics
	physx::PxMaterial* default_material = nullptr;
};

//TODO: Optimitzation - For setGeometry methods don't recreate shape when geometryType is the same

#endif // !__JP_PHYSICS_RIGIDBODY_H__

