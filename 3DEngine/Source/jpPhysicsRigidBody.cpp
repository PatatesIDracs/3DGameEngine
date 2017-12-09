#include "jpPhysicsRigidBody.h"


jpPhysicsRigidBody::jpPhysicsRigidBody(physx::PxPhysics* px_physics)
{
	//Create a default rigidbody 
	//default transform == identity
	//default geometry == sphere with 0.5 radius
	//default material: 0.5 friction(static and dynamic) and 0.5 restitution
	physx::PxMaterial* default_material = px_physics->createMaterial(0.5f, 0.5f, 0.5f);
	px_body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(), physx::PxSphereGeometry(0.5f), *default_material, 1.0f);
}

jpPhysicsRigidBody::~jpPhysicsRigidBody()
{
}

void jpPhysicsRigidBody::SetGeometry()
{

}